//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#pragma once

// Windows.h is a bit tricky file to include, due to various reasons.
// Instead of including Windows.h directly, one should use this file, which
// removes a few of the annoying problems with it.

#ifdef WIN32

// Already included Windows.h by some other means. This is an error, since including winsock2.h will fail after this.
#if defined(_WINDOWS_) && !defined(_WINSOCK2API_) && defined(FD_CLR)
#error Error: Trying to include winsock2.h after windows.h! This is not allowed! See this file for fix instructions.
#endif

// Remove the manually added #define if it exists so that winsock2.h includes OK.
#if !defined(_WINSOCK2API_) && defined(_WINSOCKAPI_)
#undef _WINSOCKAPI_
#endif

// Windows.h issue: Cannot include winsock2.h after windows.h, so include it before.

// MathGeoLib uses the symbol Polygon. Windows.h gives GDI function Polygon, which Tundra will never use, so kill it.
#define Polygon Polygon_unused

// Prevent Windows includes trying to include consoleapi.h, which will conflict with our ConsoleAPI.h file
#define NOAPISET

#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef NOGDI
#define NOGDI
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#undef Polygon

// Remove <Windows.h> PlaySound Unicode defines. Tundra does not use winmm PlaySound API anywhere (but OpenAL).
#ifdef PlaySound
#undef PlaySound
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

// Fix JSONValue::GetObjectW
// unresolved external symbol
#ifdef GetObject
#undef GetObject
#endif

#endif
