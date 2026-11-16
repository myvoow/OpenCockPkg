## @file
# Copyright (C) 2018, vit9696.  All rights reserved.<BR>
# Copyright (C) 2018, Download-Fritz.  All rights reserved.<BR>
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##

[Defines]
  PLATFORM_NAME           = OpenCockPkg
  PLATFORM_GUID           = C46F121D-ABC6-42A3-A241-91B09224C357
  PLATFORM_VERSION        = 1.0
  SUPPORTED_ARCHITECTURES = X64|IA32
  BUILD_TARGETS           = RELEASE|DEBUG|NOOPT
  SKUID_IDENTIFIER        = DEFAULT
  DSC_SPECIFICATION       = 0x0001001C
  FLASH_DEFINITION        = OpenCockPkg/OpenCockPkg.fdf

  #
  # Network definition
  #
  DEFINE NETWORK_ENABLE                 = TRUE
  DEFINE NETWORK_SNP_ENABLE             = TRUE
  DEFINE NETWORK_IP4_ENABLE             = TRUE
  DEFINE NETWORK_IP6_ENABLE             = TRUE
  DEFINE NETWORK_TLS_ENABLE             = TRUE
  DEFINE NETWORK_HTTP_ENABLE            = TRUE
  DEFINE NETWORK_HTTP_BOOT_ENABLE       = TRUE
  DEFINE NETWORK_ALLOW_HTTP_CONNECTIONS = TRUE
  DEFINE NETWORK_ISCSI_ENABLE           = FALSE
  DEFINE NETWORK_ISCSI_MD5_ENABLE       = FALSE
  DEFINE NETWORK_VLAN_ENABLE            = FALSE

