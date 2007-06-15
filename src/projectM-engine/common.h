/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id$
 *
 * $Log$
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>

#ifdef _MSC_VER
#   define strcasecmp(s, t) _strcmpi(s, t)
#endif

#ifdef DEBUG
extern FILE *debugFile;
#endif

#ifdef MACOS
#include <stdio.h>
extern FILE *fmemopen(void *buf, size_t len, const char *pMode);
#endif /** MACOS */

#include "dlldefs.h"

#define DEFAULT_FONT_PATH "/home/carm/fonts/courier1.glf"
#define MAX_TOKEN_SIZE 512
#define MAX_PATH_SIZE 4096

#define STRING_BUFFER_SIZE 1024*150
#define STRING_LINE_SIZE 1024

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif 

#define PROJECTM_FILE_EXTENSION ".prjm"
#define MILKDROP_FILE_EXTENSION ".milk"

#define MAX_DOUBLE_SIZE  10000000.0
#define MIN_DOUBLE_SIZE -10000000.0

#define MAX_INT_SIZE  10000000
#define MIN_INT_SIZE -10000000

#define DEFAULT_DOUBLE_IV 0.0 /* default float initial value */
#define DEFAULT_DOUBLE_LB MIN_DOUBLE_SIZE /* default float lower bound */
#define DEFAULT_DOUBLE_UB MAX_DOUBLE_SIZE /* default float upper bound */

#ifdef WIN32
#include <float.h>
#define isnan _isnan#endif /** WIN32 */
#endif /** WIN32 */

inline void DWRITE( char *fmt, ... ) {
    va_list args;
    va_start( args, fmt );
#ifdef DEBUG
#ifdef MACOS
    if ( debugFile != NULL ) {\
        vfprintf( debugFile, fmt, args );\
        fflush( debugFile );\
      } else {\
        vprintf( fmt, args );\
      }
#endif
#ifdef LINUX
 vprintf( fmt, args );
#endif
#endif
    va_end( args );
  }

#endif
