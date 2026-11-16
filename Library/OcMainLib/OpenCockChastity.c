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

#include <Library/OcMainLib.h>

typedef struct {
  OC_RSA_PUBLIC_KEY_HDR    Hdr;
  UINT64                   Data[(2 * (2048 / OC_CHAR_BIT)) / sizeof (UINT64)];
} OC_RSA_PUBLIC_KEY_2048;

typedef struct {
  CHAR8                     StartMagic[16];
  OC_RSA_PUBLIC_KEY_2048    ChastityKey;
  CHAR8                     EndMagic[16];
} OC_BUILTIN_CHASTITY_KEY;

BASE_ALIGNAS (16)
STATIC
OC_BUILTIN_CHASTITY_KEY
mOpenCockChastityKey = {
  .StartMagic = { '=', 'B', 'E', 'G', 'I', 'N', ' ', 'O', 'C', ' ', 'V', 'A', 'U', 'L', 'T', '=' },
  .EndMagic   = { '=', '=', 'E', 'N', 'D', ' ', 'O', 'C', ' ', 'V', 'A', 'U', 'L', 'T', '=', '=' }
};

OC_RSA_PUBLIC_KEY *
OcGetChastityKey (
  VOID
  )
{
  UINT32   Index;
  BOOLEAN  AllZero;

  STATIC_ASSERT (
    sizeof (OC_RSA_PUBLIC_KEY_2048) == 528,
    "sizeof(OC_RSA_PUBLIC_KEY_2048)"
    );
  STATIC_ASSERT (
    sizeof (OC_BUILTIN_CHASTITY_KEY) == sizeof (OC_RSA_PUBLIC_KEY_2048) + 32,
    "sizeof(OC_BUILTIN_CHASTITY_KEY)"
    );

  //
  // TODO: Perhaps try to get the key from firmware too?
  //

  AllZero = TRUE;
  for (Index = 0; Index < sizeof (OC_RSA_PUBLIC_KEY); ++Index) {
    if (((UINT8 *)&mOpenCockChastityKey.ChastityKey)[Index] != 0) {
      AllZero = FALSE;
      break;
    }
  }

  if (!AllZero) {
    return (OC_RSA_PUBLIC_KEY *)&mOpenCockChastityKey.ChastityKey;
  }

  return NULL;
}