!include NetworkPkg/NetworkDefines.dsc.inc

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  # We cannot use BaseMemoryLibOptDxe since it uses SSE instructions,
  # and some types of firmware fail to properly maintain MMX register contexts
  # across the timers. This results in exceptions when trying to execute
  # primitives like CopyMem in timers (e.g. AIKDataWriteEntry).
  # Reproduced on ASUS M5A97 with AMD FX8320 CPU.
  # REF: https://github.com/acidanthera/bugtracker/issues/754
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  BaseOverflowLib|MdePkg/Library/BaseOverflowLib/BaseOverflowLib.inf
  BaseRngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  DebugLib|OpenCockPkg/Library/OcDebugLibProtocol/OcDebugLibProtocol.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  VirtioLib|OvmfPkg/Library/VirtioLib/VirtioLib.inf
  FrameBufferBltLib|MdeModulePkg/Library/FrameBufferBltLib/FrameBufferBltLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  OcAcpiLib|OpenCockPkg/Library/OcAcpiLib/OcAcpiLib.inf
  OcAfterBootCompatLib|OpenCockPkg/Library/OcAfterBootCompatLib/OcAfterBootCompatLib.inf
  OcApfsLib|OpenCockPkg/Library/OcApfsLib/OcApfsLib.inf
  OcAppleBootPolicyLib|OpenCockPkg/Library/OcAppleBootPolicyLib/OcAppleBootPolicyLib.inf
  OcAppleChunklistLib|OpenCockPkg/Library/OcAppleChunklistLib/OcAppleChunklistLib.inf
  OcAppleDiskImageLib|OpenCockPkg/Library/OcAppleDiskImageLib/OcAppleDiskImageLib.inf
  OcAppleEventLib|OpenCockPkg/Library/OcAppleEventLib/OcAppleEventLib.inf
  OcAppleFirmwarePasswordLib|OpenCockPkg/Library/OcAppleFirmwarePasswordLib/OcAppleFirmwarePasswordLib.inf
  OcAppleImageConversionLib|OpenCockPkg/Library/OcAppleImageConversionLib/OcAppleImageConversionLib.inf
  OcAppleImg4Lib|OpenCockPkg/Library/OcAppleImg4Lib/OcAppleImg4Lib.inf
  OcAppleKernelLib|OpenCockPkg/Library/OcAppleKernelLib/OcAppleKernelLib.inf
  OcAppleKeyMapLib|OpenCockPkg/Library/OcAppleKeyMapLib/OcAppleKeyMapLib.inf
  OcAppleKeysLib|OpenCockPkg/Library/OcAppleKeysLib/OcAppleKeysLib.inf
  OcAppleRamDiskLib|OpenCockPkg/Library/OcAppleRamDiskLib/OcAppleRamDiskLib.inf
  OcAppleSecureBootLib|OpenCockPkg/Library/OcAppleSecureBootLib/OcAppleSecureBootLib.inf
  OcAppleUserInterfaceThemeLib|OpenCockPkg/Library/OcAppleUserInterfaceThemeLib/OcAppleUserInterfaceThemeLib.inf
  OcAudioLib|OpenCockPkg/Library/OcAudioLib/OcAudioLib.inf
  OcBlitLib|OpenCockPkg/Library/OcBlitLib/OcBlitLib.inf
  OcBootManagementLib|OpenCockPkg/Library/OcBootManagementLib/OcBootManagementLib.inf
  OcBootServicesTableLib|OpenCockPkg/Library/OcBootServicesTableLib/OcBootServicesTableLib.inf
  OcCompressionLib|OpenCockPkg/Library/OcCompressionLib/OcCompressionLib.inf
  OcConfigurationLib|OpenCockPkg/Library/OcConfigurationLib/OcConfigurationLib.inf
  OcConsoleControlEntryModeGenericLib|OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeGenericLib.inf
  OcConsoleControlEntryModeLocalLib|OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeLocalLib.inf
  OcConsoleLib|OpenCockPkg/Library/OcConsoleLib/OcConsoleLib.inf
  OcCpuLib|OpenCockPkg/Library/OcCpuLib/OcCpuLib.inf
  OcCryptoLib|OpenCockPkg/Library/OcCryptoLib/OcCryptoLib.inf
  OcDataHubLib|OpenCockPkg/Library/OcDataHubLib/OcDataHubLib.inf
  OcDeviceMiscLib|OpenCockPkg/Library/OcDeviceMiscLib/OcDeviceMiscLib.inf
  OcDevicePathLib|OpenCockPkg/Library/OcDevicePathLib/OcDevicePathLib.inf
  OcDevicePropertyLib|OpenCockPkg/Library/OcDevicePropertyLib/OcDevicePropertyLib.inf
  OcDeviceTreeLib|OpenCockPkg/Library/OcDeviceTreeLib/OcDeviceTreeLib.inf
  OcDirectResetLib|OpenCockPkg/Library/OcDirectResetLib/OcDirectResetLib.inf
  OcDriverConnectionLib|OpenCockPkg/Library/OcDriverConnectionLib/OcDriverConnectionLib.inf
  OcFileLib|OpenCockPkg/Library/OcFileLib/OcFileLib.inf
  OcFirmwareVolumeLib|OpenCockPkg/Library/OcFirmwareVolumeLib/OcFirmwareVolumeLib.inf
  OcFlexArrayLib|OpenCockPkg/Library/OcFlexArrayLib/OcFlexArrayLib.inf
  OcGuardLib|OpenCockPkg/Library/OcGuardLib/OcGuardLib.inf
  OcHashServicesLib|OpenCockPkg/Library/OcHashServicesLib/OcHashServicesLib.inf
  OcHdaDevicesLib|OpenCockPkg/Library/OcHdaDevicesLib/OcHdaDevicesLib.inf
  OcHeciLib|OpenCockPkg/Library/OcHeciLib/OcHeciLib.inf
  OcHiiDatabaseLocalLib|OpenCockPkg/Library/OcHiiDatabaseLib/OcHiiDatabaseLocalLib.inf
  OcInputLib|OpenCockPkg/Library/OcInputLib/OcInputLib.inf
  OcLegacyThunkLib|OpenCockPkg/Library/OcLegacyThunkLib/OcLegacyThunkLib.inf
  OcLogAggregatorLib|OpenCockPkg/Library/OcLogAggregatorLib/OcLogAggregatorLib.inf
  OcMachoLib|OpenCockPkg/Library/OcMachoLib/OcMachoLib.inf
  OcMacInfoLib|OpenCockPkg/Library/OcMacInfoLib/OcMacInfoLib.inf
  OcMainLib|OpenCockPkg/Library/OcMainLib/OcMainLib.inf
  OcMemoryLib|OpenCockPkg/Library/OcMemoryLib/OcMemoryLib.inf
  OcMiscLib|OpenCockPkg/Library/OcMiscLib/OcMiscLib.inf
  OcMp3Lib|OpenCockPkg/Library/OcMp3Lib/OcMp3Lib.inf
  OcOSInfoLib|OpenCockPkg/Library/OcOSInfoLib/OcOSInfoLib.inf
  OcPciIoLib|OpenCockPkg/Library/OcPciIoLib/OcPciIoLib.inf
  OcPngLib|OpenCockPkg/Library/OcPngLib/OcPngLib.inf
  OcRngLib|OpenCockPkg/Library/OcRngLib/OcRngLib.inf
  OcRtcLib|OpenCockPkg/Library/OcRtcLib/OcRtcLib.inf
  OcSerializeLib|OpenCockPkg/Library/OcSerializeLib/OcSerializeLib.inf
  OcSmbiosLib|OpenCockPkg/Library/OcSmbiosLib/OcSmbiosLib.inf
  OcSmcLib|OpenCockPkg/Library/OcSmcLib/OcSmcLib.inf
  OcStorageLib|OpenCockPkg/Library/OcStorageLib/OcStorageLib.inf
  OcStringLib|OpenCockPkg/Library/OcStringLib/OcStringLib.inf
  OcTemplateLib|OpenCockPkg/Library/OcTemplateLib/OcTemplateLib.inf
  OcTypingLib|OpenCockPkg/Library/OcTypingLib/OcTypingLib.inf
  TimerLib|OpenCockPkg/Library/OcTimerLib/OcTimerLib.inf
  OcUnicodeCollationEngGenericLib|OpenCockPkg/Library/OcUnicodeCollationEngLib/OcUnicodeCollationEngGenericLib.inf
  OcUnicodeCollationEngLocalLib|OpenCockPkg/Library/OcUnicodeCollationEngLib/OcUnicodeCollationEngLocalLib.inf
  OcVirtualFsLib|OpenCockPkg/Library/OcVirtualFsLib/OcVirtualFsLib.inf
  OcWaveLib|OpenCockPkg/Library/OcWaveLib/OcWaveLib.inf
  OcXmlLib|OpenCockPkg/Library/OcXmlLib/OcXmlLib.inf
  OcPeCoffExtLib|OpenCockPkg/Library/OcPeCoffExtLib/OcPeCoffExtLib.inf
  OcVariableLib|OpenCockPkg/Library/OcVariableLib/OcVariableLib.inf
  OcVariableRuntimeLib|OpenCockPkg/Library/OcVariableRuntimeLib/OcVariableRuntimeLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciCapLib|OvmfPkg/Library/BasePciCapLib/BasePciCapLib.inf
  PciCapPciIoLib|OvmfPkg/Library/UefiPciCapPciIoLib/UefiPciCapPciIoLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PeCoffLib2|MdePkg/Library/BasePeCoffLib2/BasePeCoffLib2.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  UefiApplicationEntryPoint|OpenCockPkg/Library/OcApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  UefiDriverEntryPoint|OpenCockPkg/Library/OcDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiHiiServicesLib|OpenCockPkg/Library/OcHiiServicesLib/OcHiiServicesLib.inf
  UefiImageExtraActionLib|MdePkg/Library/BaseUefiImageExtraActionLibNull/BaseUefiImageExtraActionLibNull.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf
  VariableFlashInfoLib|MdeModulePkg/Library/BaseVariableFlashInfoLib/BaseVariableFlashInfoLib.inf
  ResetSystemLib|OpenCockPkg/Library/OcResetSystemLib/OcResetSystemLib.inf

  !if $(NETWORK_TLS_ENABLE) == TRUE
    BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
    # FileExplorerLib is for TlsAuthConfigDxe only (not used by us, but enabled by NETWORK_TLS_ENABLE)
    FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
    IntrinsicLib|MdePkg/Library/IntrinsicLib/IntrinsicLib.inf
    OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
    RngLib|MdeModulePkg/Library/BaseRngLibTimerLib/BaseRngLibTimerLib.inf
    SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
    TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf
  !endif

  !include NetworkPkg/NetworkLibs.dsc.inc

  HttpLib|NetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf

  !include Ext4Pkg/Ext4Defines.dsc.inc
  !include Ext4Pkg/Ext4Libs.dsc.inc

