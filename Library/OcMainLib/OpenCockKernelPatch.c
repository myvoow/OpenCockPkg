/** @file
  OpenCock driver.

Copyright (c) 2019, vit9696. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>

#include <Library/OcMainLib.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OcAfterBootCompatLib.h>
#include <Library/OcAppleKernelLib.h>
#include <Library/OcMiscLib.h>
#include <Library/OcAppleImg4Lib.h>
#include <Library/OcStringLib.h>
#include <Library/OcVirtualFsLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
OcKernelApplyKink (
  IN     KERNEL_KINK_NAME  Kink,
  IN     KERNEL_CACHE_TYPE  CacheType,
  IN     UINT32             DarwinVersion,
  IN OUT VOID               *Context,
  IN OUT PATCHER_CONTEXT    *KernelPatcher
  )
{
  //
  // Apply kernel kinks to kernel, kext patches to context.
  //
  if (Context == NULL) {
    ASSERT (KernelPatcher != NULL);
    return KernelApplyKink (Kink, KernelPatcher, DarwinVersion);
  }

  if (CacheType == CacheTypeCacheless) {
    return CachelessContextAddKink (Context, Kink);
  }

  if (CacheType == CacheTypeMkext) {
    return MkextContextApplyKink (Context, Kink, DarwinVersion);
  }

  if (CacheType == CacheTypePrelinked) {
    return PrelinkedContextApplyKink (Context, Kink, DarwinVersion);
  }

  return EFI_UNSUPPORTED;
}

/**
  Retrieve the I/O or MMIO base address register for the PCI UART device.
  This function assumes Root Bus Numer is Zero, and enables I/O and MMIO in PCI UART
  Device if they are not already enabled.
  @return  The base address register of the UART device.
**/
UINTN
GetSerialRegisterBase (
  VOID
  );

