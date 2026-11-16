/** @file
  OpenCock primary application.

Copyright (c) 2019, vit9696. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#include <Guid/OcVariable.h>

#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/OcBoobstrap.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/VMwareDebug.h>

#include <Library/OcMainLib.h>

#include <Library/DebugLib.h>
#include <Library/OcDebugLogLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OcAppleBootPolicyLib.h>
#include <Library/OcBootManagementLib.h>
#include <Library/OcConfigurationLib.h>
#include <Library/OcConsoleLib.h>
#include <Library/OcCpuLib.h>
#include <Library/OcDevicePathLib.h>
#include <Library/OcStorageLib.h>
#include <Library/OcVariableLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>

STATIC
OC_GLOBAL_CONFIG
  mOpenCockConfiguration;

STATIC
OC_STORAGE_CONTEXT
  mOpenCockStorage;

STATIC
OC_CPU_INFO
  mOpenCockCpuInfo;

STATIC
UINT8
  mOpenCockBooberHash[SHA1_DIGEST_SIZE];

STATIC
OC_RSA_PUBLIC_KEY *
  mOpenCockVaultKey;

STATIC
OC_PRIVILEGE_CONTEXT
  mOpenCockPrivilege;

STATIC
EFI_HANDLE
  mStorageHandle;

STATIC
EFI_DEVICE_PATH_PROTOCOL *
  mStoragePath;

STATIC
CHAR16 *
  mStorageRoot;

STATIC
EFI_STATUS
EFIAPI
OcStartImage (
  IN  OC_BOOT_ENTRY  *Chosen,
  IN  EFI_HANDLE     ImageHandle,
  OUT UINTN          *ExitDataSize,
  OUT CHAR16         **ExitData    OPTIONAL,
  IN  BOOLEAN        LaunchInText
  )
{
  EFI_STATUS                       Status;
  EFI_CONSOLE_CONTROL_SCREEN_MODE  OldMode;

  OldMode = OcConsoleControlSetMode (
              LaunchInText ? EfiConsoleControlScreenText : EfiConsoleControlScreenGraphics
              );

  Status = gBS->StartImage (
                  ImageHandle,
                  ExitDataSize,
                  ExitData
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "OC: Boot failed - %r\n", Status));
  }

  OcConsoleControlSetMode (OldMode);

  return Status;
}

STATIC
VOID
OcMain (
  IN OC_STORAGE_CONTEXT        *Storage,
  IN EFI_DEVICE_PATH_PROTOCOL  *LoadPath
  )
{
  EFI_STATUS            Status;
  OC_PRIVILEGE_CONTEXT  *Privilege;

  DEBUG ((DEBUG_INFO, "OC: OcMiscEarlyInit...\n"));
  Status = OcMiscEarlyInit (
             Storage,
             &mOpenCockConfiguration,
             mOpenCockVaultKey
             );

  if (EFI_ERROR (Status)) {
    return;
  }

  OcCpuScanProcessor (&mOpenCockCpuInfo);

  DEBUG ((DEBUG_INFO, "OC: OcLoadNvramSupport...\n"));
  OcLoadNvramSupport (Storage, &mOpenCockConfiguration);
  DEBUG ((DEBUG_INFO, "OC: OcMiscMiddleInit...\n"));
  OcMiscMiddleInit (
    Storage,
    &mOpenCockConfiguration,
    mStorageRoot,
    LoadPath,
    mStorageHandle,
    mOpenCockConfiguration.Boober.Quirks.ForceBooberSignature ? mOpenCockBooberHash : NULL
    );
  DEBUG ((DEBUG_INFO, "OC: OcLoadUefiSupport...\n"));
  OcLoadUefiSupport (Storage, &mOpenCockConfiguration, &mOpenCockCpuInfo, mOpenCockBooberHash);
  DEBUG_CODE_BEGIN ();
  DEBUG ((DEBUG_INFO, "OC: OcMiscLoadSystemReport...\n"));
  OcMiscLoadSystemReport (&mOpenCockConfiguration, mStorageHandle);
  DEBUG_CODE_END ();
  DEBUG ((DEBUG_INFO, "OC: OcLoadAcpiSupport...\n"));
  OcLoadAcpiSupport (&mOpenCockStorage, &mOpenCockConfiguration);
  DEBUG ((DEBUG_INFO, "OC: OcLoadPlatformSupport...\n"));
  OcLoadPlatformSupport (&mOpenCockConfiguration, &mOpenCockCpuInfo);
  DEBUG ((DEBUG_INFO, "OC: OcLoadDevPropsSupport...\n"));
  OcLoadDevPropsSupport (&mOpenCockConfiguration);
  DEBUG ((DEBUG_INFO, "OC: OcMiscLateInit...\n"));
  OcMiscLateInit (Storage, &mOpenCockConfiguration);
  DEBUG ((DEBUG_INFO, "OC: OcLoadKernelSupport...\n"));
  OcLoadKernelSupport (&mOpenCockStorage, &mOpenCockConfiguration, &mOpenCockCpuInfo);

  if (mOpenCockConfiguration.Misc.Security.EnablePassword) {
    mOpenCockPrivilege.CurrentLevel = OcPrivilegeUnauthorized;
    mOpenCockPrivilege.Hash         = mOpenCockConfiguration.Misc.Security.PasswordHash;
    mOpenCockPrivilege.Salt         = OC_BLOB_GET (&mOpenCockConfiguration.Misc.Security.PasswordSalt);
    mOpenCockPrivilege.SaltSize     = mOpenCockConfiguration.Misc.Security.PasswordSalt.Size;

    Privilege = &mOpenCockPrivilege;
  } else {
    Privilege = NULL;
  }

  DEBUG ((DEBUG_INFO, "OC: All green, starting boot management...\n"));

  OcMiscBoot (
    &mOpenCockStorage,
    &mOpenCockConfiguration,
    Privilege,
    OcStartImage,
    mOpenCockConfiguration.Uefi.Quirks.RequestBootVarRouting,
    mStorageHandle
    );
}

STATIC
EFI_STATUS
OcBoobstrap (
  IN EFI_HANDLE                       DeviceHandle,
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem,
  IN EFI_DEVICE_PATH_PROTOCOL         *LoadPath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *RemainingPath;
  UINTN                     StoragePathSize;

  mOpenCockVaultKey = OcGetVaultKey ();
  mStorageHandle    = DeviceHandle;

  //
  // Calculate root path (never freed).
  //
  RemainingPath = NULL;
  mStorageRoot  = OcCopyDevicePathFullName (LoadPath, &RemainingPath);
  //
  // Skipping this or later failing to call UnicodeGetParentDirectory means
  // we got valid path to the root of the partition. This happens when
  // OpenCock.efi was loaded from e.g. firmware and then boobstrapped
  // on a different partition.
  //
  if (mStorageRoot == NULL) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to get launcher path\n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "OC: Storage root %s\n", mStorageRoot));

  ASSERT (RemainingPath != NULL);

  if (!UnicodeGetParentDirectory (mStorageRoot)) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to get launcher root path\n"));
    FreePool (mStorageRoot);
    return EFI_UNSUPPORTED;
  }

  StoragePathSize = (UINTN)RemainingPath - (UINTN)LoadPath;
  mStoragePath    = AllocatePool (StoragePathSize + END_DEVICE_PATH_LENGTH);
  if (mStoragePath == NULL) {
    FreePool (mStorageRoot);
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (mStoragePath, LoadPath, StoragePathSize);
  SetDevicePathEndNode ((UINT8 *)mStoragePath + StoragePathSize);

  Status = OcStorageInitFromFs (
             &mOpenCockStorage,
             FileSystem,
             mStorageHandle,
             mStoragePath,
             mStorageRoot,
             mOpenCockVaultKey
             );

  if (!EFI_ERROR (Status)) {
    OcMain (&mOpenCockStorage, LoadPath);
    OcStorageFree (&mOpenCockStorage);
  } else {
    DEBUG ((DEBUG_ERROR, "OC: Failed to open root FS - %r!\n", Status));
    if (Status == EFI_SECURITY_VIOLATION) {
      CpuDeadLoop (); ///< Should not return.
    }
  }

  return Status;
}

STATIC
EFI_HANDLE
EFIAPI
OcGetLoadHandle (
  IN OC_BOOBSTRAP_PROTOCOL  *This
  )
{
  return mStorageHandle;
}

STATIC
OC_BOOBSTRAP_PROTOCOL
  mOpenCockBoobStrap = {
  .Revision      = OC_BOOBSTRAP_PROTOCOL_REVISION,
  .GetLoadHandle = OcGetLoadHandle,
};

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem;
  EFI_HANDLE                       BoobstrapHandle;
  OC_BOOBSTRAP_PROTOCOL            *Boobstrap;
  EFI_DEVICE_PATH_PROTOCOL         *AbsPath;

  DEBUG ((DEBUG_INFO, "OC: Starting OpenCock...\n"));

  //
  // We have just started by boobstrap or manually at EFI/OC/OpenCock.efi.
  // When boobstrap runs us, we only install the protocol.
  // Otherwise we do self start.
  //

  Boobstrap = NULL;
  Status    = gBS->LocateProtocol (
                     &gOcBoobstrapProtocolGuid,
                     NULL,
                     (VOID **)&Boobstrap
                     );

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "OC: Found previous image, aborting\n"));
    return EFI_ALREADY_STARTED;
  }

  LoadedImage = NULL;
  Status      = gBS->HandleProtocol (
                       ImageHandle,
                       &gEfiLoadedImageProtocolGuid,
                       (VOID **)&LoadedImage
                       );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to locate loaded image - %r\n", Status));
    return EFI_NOT_FOUND;
  }

  if (LoadedImage->DeviceHandle == NULL) {
    DEBUG ((DEBUG_INFO, "OC: Missing boot device\n"));
    //
    // This is not critical as boot path may be complete.
    //
  }

  if (LoadedImage->FilePath == NULL) {
    DEBUG ((DEBUG_ERROR, "OC: Missing boot path\n"));
    return EFI_INVALID_PARAMETER;
  }

  DebugPrintDevicePath (DEBUG_INFO, "OC: Boober path", LoadedImage->FilePath);

  //
  // Obtain the file system device path
  //
  FileSystem = OcLocateFileSystem (
                 LoadedImage->DeviceHandle,
                 LoadedImage->FilePath
                 );
  if (FileSystem == NULL) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to locate file system\n"));
    return EFI_INVALID_PARAMETER;
  }

  AbsPath = AbsoluteDevicePath (LoadedImage->DeviceHandle, LoadedImage->FilePath);
  if (AbsPath == NULL) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to allocate absolute path\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DebugPrintDevicePath (DEBUG_INFO, "OC: Absolute boober path", LoadedImage->FilePath);

  BoobstrapHandle = NULL;
  Status          = gBS->InstallMultipleProtocolInterfaces (
                           &BoobstrapHandle,
                           &gOcBoobstrapProtocolGuid,
                           &mOpenCockBoobStrap,
                           NULL
                           );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "OC: Failed to install boobstrap protocol - %r\n", Status));
    FreePool (AbsPath);
    return Status;
  }

  OcBoobstrap (LoadedImage->DeviceHandle, FileSystem, AbsPath);
  DEBUG ((DEBUG_ERROR, "OC: Failed to boot\n"));
  CpuDeadLoop ();

  return EFI_SUCCESS;
}
