/*
 * File: show_ver.c
 * Version: 1.0,  2002-6-4
 * Purpose: console program to display file VERSIONINFO text

 * Copyright (c) 2002 by Ted Peck <tpeck@roundwave.com>
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * THIS CODE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED. IN PARTICULAR, NO WARRANTY IS MADE
 * THAT THE CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGING. IN NO EVENT WILL THE AUTHOR BE LIABLE FOR ANY COSTS OR DAMAGES
 * ARISING FROM ANY USE OF THIS CODE. NO USE OF THIS CODE IS AUTHORIZED EXCEPT UNDER
 * THIS DISCLAIMER.
 *
 * Use at your own risk!
 */

#include <windows.h>

#include "envtool.h"

struct VS_VERSIONINFO {
       WORD              wLength;
       WORD              wValueLength;
       WORD              wType;
       WCHAR             szKey[1];
       WORD              Padding1[1];
       VS_FIXEDFILEINFO  Value;
       WORD              Padding2[1];
       WORD              Children[1];
     };

struct String {
       WORD          wLength;
       WORD          wValueLength;
       WORD          wType;
       WCHAR         szKey[1];
       WORD          Padding[1];
       WORD          Value[1];
     };

struct StringTable {
       WORD          wLength;
       WORD          wValueLength;
       WORD          wType;
       WCHAR         szKey[1];
       WORD          Padding[1];
       struct String Children[1];
     };

struct StringFileInfo {
       WORD               wLength;
       WORD               wValueLength;
       WORD               wType;
       WCHAR              szKey[1];
       WORD               Padding[1];
       struct StringTable Children[1];
     };

struct Var {
       WORD       wLength;
       WORD       wValueLength;
       WORD       wType;
       WCHAR      szKey[1];
       WORD       Padding[1];
       DWORD      Value[1];
     };

struct VarFileInfo {
       WORD       wLength;
       WORD       wValueLength;
       WORD       wType;
       WCHAR      szKey[1];
       WORD       Padding[1];
       struct Var Children[1];
     };

/* ----- VS_VERSION.dwFileFlags ----- */
#define S_VS_FFI_SIGNATURE        "VS_FFI_SIGNATURE"
#define S_VS_FFI_STRUCVERSION     "VS_FFI_STRUCVERSION"
#define S_VS_FFI_FILEFLAGSMASK    "VS_FFI_FILEFLAGSMASK"

/* ----- VS_VERSION.dwFileFlags ----- */
#define S_VS_FF_DEBUG             "VS_FF_DEBUG"
#define S_VS_FF_PRERELEASE        "VS_FF_PRERELEASE"
#define S_VS_FF_PATCHED           "VS_FF_PATCHED"
#define S_VS_FF_PRIVATEBUILD      "VS_FF_PRIVATEBUILD"
#define S_VS_FF_INFOINFERRED      "VS_FF_INFOINFERRED"
#define S_VS_FF_SPECIALBUILD      "VS_FF_SPECIALBUILD"

static DWORD last_err = 0;
static BOOL  print_it = FALSE;

#define VER_ERROR(fname)  do {                                                  \
                            last_err = GetLastError();                          \
                            DEBUGF (1, "Unable to access file \"%s\":\n  %s\n", \
                                    fname, win_strerror(last_err));            \
                          } while (0)

static void do_printf (const char *fmt, ...) ATTR_PRINTF (1,2);

static void do_printf (const char *fmt, ...)
{
  va_list args;

  if (!print_it)
     return;

  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
}