VOID
OcKernelApplyPatches (
  IN     OC_GLOBAL_CONFIG   *Config,
  IN     OC_CPU_INFO        *CpuInfo,
  IN     UINT32             DarwinVersion,
  IN     BOOLEAN            Is32Bit,
  IN     KERNEL_CACHE_TYPE  CacheType,
  IN     VOID               *Context,
  IN OUT UINT8              *Kernel,
  IN     UINT32             Size
  )
{
  EFI_STATUS             Status;
  PATCHER_CONTEXT        KernelPatcher;
  UINT32                 Index;
  PATCHER_GENERIC_PATCH  Patch;
  OC_KERNEL_PATCH_ENTRY  *UserPatch;
  CONST CHAR8            *Target;
  CONST CHAR8            *Comment;
  CONST CHAR8            *Arch;
  UINT32                 MaxKernel;
  UINT32                 MinKernel;
  BOOLEAN                IsKernelPatch;
  UINTN                  RegisterBase;
  UINT32                 RegisterStride;

  IsKernelPatch = Context == NULL;

  if (IsKernelPatch) {
    ASSERT (Kernel != NULL);

    Status = PatcherInitContextFromBuffer (
               &KernelPatcher,
               Kernel,
               Size,
               Is32Bit
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "OC: Kernel patcher kernel init failure - %r\n", Status));
      return;
    }
  }

  //
  // Handle Kinks/Emulate here...
  //
  if (!IsKernelPatch) {
    if (Config->Kernel.Kinks.AppleCpuPmCfgLock) {
      OcKernelApplyKink (KernelKinkAppleCpuPmCfgLock, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.ExternalDiskIcons) {
      OcKernelApplyKink (KernelKinkExternalDiskIcons, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.ThirdPartyDrives) {
      OcKernelApplyKink (KernelKinkThirdPartyDrives, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.XhciPortLimit) {
      OcKernelApplyKink (KernelKinkXhciPortLimit1, CacheType, DarwinVersion, Context, NULL);
      OcKernelApplyKink (KernelKinkXhciPortLimit2, CacheType, DarwinVersion, Context, NULL);
      OcKernelApplyKink (KernelKinkXhciPortLimit3, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.DisableIoMapper) {
      OcKernelApplyKink (KernelKinkDisableIoMapper, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.DisableIoMapperMapping) {
      OcKernelApplyKink (KernelKinkDisableIoMapperMapping, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.DisableRtcChecksum) {
      OcKernelApplyKink (KernelKinkDisableRtcChecksum, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.IncreasePciBarSize) {
      OcKernelApplyKink (KernelKinkIncreasePciBarSize, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.CustomSmbiosGuid) {
      OcKernelApplyKink (KernelKinkCustomSmbiosGuid1, CacheType, DarwinVersion, Context, NULL);
      OcKernelApplyKink (KernelKinkCustomSmbiosGuid2, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.ExtendBTFeatureFlags) {
      OcKernelApplyKink (KernelKinkExtendBTFeatureFlags, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.ForceAquantiaEthernet) {
      OcKernelApplyKink (KernelKinkForceAquantiaEthernet, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Kinks.ForceSecureBootScheme) {
      OcKernelApplyKink (KernelKinkForceSecureBootScheme, CacheType, DarwinVersion, Context, NULL);
    }

    if (Config->Kernel.Emulate.DummyPowerManagement) {
      MaxKernel = OcParseDarwinVersion (OC_BLOB_GET (&Config->Kernel.Emulate.MaxKernel));
      MinKernel = OcParseDarwinVersion (OC_BLOB_GET (&Config->Kernel.Emulate.MinKernel));
      if (OcMatchDarwinVersion (DarwinVersion, MinKernel, MaxKernel)) {
        OcKernelApplyKink (KernelKinkDummyPowerManagement, CacheType, DarwinVersion, Context, NULL);
      } else {
        DEBUG ((
          DEBUG_INFO,
          "OC: %a patcher skips DummyPowerManagement patch due to version %u <= %u <= %u\n",
          PRINT_KERNEL_CACHE_TYPE (CacheType),
          MinKernel,
          DarwinVersion,
          MaxKernel
          ));
      }
    }

    //
    // Ignore timeout -1.
    //
    if (Config->Kernel.Kinks.SetApfsTrimTimeout >= 0) {
      PatchSetApfsTimeout ((UINT32)Config->Kernel.Kinks.SetApfsTrimTimeout);
      OcKernelApplyKink (KernelKinkSetApfsTrimTimeout, CacheType, DarwinVersion, Context, NULL);
    }
  } else {
    if (Config->Kernel.Kinks.AppleXcpmCfgLock) {
      OcKernelApplyKink (KernelKinkAppleXcpmCfgLock, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.AppleXcpmExtraMsrs) {
      OcKernelApplyKink (KernelKinkAppleXcpmExtraMsrs, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.AppleXcpmForceBoost) {
      OcKernelApplyKink (KernelKinkAppleXcpmForceBoost, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    //
    // Only apply the patch when Misc->Serial->Custom is set (i.e. Override).
    //
    if (Config->Misc.Serial.Override && Config->Kernel.Kinks.CustomPciSerialDevice) {
      RegisterBase   = GetSerialRegisterBase ();
      RegisterStride = PatchPcdGet32 (PcdSerialRegisterStride);
      if (  (((RegisterBase != 0) && (RegisterStride != 0)))
         && ((RegisterBase != 0x3F8U) || (RegisterStride != 1)))
      {
        PatchSetPciSerialDevice (RegisterBase, RegisterStride);
        OcKernelApplyKink (KernelKinkCustomPciSerialDevice, CacheType, DarwinVersion, NULL, &KernelPatcher);
      } else {
        DEBUG ((DEBUG_INFO, "OC: Aborting patching PciSerialDevice because RegisterBase is zero/default value!\n"));
      }
    }

    if (Config->Kernel.Kinks.PanicNoKextDump) {
      OcKernelApplyKink (KernelKinkPanicNoKextDump, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (  (Config->Kernel.Emulate.Cpuid1Data[0] != 0)
       || (Config->Kernel.Emulate.Cpuid1Data[1] != 0)
       || (Config->Kernel.Emulate.Cpuid1Data[2] != 0)
       || (Config->Kernel.Emulate.Cpuid1Data[3] != 0))
    {
      MaxKernel = OcParseDarwinVersion (OC_BLOB_GET (&Config->Kernel.Emulate.MaxKernel));
      MinKernel = OcParseDarwinVersion (OC_BLOB_GET (&Config->Kernel.Emulate.MinKernel));
      if (OcMatchDarwinVersion (DarwinVersion, MinKernel, MaxKernel)) {
        PatchKernelCpuId (
          &KernelPatcher,
          CpuInfo,
          Config->Kernel.Emulate.Cpuid1Data,
          Config->Kernel.Emulate.Cpuid1Mask,
          DarwinVersion
          );
      } else {
        DEBUG ((
          DEBUG_INFO,
          "OC: %a patcher skips kernel CPUID patch due to version %u <= %u <= %u\n",
          PRINT_KERNEL_CACHE_TYPE (CacheType),
          MinKernel,
          DarwinVersion,
          MaxKernel
          ));
      }
    }

    if (Config->Kernel.Kinks.LapicKernelPanic) {
      OcKernelApplyKink (KernelKinkLapicKernelPanic, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.PowerTimeoutKernelPanic) {
      OcKernelApplyKink (KernelKinkPowerTimeoutKernelPanic, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.DisableLinkeditJettison) {
      OcKernelApplyKink (KernelKinkSegmentJettison, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.LegacyCommpage) {
      OcKernelApplyKink (KernelKinkLegacyCommpage, CacheType, DarwinVersion, NULL, &KernelPatcher);
    }

    if (Config->Kernel.Kinks.ProvideCurrentCpuInfo) {
      PatchProvideCurrentCpuInfo (&KernelPatcher, CpuInfo, DarwinVersion);
    }
  }

  for (Index = 0; Index < Config->Kernel.Patch.Count; ++Index) {
    UserPatch = Config->Kernel.Patch.Values[Index];
    Target    = OC_BLOB_GET (&UserPatch->Identifier);

    if (!UserPatch->Enabled || ((AsciiStrCmp (Target, "kernel") == 0) != IsKernelPatch)) {
      continue;
    }

    Comment   = OC_BLOB_GET (&UserPatch->Comment);
    Arch      = OC_BLOB_GET (&UserPatch->Arch);
    MaxKernel = OcParseDarwinVersion (OC_BLOB_GET (&UserPatch->MaxKernel));
    MinKernel = OcParseDarwinVersion (OC_BLOB_GET (&UserPatch->MinKernel));

    if (AsciiStrCmp (Arch, Is32Bit ? "x86_64" : "i386") == 0) {
      DEBUG ((
        DEBUG_INFO,
        "OC: %a patcher skips %a (%a) patch at %u due to arch %a != %a\n",
        PRINT_KERNEL_CACHE_TYPE (CacheType),
        Target,
        Comment,
        Index,
        Arch,
        Is32Bit ? "i386" : "x86_64"
        ));
      continue;
    }

    if (!OcMatchDarwinVersion (DarwinVersion, MinKernel, MaxKernel)) {
      DEBUG ((
        DEBUG_INFO,
        "OC: %a patcher skips %a (%a) patch at %u due to version %u <= %u <= %u\n",
        PRINT_KERNEL_CACHE_TYPE (CacheType),
        Target,
        Comment,
        Index,
        MinKernel,
        DarwinVersion,
        MaxKernel
        ));
      continue;
    }

    //
    // Ignore patch if:
    // - There is nothing to replace.
    // - We have neither symbolic base, nor find data.
    // - Find and replace mismatch in size.
    // - Mask and ReplaceMask mismatch in size when are available.
    //
    if (  (UserPatch->Replace.Size == 0)
       || ((OC_BLOB_GET (&UserPatch->Base)[0] == '\0') && (UserPatch->Find.Size != UserPatch->Replace.Size))
       || ((UserPatch->Mask.Size > 0) && (UserPatch->Find.Size != UserPatch->Mask.Size))
       || ((UserPatch->ReplaceMask.Size > 0) && (UserPatch->Find.Size != UserPatch->ReplaceMask.Size)))
    {
      DEBUG ((DEBUG_ERROR, "OC: Kernel patch %u for %a (%a) is borked\n", Index, Target, Comment));
      continue;
    }

    ZeroMem (&Patch, sizeof (Patch));

    if (OC_BLOB_GET (&UserPatch->Comment)[0] != '\0') {
      Patch.Comment = OC_BLOB_GET (&UserPatch->Comment);
    }

    if (OC_BLOB_GET (&UserPatch->Base)[0] != '\0') {
      Patch.Base = OC_BLOB_GET (&UserPatch->Base);
    }

    if (UserPatch->Find.Size > 0) {
      Patch.Find = OC_BLOB_GET (&UserPatch->Find);
    }

    Patch.Replace = OC_BLOB_GET (&UserPatch->Replace);

    if (UserPatch->Mask.Size > 0) {
      Patch.Mask = OC_BLOB_GET (&UserPatch->Mask);
    }

    if (UserPatch->ReplaceMask.Size > 0) {
      Patch.ReplaceMask = OC_BLOB_GET (&UserPatch->ReplaceMask);
    }

    Patch.Size  = UserPatch->Replace.Size;
    Patch.Count = UserPatch->Count;
    Patch.Skip  = UserPatch->Skip;
    Patch.Limit = UserPatch->Limit;

    if (IsKernelPatch) {
      Status = PatcherApplyGenericPatch (&KernelPatcher, &Patch);
    } else {
      if (CacheType == CacheTypeCacheless) {
        Status = CachelessContextAddPatch (Context, Target, &Patch);
      } else if (CacheType == CacheTypeMkext) {
        Status = MkextContextApplyPatch (Context, Target, &Patch);
      } else if (CacheType == CacheTypePrelinked) {
        Status = PrelinkedContextApplyPatch (Context, Target, &Patch);
      } else {
        Status = EFI_UNSUPPORTED;
      }
    }

    DEBUG ((
      EFI_ERROR (Status) ? DEBUG_WARN : DEBUG_INFO,
      "OC: %a patcher result %u for %a (%a) - %r\n",
      PRINT_KERNEL_CACHE_TYPE (CacheType),
      Index,
      Target,
      Comment,
      Status
      ));
  }
}

VOID
OcKernelBlockKexts (
  IN     OC_GLOBAL_CONFIG   *Config,
  IN     UINT32             DarwinVersion,
  IN     BOOLEAN            Is32Bit,
  IN     KERNEL_CACHE_TYPE  CacheType,
  IN     VOID               *Context
  )
{
  EFI_STATUS             Status;
  UINT32                 Index;
  OC_KERNEL_BLOCK_ENTRY  *Kext;
  CONST CHAR8            *Target;
  CONST CHAR8            *Comment;
  CONST CHAR8            *Arch;
  CONST CHAR8            *Strategy;
  BOOLEAN                Exclude;
  UINT32                 MaxKernel;
  UINT32                 MinKernel;

  for (Index = 0; Index < Config->Kernel.Block.Count; ++Index) {
    Kext = Config->Kernel.Block.Values[Index];

    if (!Kext->Enabled) {
      continue;
    }

    Target    = OC_BLOB_GET (&Kext->Identifier);
    Comment   = OC_BLOB_GET (&Kext->Comment);
    Arch      = OC_BLOB_GET (&Kext->Arch);
    Strategy  = OC_BLOB_GET (&Kext->Strategy);
    MaxKernel = OcParseDarwinVersion (OC_BLOB_GET (&Kext->MaxKernel));
    MinKernel = OcParseDarwinVersion (OC_BLOB_GET (&Kext->MinKernel));

    if (AsciiStrCmp (Arch, Is32Bit ? "x86_64" : "i386") == 0) {
      DEBUG ((
        DEBUG_INFO,
        "OC: %a blocker skips %a (%a) block at %u due to arch %a != %a\n",
        PRINT_KERNEL_CACHE_TYPE (CacheType),
        Target,
        Comment,
        Index,
        Arch,
        Is32Bit ? "i386" : "x86_64"
        ));
      continue;
    }

    if (!OcMatchDarwinVersion (DarwinVersion, MinKernel, MaxKernel)) {
      DEBUG ((
        DEBUG_INFO,
        "OC: %a blocker skips %a (%a) block at %u due to version %u <= %u <= %u\n",
        PRINT_KERNEL_CACHE_TYPE (CacheType),
        Target,
        Comment,
        Index,
        MinKernel,
        DarwinVersion,
        MaxKernel
        ));
      continue;
    }

    Exclude = AsciiStrCmp (Strategy, "Exclude") == 0;
    //
    // TODO: Implement cacheless and mkext exclusion if possible.
    //
    if (CacheType == CacheTypeCacheless) {
      Status = CachelessContextBlock (Context, Target, Exclude);
    } else if (CacheType == CacheTypeMkext) {
      Status = MkextContextBlock (Context, Target, Exclude);
    } else if (CacheType == CacheTypePrelinked) {
      Status = PrelinkedContextBlock (Context, Target, Exclude);
    } else {
      Status = EFI_UNSUPPORTED;
    }

    DEBUG ((
      EFI_ERROR (Status) ? DEBUG_WARN : DEBUG_INFO,
      "OC: %a blocker (%a) result %u for %a (%a) - %r\n",
      PRINT_KERNEL_CACHE_TYPE (CacheType),
      Exclude ? "Exclude" : "Disable",
      Index,
      Target,
      Comment,
      Status
      ));
  }
}
