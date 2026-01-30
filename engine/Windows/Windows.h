/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(USEWIN)

/*  If defined, the following flags inhibit definition
 *     of the indicated items.
 *
 *  NOGDICAPMASKS     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
 *  NOVIRTUALKEYCODES - VK_*
 *  NOWINMESSAGES     - WM_*, EM_*, LB_*, CB_*
 *  NOWINSTYLES       - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
 *  NOSYSMETRICS      - SM_*
 *  NOMENUS           - MF_*
 *  NOICONS           - IDI_*
 *  NOKEYSTATES       - MK_*
 *  NOSYSCOMMANDS     - SC_*
 *  NORASTEROPS       - Binary and Tertiary raster ops
 *  NOSHOWWINDOW      - SW_*
 *  OEMRESOURCE       - OEM Resource values
 *  NOATOM            - Atom Manager routines
 *  NOCLIPBOARD       - Clipboard routines
 *  NOCOLOR           - Screen colors
 *  NOCTLMGR          - Control and Dialog routines
 *  NODRAWTEXT        - DrawText() and DT_*
 *  NOGDI             - All GDI defines and routines
 *  NOKERNEL          - All KERNEL defines and routines
 *  NOUSER            - All USER defines and routines
 *  NONLS             - All NLS defines and routines
 *  NOMB              - MB_* and MessageBox()
 *  NOMEMMGR          - GMEM_*, LMEM_*, GHND, LHND, associated routines
 *  NOMETAFILE        - typedef METAFILEPICT
 *  NOMINMAX          - Macros min(a,b) and max(a,b)
 *  NOMSG             - typedef MSG and associated routines
 *  NOOPENFILE        - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
 *  NOSCROLL          - SB_* and scrolling routines
 *  NOSERVICE         - All Service Controller routines, SERVICE_ equates, etc.
 *  NOSOUND           - Sound driver routines
 *  NOTEXTMETRIC      - typedef TEXTMETRIC and associated routines
 *  NOWH              - SetWindowsHook and WH_*
 *  NOWINOFFSETS      - GWL_*, GCL_*, associated routines
 *  NOCOMM            - COMM driver routines
 *  NOKANJI           - Kanji support stuff.
 *  NOHELP            - Help engine interface.
 *  NOPROFILER        - Profiler interface.
 *  NODEFERWINDOWPOS  - DeferWindowPos routines
 *  NOMCX             - Modem Configuration Extensions
 */

#ifndef GDICAPMASKS
#define NOGDICAPMASKS
#endif
#ifndef VIRTUALKEYCODES
#define NOVIRTUALKEYCODES
#endif
#ifndef WINMESSAGES
#define NOWINMESSAGES
#endif
#ifndef WINSTYLES
#define NOWINSTYLES
#endif
#ifndef SYSMETRICS
#define NOSYSMETRICS
#endif
#ifndef MENUS
#define NOMENUS
#endif
#ifndef ICONS
#define NOICONS
#endif
#ifndef KEYSTATES
#define NOKEYSTATES
#endif
#ifndef SYSCOMMANDS
#define NOSYSCOMMANDS
#endif
#ifndef RASTEROPS
#define NORASTEROPS
#endif
#ifndef SHOWWINDOW
#define NOSHOWWINDOW
#endif
#ifndef USE_OEMRESOURCE
#define OEMRESOURCE
#endif
#ifndef ATOM
#define NOATOM
#endif
#ifndef CLIPBOARD
#define NOCLIPBOARD
#endif
#ifndef COLOR
#define NOCOLOR
#endif
#ifndef CTLMGR
#define NOCTLMGR
#endif
#ifndef DRAWTEXT
#define NODRAWTEXT
#endif
#ifndef GDI
#define NOGDI
#endif
#ifndef KERNEL
#define NOKERNEL
#endif
#ifndef USER
#define NOUSER
#endif
#ifndef NLS
#define NONLS
#endif
#ifndef MB
#define NOMB
#endif
#ifndef MEMMGR
#define NOMEMMGR
#endif
#ifndef METAFILE
#define NOMETAFILE
#endif
#ifndef MINMAX
#define NOMINMAX
#endif
#ifndef MSG
#define NOMSG
#endif
#ifndef OPENFILE
#define NOOPENFILE
#endif
#ifndef SCROLL
#define NOSCROLL
#endif
#ifndef SERVICE
#define NOSERVICE
#endif
#ifndef SOUND
#define NOSOUND
#endif
#ifndef TEXTMETRIC
#define NOTEXTMETRIC
#endif
#ifndef WH
#define NOWH
#endif
#ifndef WINOFFSETS
#define NOWINOFFSETS
#endif
#ifndef COMM
#define NOCOMM
#endif
#ifndef KANJI
#define NOKANJI
#endif
#ifndef HELP
#define NOHELP
#endif
#ifndef PROFILER
#define NOPROFILER
#endif
#ifndef DEFERWINDOWPOS
#define NODEFERWINDOWPOS
#endif
#ifndef MCX
#define NOMCX
#endif

#define	WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00 // Windows 10
#include <Windows.h>

#endif // defined(_WIN32) || defined(_WIN64)