static const char *showFileFlags (DWORD dwFileFlags)
{
  static char s[200];
  int pos = 0;

  s[pos] = '\0';

#define VS_FF_KNOWNFLAGS (VS_FF_DEBUG         \
                        | VS_FF_PRERELEASE    \
                        | VS_FF_PATCHED       \
                        | VS_FF_PRIVATEBUILD  \
                        | VS_FF_INFOINFERRED  \
                        | VS_FF_SPECIALBUILD)

  if (dwFileFlags & ~VS_FF_KNOWNFLAGS)
     pos += sprintf (&s[pos], "0x%lX", dwFileFlags & ~VS_FF_KNOWNFLAGS);

  if (dwFileFlags & VS_FF_DEBUG)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_DEBUG, sizeof (S_VS_FF_DEBUG));
    pos += sizeof (S_VS_FF_DEBUG) - 1;
  }

  if (dwFileFlags & VS_FF_PRERELEASE)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_PRERELEASE, sizeof (S_VS_FF_PRERELEASE));
    pos += sizeof (S_VS_FF_PRERELEASE) - 1;
  }

  if (dwFileFlags & VS_FF_PATCHED)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_PATCHED, sizeof (S_VS_FF_PATCHED));
    pos += sizeof (S_VS_FF_PATCHED) - 1;
  }

  if (dwFileFlags & VS_FF_PRIVATEBUILD)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_PRIVATEBUILD, sizeof (S_VS_FF_PRIVATEBUILD));
    pos += sizeof (S_VS_FF_PRIVATEBUILD) - 1;
  }

  if (dwFileFlags & VS_FF_INFOINFERRED)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_INFOINFERRED, sizeof (S_VS_FF_INFOINFERRED));
    pos += sizeof (S_VS_FF_INFOINFERRED) - 1;
  }

  if (dwFileFlags & VS_FF_SPECIALBUILD)
  {
    if (pos)
    {
      memcpy (&s[pos], " | ", 3);
      pos += 3;
    }
    memcpy (&s[pos], S_VS_FF_SPECIALBUILD, sizeof (S_VS_FF_SPECIALBUILD));
    pos += sizeof (S_VS_FF_SPECIALBUILD) - 1;
  }

  if (!pos)
    memcpy (s, "0", 2);
  return s;
}

/* ----- VS_VERSION.dwFileOS ----- */
#define S_VOS_UNKNOWN             "VOS_UNKNOWN"
#define S_VOS_DOS                 "VOS_DOS"
#define S_VOS_OS216               "VOS_OS216"
#define S_VOS_OS232               "VOS_OS232"
#define S_VOS_NT                  "VOS_NT"

#define S_VOS__BASE               "VOS__BASE"
#define S_VOS__WINDOWS16          "VOS__WINDOWS16"
#define S_VOS__PM16               "VOS__PM16"
#define S_VOS__PM32               "VOS__PM32"
#define S_VOS__WINDOWS32          "VOS__WINDOWS32"

#define S_VOS_DOS_WINDOWS16       "VOS_DOS_WINDOWS16"
#define S_VOS_DOS_WINDOWS32       "VOS_DOS_WINDOWS32"
#define S_VOS_OS216_PM16          "VOS_OS216_PM16"
#define S_VOS_OS232_PM32          "VOS_OS232_PM32"
#define S_VOS_NT_WINDOWS32        "VOS_NT_WINDOWS32"

static const char *showFileOS (DWORD dwFileOS)
{
  switch (dwFileOS)
  {
    case VOS_UNKNOWN:
         return S_VOS_UNKNOWN;
    case VOS_DOS:
         return S_VOS_DOS;
    case VOS_OS216:
         return S_VOS_OS216;
    case VOS_OS232:
         return S_VOS_OS232;
    case VOS_NT:
         return S_VOS_NT;
#if 0
    case VOS__BASE:
         return S_VOS__BASE;
#endif
    case VOS__WINDOWS16:
         return S_VOS__WINDOWS16;
    case VOS__PM16:
         return S_VOS__PM16;
    case VOS__PM32:
         return S_VOS__PM32;
    case VOS__WINDOWS32:
         return S_VOS__WINDOWS32;

    case VOS_DOS_WINDOWS16:
         return S_VOS_DOS_WINDOWS16;
    case VOS_DOS_WINDOWS32:
         return S_VOS_DOS_WINDOWS32;
    case VOS_OS216_PM16:
         return S_VOS_OS216_PM16;
    case VOS_OS232_PM32:
         return S_VOS_OS232_PM32;
    case VOS_NT_WINDOWS32:
         return S_VOS_NT_WINDOWS32;
    default:
         return ("Unknown FileOS");
  }
}

/* ----- VS_VERSION.dwFileType ----- */

#define S_VFT_UNKNOWN      "VFT_UNKNOWN"
#define S_VFT_APP          "VFT_APP"
#define S_VFT_DLL          "VFT_DLL"
#define S_VFT_DRV          "VFT_DRV"
#define S_VFT_FONT         "VFT_FONT"
#define S_VFT_VXD          "VFT_VXD"
#define S_VFT_STATIC_LIB   "VFT_STATIC_LIB"

