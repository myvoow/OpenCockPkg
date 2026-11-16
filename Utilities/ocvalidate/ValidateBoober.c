/** @file
  Copyright (C) 2018, vit9696. All rights reserved.
  Copyright (C) 2020, PMheart. All rights reserved.

  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "ocvalidate.h"
#include "OcValidateLib.h"

STATIC
UINT32
CheckBooberMmioWhitelist (
  IN  OC_GLOBAL_CONFIG  *Config
  )
{
  UINT32       ErrorCount;
  UINT32       Index;
  CONST CHAR8  *Comment;
  BOOLEAN      IsMmioWhitelistEnabled;
  BOOLEAN      ShouldEnableDevirtualiseMmio;
  BOOLEAN      IsDevirtualiseMmioEnabled;

  ErrorCount                = 0;
  IsDevirtualiseMmioEnabled = Config->Boober.Quirks.DevirtualiseMmio;

  IsMmioWhitelistEnabled       = FALSE;
  ShouldEnableDevirtualiseMmio = FALSE;
  for (Index = 0; Index < Config->Boober.MmioWhitelist.Count; ++Index) {
    Comment                = OC_BLOB_GET (&Config->Boober.MmioWhitelist.Values[Index]->Comment);
    IsMmioWhitelistEnabled = Config->Boober.MmioWhitelist.Values[Index]->Enabled;
    //
    // DevirtualiseMmio should be enabled if at least one entry is enabled.
    //
    ShouldEnableDevirtualiseMmio = IsMmioWhitelistEnabled;

    //
    // Sanitise strings.
    //
    if (!AsciiCommentIsLegal (Comment)) {
      DEBUG ((DEBUG_WARN, "Boober->MmioWhitelist[%u]->Comment contains illegal character!\n", Index));
      ++ErrorCount;
    }
  }

  if (ShouldEnableDevirtualiseMmio && !IsDevirtualiseMmioEnabled) {
    DEBUG ((DEBUG_WARN, "There are enabled entries under Boober->MmioWhitelist, but DevirtualiseMmio is not enabled!\n"));
    ++ErrorCount;
  }

  return ErrorCount;
}

UINT32
CheckBooberPatch (
  IN  OC_GLOBAL_CONFIG  *Config
  )
{
  UINT32       ErrorCount;
  UINT32       Index;
  CONST CHAR8  *Comment;
  CONST CHAR8  *Arch;
  CONST CHAR8  *Identifier;
  CONST UINT8  *Find;
  UINT32       FindSize;
  CONST UINT8  *Replace;
  UINT32       ReplaceSize;
  CONST UINT8  *Mask;
  UINT32       MaskSize;
  CONST UINT8  *ReplaceMask;
  UINT32       ReplaceMaskSize;

  ErrorCount = 0;

  for (Index = 0; Index < Config->Boober.Patch.Count; ++Index) {
    Comment         = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Comment);
    Arch            = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Arch);
    Identifier      = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Identifier);
    Find            = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Find);
    FindSize        = Config->Boober.Patch.Values[Index]->Find.Size;
    Replace         = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Replace);
    ReplaceSize     = Config->Boober.Patch.Values[Index]->Replace.Size;
    Mask            = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->Mask);
    MaskSize        = Config->Boober.Patch.Values[Index]->Mask.Size;
    ReplaceMask     = OC_BLOB_GET (&Config->Boober.Patch.Values[Index]->ReplaceMask);
    ReplaceMaskSize = Config->Boober.Patch.Values[Index]->ReplaceMask.Size;

    //
    // Sanitise strings.
    //
    if (!AsciiCommentIsLegal (Comment)) {
      DEBUG ((DEBUG_WARN, "Boober->Patch[%u]->Comment contains illegal character!\n", Index));
      ++ErrorCount;
    }

    if (!AsciiArchIsLegal (Arch, FALSE)) {
      DEBUG ((DEBUG_WARN, "Boober->Patch[%u]->Arch is borked (Can only be Any, i386, and x86_64)!\n", Index));
      ++ErrorCount;
    }

    if (!AsciiIdentifierIsLegal (Identifier, FALSE)) {
      DEBUG ((DEBUG_WARN, "Boober->Patch[%u]->Identifier contains illegal character!\n", Index));
      ++ErrorCount;
    }

    //
    // Checks for size.
    //
    ErrorCount += ValidatePatch (
                    "Boober->Patch",
                    Index,
                    FALSE,
                    Find,
                    FindSize,
                    Replace,
                    ReplaceSize,
                    Mask,
                    MaskSize,
                    ReplaceMask,
                    ReplaceMaskSize
                    );
  }

  return ErrorCount;
}

STATIC
UINT32
CheckBooberQuirks (
  IN  OC_GLOBAL_CONFIG  *Config
  )
{
  UINT32                ErrorCount;
  UINT32                Index;
  OC_UEFI_DRIVER_ENTRY  *DriverEntry;
  CONST CHAR8           *Driver;
  UINT8                 MaxSlide;
  BOOLEAN               IsAllowRelocationBlockEnabled;
  BOOLEAN               IsProvideCustomSlideEnabled;
  BOOLEAN               IsEnableSafeModeSlideEnabled;
  BOOLEAN               IsDisableVariableWriteEnabled;
  BOOLEAN               IsEnableWriteUnprotectorEnabled;
  BOOLEAN               IsClearTaskSwitchBitEnabled;
  BOOLEAN               HasOpenRuntimeEfiDriver;
  INT8                  ResizeAppleGpuBars;

  ErrorCount                      = 0;
  IsAllowRelocationBlockEnabled   = Config->Boober.Quirks.AllowRelocationBlock;
  IsProvideCustomSlideEnabled     = Config->Boober.Quirks.ProvideCustomSlide;
  IsEnableSafeModeSlideEnabled    = Config->Boober.Quirks.EnableSafeModeSlide;
  IsDisableVariableWriteEnabled   = Config->Boober.Quirks.DisableVariableWrite;
  IsEnableWriteUnprotectorEnabled = Config->Boober.Quirks.EnableWriteUnprotector;
  IsClearTaskSwitchBitEnabled     = Config->Boober.Quirks.ClearTaskSwitchBit;
  HasOpenRuntimeEfiDriver         = FALSE;
  MaxSlide                        = Config->Boober.Quirks.ProvideMaxSlide;
  ResizeAppleGpuBars              = Config->Boober.Quirks.ResizeAppleGpuBars;

  for (Index = 0; Index < Config->Uefi.Drivers.Count; ++Index) {
    DriverEntry = Config->Uefi.Drivers.Values[Index];
    Driver      = OC_BLOB_GET (&DriverEntry->Path);

    //
    // Skip sanitising UEFI->Drivers as it will be performed when checking UEFI section.
    //
    if (DriverEntry->Enabled && (AsciiStrCmp (Driver, "OpenRuntime.efi") == 0)) {
      HasOpenRuntimeEfiDriver = TRUE;
    }
  }

  if (!HasOpenRuntimeEfiDriver) {
    if (IsProvideCustomSlideEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->ProvideCustomSlide is enabled, but OpenRuntime.efi is not loaded at UEFI->Drivers!\n"));
      ++ErrorCount;
    }

    if (IsDisableVariableWriteEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->DisableVariableWrite is enabled, but OpenRuntime.efi is not loaded at UEFI->Drivers!\n"));
      ++ErrorCount;
    }

    if (IsEnableWriteUnprotectorEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->EnableWriteUnprotector is enabled, but OpenRuntime.efi is not loaded at UEFI->Drivers!\n"));
      ++ErrorCount;
    }

    if (IsClearTaskSwitchBitEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->ClearTaskSwitchBit is enabled, but OpenRuntime.efi is not loaded at UEFI->Drivers!\n"));
      ++ErrorCount;
    }
  }

  if (!IsProvideCustomSlideEnabled) {
    if (IsAllowRelocationBlockEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->AllowRelocationBlock is enabled, but ProvideCustomSlide is not enabled altogether!\n"));
      ++ErrorCount;
    }

    if (IsEnableSafeModeSlideEnabled) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->EnableSafeModeSlide is enabled, but ProvideCustomSlide is not enabled altogether!\n"));
      ++ErrorCount;
    }

    if (MaxSlide > 0) {
      DEBUG ((DEBUG_WARN, "Boober->Quirks->ProvideMaxSlide is set to %u, but ProvideCustomSlide is not enabled altogether!\n", MaxSlide));
      ++ErrorCount;
    }
  }

  if (ResizeAppleGpuBars > 10) {
    DEBUG ((DEBUG_WARN, "Boober->Quirks->ResizeAppleGpuBars is set to %d, which is unsupported by macOS!\n", Config->Boober.Quirks.ResizeAppleGpuBars));
    ++ErrorCount;
  } else if (ResizeAppleGpuBars > 8) {
    DEBUG ((DEBUG_WARN, "Boober->Quirks->ResizeAppleGpuBars is set to %d, which is unstable with macOS sleep-wake!\n", Config->Boober.Quirks.ResizeAppleGpuBars));
    ++ErrorCount;
  } else if (ResizeAppleGpuBars > 0) {
    DEBUG ((DEBUG_WARN, "Boober->Quirks->ResizeAppleGpuBars is set to %d, which is not useful for macOS!\n", Config->Boober.Quirks.ResizeAppleGpuBars));
    ++ErrorCount;
  }

  return ErrorCount;
}

UINT32
CheckBoober (
  IN  OC_GLOBAL_CONFIG  *Config
  )
{
  UINT32               ErrorCount;
  UINTN                Index;
  STATIC CONFIG_CHECK  BooberCheckers[] = {
    &CheckBooberMmioWhitelist,
    &CheckBooberPatch,
    &CheckBooberQuirks
  };

  DEBUG ((DEBUG_VERBOSE, "config loaded into %a!\n", __func__));

  ErrorCount = 0;

  for (Index = 0; Index < ARRAY_SIZE (BooberCheckers); ++Index) {
    ErrorCount += BooberCheckers[Index](Config);
  }

  return ReportError (__func__, ErrorCount);
}
