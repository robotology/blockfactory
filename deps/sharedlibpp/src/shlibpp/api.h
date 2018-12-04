/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_API_H
#define SHAREDLIBPP_API_H

#if defined _WIN32 || defined __CYGWIN__
#  define SHLIBPP_HELPER_DLL_EXPORT __declspec(dllexport)
#  define SHLIBPP_HELPER_DLL_IMPORT __declspec(dllimport)
#  define SHLIBPP_HELPER_DLL_LOCAL
#else
#  if __GNUC__ >= 4
#    define SHLIBPP_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
#    define SHLIBPP_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
#    define SHLIBPP_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
#  else
#     define SHLIBPP_HELPER_DLL_EXPORT
#     define SHLIBPP_HELPER_DLL_IMPORT
#     define SHLIBPP_HELPER_DLL_LOCAL
#  endif
#endif

#define SHLIBPP_EXPORT SHLIBPP_HELPER_DLL_EXPORT
#define SHLIBPP_IMPORT SHLIBPP_HELPER_DLL_IMPORT
#define SHLIBPP_LOCAL SHLIBPP_HELPER_DLL_LOCAL

#ifdef SHLIBPP_STATIC
#  define SHLIBPP_API
#  define SHLIBPP_HIDDEN
#else
#  ifdef BUILDING_SHLIBPP
#    define SHLIBPP_API SHLIBPP_HELPER_DLL_EXPORT
#  else
#    define SHLIBPP_API SHLIBPP_HELPER_DLL_IMPORT
#  endif
#  define SHLIBPP_HIDDEN SHLIBPP_HELPER_DLL_LOCAL
#endif

#endif // SHAREDLIBPP_API_H