[Components]
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf {
    <LibraryClasses>
      !if $(TARGET) == RELEASE
        DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      !endif
  }
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf {
    <LibraryClasses>
      !if $(TARGET) == RELEASE
        DebugLib|OpenCockPkg/Library/OcDebugLibNull/OcDebugLibNull.inf
      !endif
  }
  MdeModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  MdeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2KeyboardDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  OpenCockPkg/Application/BootKicker/BootKicker.inf
  OpenCockPkg/Application/Bootstrap/Bootstrap.inf {
    <LibraryClasses>
      !if $(TARGET) != RELEASE
        # Force onscreen visible logging in DEBUG/NOOPT builds.
        NULL|OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeGenericLib.inf
      !endif
  }
  OpenCockPkg/Application/ChipTune/ChipTune.inf
  OpenCockPkg/Application/CleanNvram/CleanNvram.inf
  OpenCockPkg/Application/CsrUtil/CsrUtil.inf
  OpenCockPkg/Application/FontTester/FontTester.inf
  OpenCockPkg/Application/GopPerf/GopPerf.inf
  OpenCockPkg/Application/GopStop/GopStop.inf
  OpenCockPkg/Application/KeyTester/KeyTester.inf
  OpenCockPkg/Application/ListPartitions/ListPartitions.inf
  OpenCockPkg/Application/MmapDump/MmapDump.inf
  OpenCockPkg/Application/OpenControl/OpenControl.inf
  OpenCockPkg/Application/OpenCock/OpenCock.inf {
    <LibraryClasses>
      !if $(TARGET) != RELEASE
        # Force onscreen visible logging in DEBUG/NOOPT builds.
        NULL|OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeGenericLib.inf
      !endif
  }
  OpenCockPkg/Application/PavpProvision/PavpProvision.inf
  OpenCockPkg/Application/ResetSystem/ResetSystem.inf
  OpenCockPkg/Application/RtcRw/RtcRw.inf
  OpenCockPkg/Application/TpmInfo/TpmInfo.inf
  OpenCockPkg/Application/VerifyMemOpt/VerifyMemOpt.inf {
    <LibraryClasses>
      BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptDxe/BaseMemoryLibOptDxe.inf
  }
  OpenCockPkg/Application/ControlMsrE2/ControlMsrE2.inf
  OpenCockPkg/Debug/GdbSyms/GdbSyms.inf
  OpenCockPkg/Library/OcAcpiLib/OcAcpiLib.inf
  OpenCockPkg/Library/OcAfterBootCompatLib/OcAfterBootCompatLib.inf
  OpenCockPkg/Library/OcApfsLib/OcApfsLib.inf
  OpenCockPkg/Library/OcAppleBootPolicyLib/OcAppleBootPolicyLib.inf
  OpenCockPkg/Library/OcAppleChunklistLib/OcAppleChunklistLib.inf
  OpenCockPkg/Library/OcAppleDiskImageLib/OcAppleDiskImageLib.inf
  OpenCockPkg/Library/OcAppleEventLib/OcAppleEventLib.inf
  OpenCockPkg/Library/OcAppleFirmwarePasswordLib/OcAppleFirmwarePasswordLib.inf
  OpenCockPkg/Library/OcAppleImageConversionLib/OcAppleImageConversionLib.inf
  OpenCockPkg/Library/OcAppleImg4Lib/OcAppleImg4Lib.inf
  OpenCockPkg/Library/OcAppleKernelLib/OcAppleKernelLib.inf
  OpenCockPkg/Library/OcAppleKeyMapLib/OcAppleKeyMapLib.inf
  OpenCockPkg/Library/OcAppleRamDiskLib/OcAppleRamDiskLib.inf
  OpenCockPkg/Library/OcAppleSecureBootLib/OcAppleSecureBootLib.inf
  OpenCockPkg/Library/OcAppleUserInterfaceThemeLib/OcAppleUserInterfaceThemeLib.inf
  OpenCockPkg/Library/OcAudioLib/OcAudioLib.inf
  OpenCockPkg/Library/OcBlitLib/OcBlitLib.inf
  OpenCockPkg/Library/OcBootManagementLib/OcBootManagementLib.inf
  OpenCockPkg/Library/OcBootServicesTableLib/OcBootServicesTableLib.inf
  OpenCockPkg/Library/OcCompressionLib/OcCompressionLib.inf
  OpenCockPkg/Library/OcConfigurationLib/OcConfigurationLib.inf
  OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeGenericLib.inf
  OpenCockPkg/Library/OcConsoleControlEntryModeLib/OcConsoleControlEntryModeLocalLib.inf
  OpenCockPkg/Library/OcConsoleLib/OcConsoleLib.inf
  OpenCockPkg/Library/OcCpuLib/OcCpuLib.inf
  OpenCockPkg/Library/OcCryptoLib/OcCryptoLib.inf
  OpenCockPkg/Library/OcDataHubLib/OcDataHubLib.inf
  OpenCockPkg/Library/OcDebugLibSerial/OcDebugLibSerial.inf
  OpenCockPkg/Library/OcDebugLibProtocol/OcDebugLibProtocol.inf
  OpenCockPkg/Library/OcDebugLibNull/OcDebugLibNull.inf
  OpenCockPkg/Library/OcDeviceMiscLib/OcDeviceMiscLib.inf
  OpenCockPkg/Library/OcDevicePathLib/OcDevicePathLib.inf
  OpenCockPkg/Library/OcDevicePropertyLib/OcDevicePropertyLib.inf
  OpenCockPkg/Library/OcDeviceTreeLib/OcDeviceTreeLib.inf
  OpenCockPkg/Library/OcDirectResetLib/OcDirectResetLib.inf
  OpenCockPkg/Library/OcDriverConnectionLib/OcDriverConnectionLib.inf
  OpenCockPkg/Library/OcFileLib/OcFileLib.inf
  OpenCockPkg/Library/OcFirmwareVolumeLib/OcFirmwareVolumeLib.inf
  OpenCockPkg/Library/OcFlexArrayLib/OcFlexArrayLib.inf
  OpenCockPkg/Library/OcGuardLib/OcGuardLib.inf
  OpenCockPkg/Library/OcHashServicesLib/OcHashServicesLib.inf
  OpenCockPkg/Library/OcHdaDevicesLib/OcHdaDevicesLib.inf
  OpenCockPkg/Library/OcHeciLib/OcHeciLib.inf
  OpenCockPkg/Library/OcHiiDatabaseLib/OcHiiDatabaseLocalLib.inf
  OpenCockPkg/Library/OcInputLib/OcInputLib.inf
  OpenCockPkg/Library/OcLogAggregatorLib/OcLogAggregatorLib.inf
  OpenCockPkg/Library/OcLogAggregatorLibNull/OcLogAggregatorLibNull.inf
  OpenCockPkg/Library/OcMachoLib/OcMachoLib.inf
  OpenCockPkg/Library/OcMainLib/OcMainLib.inf
  OpenCockPkg/Library/OcMemoryLib/OcMemoryLib.inf
  OpenCockPkg/Library/OcMiscLib/OcMiscLib.inf
  OpenCockPkg/Library/OcMp3Lib/OcMp3Lib.inf
  OpenCockPkg/Library/OcOSInfoLib/OcOSInfoLib.inf
  OpenCockPkg/Library/OcPeCoffExtLib/OcPeCoffExtLib.inf
  OpenCockPkg/Library/OcPngLib/OcPngLib.inf
  OpenCockPkg/Library/OcRngLib/OcRngLib.inf
  OpenCockPkg/Library/OcSerializeLib/OcSerializeLib.inf
  OpenCockPkg/Library/OcSmbiosLib/OcSmbiosLib.inf
  OpenCockPkg/Library/OcSmcLib/OcSmcLib.inf
  OpenCockPkg/Library/OcStorageLib/OcStorageLib.inf
  OpenCockPkg/Library/OcStringLib/OcStringLib.inf
  OpenCockPkg/Library/OcTemplateLib/OcTemplateLib.inf
  OpenCockPkg/Library/OcTimerLib/OcTimerLib.inf
  OpenCockPkg/Library/OcUnicodeCollationEngLib/OcUnicodeCollationEngGenericLib.inf
  OpenCockPkg/Library/OcUnicodeCollationEngLib/OcUnicodeCollationEngLocalLib.inf
  OpenCockPkg/Library/OcPciIoLib/OcPciIoLib.inf
  OpenCockPkg/Library/OcVirtualFsLib/OcVirtualFsLib.inf
  OpenCockPkg/Library/OcWaveLib/OcWaveLib.inf
  OpenCockPkg/Library/OcXmlLib/OcXmlLib.inf
  OpenCockPkg/Legacy/BootPlatform/BiosVideo/BiosVideo.inf
  OpenCockPkg/Platform/CrScreenshotDxe/CrScreenshotDxe.inf
  OpenCockPkg/Platform/OpenCanopy/OpenCanopy.inf
  OpenCockPkg/Platform/OpenLegacyBoot/OpenLegacyBoot.inf
  OpenCockPkg/Platform/OpenLinuxBoot/OpenLinuxBoot.inf
  OpenCockPkg/Platform/OpenNetworkBoot/OpenNetworkBoot.inf
  OpenCockPkg/Platform/OpenNtfsDxe/OpenNtfsDxe.inf
  OpenCockPkg/Platform/OpenPartitionDxe/PartitionDxe.inf
  OpenCockPkg/Platform/OpenRuntime/OpenRuntime.inf
  OpenCockPkg/Platform/OpenUsbKbDxe/UsbKbDxe.inf
  OpenCockPkg/Platform/OpenVariableRuntimeDxe/VariableRuntimeDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable|TRUE
      gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
      gEfiMdeModulePkgTokenSpaceGuid.PcdVariableStoreSize|0x10000
    <LibraryClasses>
      UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
      SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
      TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
      VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
      VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf
      TimerLib|OpenCockPkg/Library/DuetTimerLib/DuetTimerLib.inf
      DebugLib|OpenCockPkg/Library/OcDebugLibNull/OcDebugLibNull.inf
      NULL|OpenCockPkg/Library/OcVariableRuntimeLib/OcVariableRuntimeLib.inf
  }
  OpenCockPkg/Platform/FirmwareSettingsEntry/FirmwareSettingsEntry.inf
  OpenCockPkg/Platform/ResetNvramEntry/ResetNvramEntry.inf
  OpenCockPkg/Platform/ToggleSipEntry/ToggleSipEntry.inf
  OpenCockPkg/Staging/AudioDxe/AudioDxe.inf
  OpenCockPkg/Staging/EnableGop/EnableGop.inf {
    <LibraryClasses>
      DebugLib|OpenCockPkg/Library/OcDebugLibNull/OcDebugLibNull.inf
  }
  OpenCockPkg/Staging/EnableGop/EnableGopDirect.inf {
    <LibraryClasses>
      DebugLib|OpenCockPkg/Library/OcDebugLibNull/OcDebugLibNull.inf
  }
  OpenCockPkg/Staging/OpenHfsPlus/OpenHfsPlus.inf
  OpenCockPkg/Tests/AcpiTest/AcpiTest.inf
  OpenCockPkg/Tests/AcpiTest/AcpiTestApp.inf
  OpenCockPkg/Tests/CryptoTest/CryptoTest.inf
  OpenCockPkg/Tests/CryptoTest/CryptoTestApp.inf
  OpenCockPkg/Tests/DataHubTest/DataHubTest.inf
  OpenCockPkg/Tests/DataHubTest/DataHubTestApp.inf
  OpenCockPkg/Tests/PropertyTest/PropertyTest.inf
  OpenCockPkg/Tests/PropertyTest/PropertyTestApp.inf
  OpenCockPkg/Tests/SmbiosTest/SmbiosTest.inf
  OpenCockPkg/Tests/SmbiosTest/SmbiosTestApp.inf
  # UEFI Shell with all commands integrated
  ShellPkg/Application/Shell/Shell.inf {
   <Defines>
      FILE_GUID = EA4BB293-2D7F-4456-A681-1F22F42CD0BC
    <PcdsFixedAtBuild>
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      # For some reason ShellPkg overrides this, so do we.
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
    <LibraryClasses>
      # Use custom BootServicesTable
      UefiBootServicesTableLib|OpenCockPkg/Library/OcBootServicesTableLib/UefiBootServicesTableLib.inf
      # Add the original commands.
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellAcpiViewCommandLib/UefiShellAcpiViewCommandLib.inf
  }

  # Ext4 driver
  Ext4Pkg/Ext4Dxe/Ext4Dxe.inf

  # RNG and HASH2 protocols are required by various network boot drivers since edk2-stable202405
  # REF: https://github.com/acidanthera/bugtracker/issues/2421
  SecurityPkg/RandomNumberGenerator/RngDxe/RngDxe.inf
  SecurityPkg/Hash2DxeCrypto/Hash2DxeCrypto.inf

  #
  # Network Support
  #
  !include NetworkPkg/NetworkComponents.dsc.inc

  #
  # Ramdisk support (driver required for network boot native .iso/.img support)
  #
  MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf

  #
  # QEMU Virtio devices support
  #
  OvmfPkg/VirtioPciDeviceDxe/VirtioPciDeviceDxe.inf
  OvmfPkg/Virtio10Dxe/Virtio10.inf
  OvmfPkg/VirtioBlkDxe/VirtioBlk.inf
  OvmfPkg/VirtioScsiDxe/VirtioScsi.inf
  OvmfPkg/VirtioSerialDxe/VirtioSerial.inf
  OvmfPkg/VirtioGpuDxe/VirtioGpu.inf
  OvmfPkg/VirtioNetDxe/VirtioNet.inf