#if 0
static const char *showFileType (DWORD dwFileType)
{
  switch (dwFileType)
  {
    case VFT_UNKNOWN:
         return S_VFT_UNKNOWN;
    case VFT_APP:
         return S_VFT_APP;
    case VFT_DLL:
         return S_VFT_DLL;
    case VFT_DRV:
         return S_VFT_DRV;
    case VFT_FONT:
         return S_VFT_FONT;
    case VFT_VXD:
         return S_VFT_VXD;
    case VFT_STATIC_LIB:
         return S_VFT_STATIC_LIB;
    default:
         return ("Unknown FileType");
  }
}
#else
static const char *showFileType (DWORD type)
{
  return (type == VFT_UNKNOWN    ? "VFT_UNKNOWN"    :
          type == VFT_APP        ? "VFT_APP"        :
          type == VFT_DLL        ? "VFT_DLL"        :
          type == VFT_DRV        ? "VFT_DRV"        :
          type == VFT_FONT       ? "VFT_FONT"       :
          type == VFT_VXD        ? "VFT_VXD"        :
          type == VFT_STATIC_LIB ? "VFT_STATIC_LIB" :
                                   "Unknown FileType");
}
#endif

/* ----- VS_VERSION.dwFileSubtype for VFT_WINDOWS_DRV ----- */
#define S_VFT2_UNKNOWN         "VFT2_UNKNOWN"
#define S_VFT2_DRV_PRINTER     "VFT2_DRV_PRINTER"
#define S_VFT2_DRV_KEYBOARD    "VFT2_DRV_KEYBOARD"
#define S_VFT2_DRV_LANGUAGE    "VFT2_DRV_LANGUAGE"
#define S_VFT2_DRV_DISPLAY     "VFT2_DRV_DISPLAY"
#define S_VFT2_DRV_MOUSE       "VFT2_DRV_MOUSE"
#define S_VFT2_DRV_NETWORK     "VFT2_DRV_NETWORK"
#define S_VFT2_DRV_SYSTEM      "VFT2_DRV_SYSTEM"
#define S_VFT2_DRV_INSTALLABLE "VFT2_DRV_INSTALLABLE"
#define S_VFT2_DRV_SOUND       "VFT2_DRV_SOUND"
#define S_VFT2_DRV_COMM        "VFT2_DRV_COMM"
#define S_VFT2_DRV_INPUTMETHOD "VFT2_DRV_INPUTMETHOD"

/* ----- VS_VERSION.dwFileSubtype for VFT_WINDOWS_FONT ----- */
#define S_VFT2_FONT_RASTER     "VFT2_FONT_RASTER"
#define S_VFT2_FONT_VECTOR     "VFT2_FONT_VECTOR"
#define S_VFT2_FONT_TRUETYPE   "VFT2_FONT_TRUETYPE"

static const char *showFileSubtype (DWORD dwFileType, DWORD dwFileSubtype)
{
  static char s[50];

  switch (dwFileType)
  {
    case VFT_DRV:
         switch (dwFileSubtype)
         {
           case VFT2_UNKNOWN:
                return "FileSubtype: " S_VFT2_UNKNOWN;
           case VFT2_DRV_PRINTER:
                return "FileSubtype: " S_VFT2_DRV_PRINTER;
           case VFT2_DRV_KEYBOARD:
                return "FileSubtype: " S_VFT2_DRV_KEYBOARD;
           case VFT2_DRV_LANGUAGE:
                return "FileSubtype: " S_VFT2_DRV_LANGUAGE;
           case VFT2_DRV_DISPLAY:
                return "FileSubtype: " S_VFT2_DRV_DISPLAY;
           case VFT2_DRV_MOUSE:
                return "FileSubtype: " S_VFT2_DRV_MOUSE;
           case VFT2_DRV_NETWORK:
                return "FileSubtype: " S_VFT2_DRV_NETWORK;
           case VFT2_DRV_SYSTEM:
                return "FileSubtype: " S_VFT2_DRV_SYSTEM;
           case VFT2_DRV_INSTALLABLE:
                return "FileSubtype: " S_VFT2_DRV_INSTALLABLE;
           case VFT2_DRV_SOUND:
                return "FileSubtype: " S_VFT2_DRV_SOUND;
           case VFT2_DRV_COMM:
                return "FileSubtype: " S_VFT2_DRV_COMM;
           case VFT2_DRV_INPUTMETHOD:
                return "FileSubtype: " S_VFT2_DRV_INPUTMETHOD;
           default:
                s[0] = '\0';
                sprintf (s, "Unknown FileSubtype: 0x%lX", dwFileSubtype);
                return s;
         }
         break;

    case VFT_FONT:
         switch (dwFileSubtype)
         {
           case VFT2_FONT_RASTER:
                return "FileSubtype: " S_VFT2_FONT_RASTER;
           case VFT2_FONT_VECTOR:
                return "FileSubtype: " S_VFT2_FONT_VECTOR;
           case VFT2_FONT_TRUETYPE:
                return "FileSubtype: " S_VFT2_FONT_TRUETYPE;
           default:
                s[0] = '\0';
                sprintf (s, "Unknown FileSubtype: 0x%lX", dwFileSubtype);
                return (s);
         }
         break;

    default:
         s[0] = '\0';
         if (dwFileSubtype)
            sprintf (s, ", FileSubtype: 0x%lX", dwFileSubtype);
         return (s);
  }
}

