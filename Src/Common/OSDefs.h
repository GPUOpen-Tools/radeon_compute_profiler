//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file provides definitions of wrappers for OS-specific functionality
//==============================================================================

#ifndef _OS_DEFS_H_
#define _OS_DEFS_H_

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
#else
    #include <sys/types.h>
    #include <unistd.h>
#endif


#ifdef _WIN32
    #define THREADHANDLE HANDLE
    #define TIMERID UINT_PTR
    #define PROCESSID PROCESS_INFORMATION
    #define LIB_HANDLE     HMODULE
    #define ENVSYSBLOCK const TCHAR*
#elif defined (_LINUX) || defined (LINUX)
    #define THREADHANDLE pthread_t
    #define TIMERID int
    #define PROCESSID pid_t
    #define LIB_HANDLE     void *
    #define ENVSYSBLOCK const char*
#endif

#ifdef WIN32
    #define SP_strcpy(des,size,src) strcpy_s(des, size, src)
    #define SP_wstrcpy(des,size,src) wcscpy_s(des, size, src)
    #define SP_strcat(des,size,src) strcat_s(des, size, src)
    #define SP_wstrcat(des,size,src) wcscat_s(des, size, src)
    #define SP_snprintf(buf, nLen, format, ...) _snprintf_s(buf, nLen, _TRUNCATE, format, ##__VA_ARGS__)
    #define SP_snwprintf(buf, nLen, format, ...) _snwprintf_s(buf, nLen, _TRUNCATE, format, ##__VA_ARGS__)
    #define SP_vsnwprintf(buf, nLen, format, ...) _vsnwprintf_s(buf, nLen, _TRUNCATE, format, ##__VA_ARGS__)
    #define SP_vsnprintf(buf, nLen, format, ...) vsnprintf_s(buf, nLen, _TRUNCATE, format, ##__VA_ARGS__)
    #define SP_sprintf(buf, nLen, format, ...) sprintf_s(buf, nLen, format, ##__VA_ARGS__)
    //#define SP_swprintf() swprintf_s
    #define SP_max(x,y) max(x,y)
    #define SP_getcwd(buf, len) _getcwd(buf, len)
#else
    #define SP_strcpy(des,size,src) strcpy(des, src)
    #define SP_wstrcpy(des,size,src) wcscpy(des, src)
    #define SP_strcat(des,size,src) strcat(des, src)
    #define SP_wstrcat(des,size,src) wcscat(des, src)
    #define SP_snprintf(buf, nLen, format, ...) snprintf(buf, nLen, format, ##__VA_ARGS__)
    #define SP_vsnprintf(buf, nLen, format, ...) vsnprintf(buf, nLen, format, ##__VA_ARGS__)
    #define SP_sprintf(buf, nLen, format, ...) sprintf(buf, format, ##__VA_ARGS__)
    #define SP_max(x,y) std::max(x,y)
    #define SP_getcwd(buf, len) getcwd(buf, len)
#endif

// These defs are needed since in linux using ofstream is the better behaviour: wofstream << wchar_t is not working correctly if you mix char & wchar_t
// and utf8 writing works better and correctly with ofstream. In windows we use wofstreams in order to support in the future unicode file names

#ifdef _WIN32
    #define SP_outStream std::wostream
    #define SP_fileStream std::wofstream
    #define SP_stringStream std::wstringstream
#else
    #define SP_outStream std::ostream
    #define SP_fileStream std::ofstream
    #define SP_stringStream std::stringstream
#endif

#endif //_OS_DEFS_H_