[LibraryClasses]
  NULL|MdePkg/Library/IntrinsicLib/IntrinsicLib.inf

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|0
!if $(TARGET) == RELEASE
  # DEBUG_PRINT_ENABLED
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|2
  # DEBUG_ERROR | DEBUG_WARN
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000002
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x80000002
!else
  # DEBUG_ASSERT_ENABLED | DEBUG_PRINT_ENABLED | DEBUG_CODE_ENABLED | CLEAR_MEMORY_ENABLED | ASSERT_DEADLOOP_ENABLED
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
  # DEBUG_ERROR | DEBUG_WARN | DEBUG_INFO
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000042
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x80000042
!endif
  gOpenCockPkgTokenSpaceGuid.PcdCanaryAllowRdtscFallback|TRUE

  # ImageLoader settings
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderRtRelocAllowTargetMismatch|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderHashProhibitOverlap|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderLoadHeader|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderDebugSupport|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderAllowMisalignedOffset|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderRemoveXForWX|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderWXorX|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderAlignmentPolicy|0xFFFFFFFF
  gEfiMdePkgTokenSpaceGuid.PcdImageLoaderRelocTypePolicy|0xFFFFFFFF
  gEfiMdePkgTokenSpaceGuid.PcdImageProtectionPolicy|0x00000003