static void show_FIXEDFILEINFO (const VS_FIXEDFILEINFO *pValue, struct ver_info *ver_p)
{
  ASSERT (VS_FFI_SIGNATURE == pValue->dwSignature);
  ASSERT (VS_FFI_STRUCVERSION == pValue->dwStrucVersion);

  ver_p->val_1 = (UINT) (pValue->dwFileVersionMS >> 16);
  ver_p->val_2 = (UINT) (pValue->dwFileVersionMS & 0xFFFF);
  ver_p->val_3 = (UINT) (pValue->dwFileVersionLS >> 16);
  ver_p->val_4 = (UINT) (pValue->dwFileVersionLS & 0xFFFF);

  do_printf ("  Signature:      0x%08lX\n", pValue->dwSignature);
  do_printf ("  StrucVersion:   %lu.%lu\n", pValue->dwStrucVersion >> 16, pValue->dwStrucVersion & 0xFFFF);
  do_printf ("  FileVersion:    %u.%u.%u.%u\n", ver_p->val_1, ver_p->val_2, ver_p->val_3, ver_p->val_4);

  do_printf ("  ProductVersion: %u.%u.%u.%u\n", (UINT) (pValue->dwProductVersionMS >> 16),
                                                (UINT) (pValue->dwProductVersionMS & 0xFFFF),
                                                (UINT) (pValue->dwProductVersionLS >> 16),
                                                (UINT) (pValue->dwProductVersionLS & 0xFFFF));

  do_printf ("  FileFlagsMask:  0x%lX\n", pValue->dwFileFlagsMask);

  if (pValue->dwFileFlags)
       do_printf ("  FileFlags:      0x%lX (%s)\n", pValue->dwFileFlags, showFileFlags(pValue->dwFileFlags));
  else do_printf ("  FileFlags:      0\n");

  do_printf ("  FileOS:         %s\n", showFileOS (pValue->dwFileOS));
  do_printf ("  FileType:       %s%s\n", showFileType (pValue->dwFileType),
                                      showFileSubtype (pValue->dwFileType, pValue->dwFileSubtype));
  do_printf ("  FileDate:       %lX.%lX\n", pValue->dwFileDateMS, pValue->dwFileDateLS);
}


#define ROUND_OFS(a,b,r)    (((BYTE*)(b) - (BYTE*)(a) + ((r)-1)) & ~((r)-1))
#define ROUND_POS(b, a, r)  (((BYTE*)(a)) + ROUND_OFS(a,b,r))

