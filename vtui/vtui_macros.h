// vtui, type, helper, and error macros

/*
The Clear BSD License

Copyright (c) 2021 Valyrie Autumn
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the disclaimer
below) provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

     * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

     * Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef vtui_macros_h
#define vtui_macros_h

// ## INT32_T & UINT32_T TYPES ##

// a fixed, 32 bit integer type is used in various parts of vtui. if the user
// does not supply their own, vtui will first attempt to use <stdint.h> if
// C99 is supported, otherwise it will fall back to long int, which is at least
// 32 bits in size, though may be larger, depending on platform.

// should you be compiling vtui with a C99 compatible compiler, it is
// reccomended you let the macros include <stdint.h>. if your compiler does not
// support C99, it is reccomended you provide your own int32_t and uint32_t
// types, the typedefs for which will depend on your target platform.

#ifndef VTUI_INT32_T

// first check for C99 support
#if __STDC_VERSION__ >= 199901L

#include <stdint.h>

#define VTUI_INT32_T int32_t

#else
// no C99 support, fallback to long int

// guranteed to be at least 32 bits
#define VTUI_INT32_T long int

#endif
#endif

#ifndef VTUI_UINT32_T

// first check for C99 support
#if __STDC_VERSION__ >= 199901L

#include <stdint.h>

#define VTUI_UINT32_T uint32_t

#else
// no C99 support, fallback to long int

// guranteed to be at least 32 bits
#define VTUI_UINT32_T unsigned long int

#endif
#endif

// ## VTUI SIZE_T TYPE ##

// much like vtui_int32_t and vtui_uint32_t, vtui needs a size_t; override this
// if you don't want <stddef.h> pulled into the environment

#ifndef VTUI_SIZE_T

#include <stddef.h>

#define VTUI_SIZE_T size_t

#endif

// ## VTUI_BOOL TYPE ##

// also a lot like u?int32_t and size_t, in one or two little places vtui needs
// a boolean type

#ifndef VTUI_BOOL_T

// first check for C99 support
#if __STDC_VERSION__ >= 199901L

#include <stdbool.h>

#define VTUI_BOOL_T bool
#define VTUI_TRUE_T true
#define VTUI_FALSE_T false

#else
// no C99 support, fallback to char

#define VTUI_BOOL_T char
#define VTUI_TRUE (1)
#define VTUI_FALSE (0)

#endif
#endif

// ## VTUI BYTE_T TYPE ##

#ifndef VTUI_BYTE_T

#define VTUI_BYTE_T unsigned char

#endif

// ## VTUI NULL MACRO ##

#ifndef VTUI_NULL

// NULL exists
#ifdef NULL

// local null macro
#define VTUI_NULL NULL

#else
// null not present, panic

// local null macro
#define VTUI_NULL ((void *)0)

#endif
#endif

// ## VTUI HELPER MACROS ##

// min / max are nifty little things

#ifndef vtui_min

// does min exist?
#ifdef min

#define vtui_min(a, b) min(a, b)

#else
// min doesnt exist, use fallback impl

#define vtui_min(a, b) (a <= b ? a : b)

#endif
#endif

#ifndef vtui_max

// does max exist?
#ifdef max

#define vtui_max(a, b) max(a, b)

#else
// max doesnt exist, use fallback impl

#define vtui_max(a, b) (a >= b ? a : b)

#endif
#endif

#ifndef vtui_abs

#define vtui_abs(n) (n >= 0 ? n : -n)

#endif

#ifndef vtui_within

#define vtui_within(n, l, h) (n >= l && n <= h)

#endif

// ## VTUI ERROR CODES ##

#define VTUI_OK           0 // success!
#define VTUI_ENOMEM      -1 // could not allocate memory
#define VTUI_EIO         -2 // generic i/o error
#define VTUI_EWRONGSTATE -3 // the provided vtui is in the wrong state
#define VTUI_EFMT        -4 // an error occurred while formatting a string

#endif