[PcdsPatchableInModule]
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterAccessWidth|8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialDetectCable|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x03F8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|115200
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialLineControl|0x03
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|0x07
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|1843200
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialPciDeviceInfo|{0xFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialExtendedTxFifoSize|64
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1
  #
  # Network Pcds
  #
  !include NetworkPkg/NetworkFixedPcds.dsc.inc

[BuildOptions]
  # While there are no PCDs as of now, there at least are some custom macros.
  DEFINE OCPKG_BUILD_OPTIONS_GEN = -D DISABLE_NEW_DEPRECATED_INTERFACES $(OCPKG_BUILD_OPTIONS) -D OC_TARGET_$(TARGET)=1
  DEFINE OCPKG_ANAL_OPTIONS_GEN = "-DANALYZER_UNREACHABLE=__builtin_unreachable" "-DANALYZER_NORETURN=__attribute__((noreturn))"

  GCC:DEBUG_*_*_CC_FLAGS        = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -Wuninitialized
  GCC:NOOPT_*_*_CC_FLAGS        = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -Wuninitialized
  GCC:RELEASE_*_*_CC_FLAGS      = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -Wuninitialized
  CLANGPDB:DEBUG_*_*_CC_FLAGS   = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  CLANGPDB:NOOPT_*_*_CC_FLAGS   = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  CLANGPDB:RELEASE_*_*_CC_FLAGS = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  CLANGGCC:DEBUG_*_*_CC_FLAGS   = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  CLANGGCC:NOOPT_*_*_CC_FLAGS   = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  CLANGGCC:RELEASE_*_*_CC_FLAGS = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -mstack-protector-guard=global -ftrivial-auto-var-init=pattern
  MSFT:DEBUG_*_*_CC_FLAGS       = $(OCPKG_BUILD_OPTIONS_GEN) /wd4324 /wd4723 /GS /kernel
  MSFT:NOOPT_*_*_CC_FLAGS       = $(OCPKG_BUILD_OPTIONS_GEN) /wd4324 /wd4723 /GS /kernel
  MSFT:RELEASE_*_*_CC_FLAGS     = $(OCPKG_BUILD_OPTIONS_GEN) /wd4324 /wd4723 /GS /kernel
  XCODE:DEBUG_*_*_CC_FLAGS      = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -ftrivial-auto-var-init=pattern
  XCODE:NOOPT_*_*_CC_FLAGS      = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -fstack-protector-strong -ftrivial-auto-var-init=pattern
  XCODE:RELEASE_*_*_CC_FLAGS    = $(OCPKG_BUILD_OPTIONS_GEN) $(OCPKG_ANAL_OPTIONS_GEN) -Oz -flto -fstack-protector-strong -ftrivial-auto-var-init=pattern

  !include NetworkPkg/NetworkBuildOptions.dsc.inc