static void get_version_data (const void *pVer, DWORD size, struct ver_info *ver_p)
{
  /* Interpret the VS_VERSIONINFO header pseudo-struct.
   */
  const struct VS_VERSIONINFO *pVS = (const struct VS_VERSIONINFO*) pVer;
  const BYTE                  *pVt;

#if 0
  BYTE* nEndRaw   = ROUND_POS((((BYTE*)pVer) + size), pVer, 4);
  BYTE* nEndNamed = ROUND_POS((((BYTE*) pVS) + pVS->wLength), pVS, 4);
  ASSERT (nEndRaw == nEndNamed); // size reported from GetFileVersionInfoSize is much padded for some reason...
#endif

  ASSERT (!wcscmp(pVS->szKey, L"VS_VERSION_INFO"));
  do_printf (" (type:%d)\n", pVS->wType);

  pVt = (const BYTE*) &pVS->szKey [wcslen(pVS->szKey)+1];

  const VS_FIXEDFILEINFO *pValue = (const VS_FIXEDFILEINFO *) ROUND_POS (pVt, pVS, 4);

  if (pVS->wValueLength)
     show_FIXEDFILEINFO (pValue, ver_p); /* Show the 'Value' element */

  /* Iterate over the 'Children' elements of VS_VERSIONINFO (either a
   * 'struct StringFileInfo' or 'struct VarFileInfo').
   */
  const struct StringFileInfo *pSFI = (struct StringFileInfo*) ROUND_POS (((BYTE*)pValue) + pVS->wValueLength, pValue, 4);

  for (; ((const BYTE*)pSFI) < (((const BYTE*)pVS) + pVS->wLength);
         pSFI = (const struct StringFileInfo*) ROUND_POS ((((const BYTE*)pSFI) + pSFI->wLength), pSFI, 4))
  {                             /* 'struct StringFileInfo' or 'struct VarFileInfo' */
    if (!wcscmp(pSFI->szKey, L"StringFileInfo"))
    {
      /* The current child is a 'struct StringFileInfo' element
       */
      ASSERT (1 == pSFI->wType);
      ASSERT (!pSFI->wValueLength);

      /* Iterate through the 'struct StringTable' elements of 'struct StringFileInfo'
       */
      const struct StringTable *pST = (const struct StringTable*) ROUND_POS (&pSFI->szKey[wcslen(pSFI->szKey)+1], pSFI, 4);

      for (; (const BYTE*)pST < (const BYTE*)pSFI + pSFI->wLength;
            pST = (const struct StringTable*) ROUND_POS ((BYTE*)pST + pST->wLength, pST, 4))
      {
        do_printf ("  LangID:         %S\n", pST->szKey);

        ASSERT (!pST->wValueLength);

        /* Iterate through the String elements of 'struct StringTable'
         */
        const struct String *pS = (const struct String*) ROUND_POS (&pST->szKey[wcslen(pST->szKey) + 1], pST, 4);

        for (; (const BYTE*)pS < (const BYTE*)pST + pST->wLength;
             pS = (const struct String*) ROUND_POS ((BYTE*)pS + pS->wLength, pS, 4))
        {
          const wchar_t *psVal = (const wchar_t*) ROUND_POS (&pS->szKey[wcslen(pS->szKey)+1], pS, 4);

          do_printf ("  %-17S: %.*S\n", pS->szKey, pS->wValueLength, psVal); // print <sKey> : <sValue>
        }
      }
    }
    else
    {
      /* The current child is a 'struct VarFileInfo' element */
      ASSERT (1 == pSFI->wType); /* ?? it just seems to be this way... */

      const struct VarFileInfo *pVFI = (const struct VarFileInfo*) pSFI;

      ASSERT (!wcscmp(pVFI->szKey, L"VarFileInfo"));
      ASSERT (!pVFI->wValueLength);

      /* Iterate through the Var elements of 'struct VarFileInfo'
       * (there should be only one, but just in case...)
       */
      const struct Var *pV = (const struct Var*) ROUND_POS (&pVFI->szKey[wcslen(pVFI->szKey)+1], pVFI, 4);

      for (; (const BYTE*)pV < (const BYTE*)pVFI + pVFI->wLength;
           pV = (const struct Var*) ROUND_POS ((const BYTE*)pV + pV->wLength, pV, 4))
      {
        do_printf ("  %S:    ", pV->szKey);

        /* Iterate through the array of pairs of 16-bit language ID values that make up
         * the standard 'Translation' VarFileInfo element.
         */
        const WORD *wpos, *pwV = (const WORD*) ROUND_POS (&pV->szKey[wcslen(pV->szKey)+1], pV, 4);

        for (wpos = pwV; (const BYTE*)wpos < (const BYTE*)pwV + pV->wValueLength; wpos += 2)
        {
          WORD w1 = wpos[0];
          WORD w2 = wpos[1];
          do_printf ("%04X%04X ", w1, w2);
        }
        do_printf ("\n");
      }
    }
  }
  ASSERT ((const BYTE*)pSFI == ROUND_POS ((const BYTE*)pVS + pVS->wLength, pVS, 4));
}

int show_version_info (const char *file, struct ver_info *ver)
{
  DWORD  dummy = 0;
  DWORD  size = GetFileVersionInfoSizeA (file, &dummy);
  void  *ver_data;

  last_err = 0;
  memset (ver, 0, sizeof(*ver));

  if (!size)
  {
    VER_ERROR (file);
    return (0);
  }

  ver_data = CALLOC (size,1);
  if (!GetFileVersionInfoA(file, 0, size, ver_data))
  {
    VER_ERROR (file);
    FREE (ver_data);
    return (0);
  }

  print_it = (verbose >= 3);

  do_printf ("VERSIONINFO dump for file \"%s\":\n", file);
  if (print_it)
     hex_dump (ver_data, size);

  print_it = (verbose >= 1);

  do_printf ("VERSIONINFO: ");
  get_version_data (ver_data, size, ver);

  FREE (ver_data);
  return (1);
}


