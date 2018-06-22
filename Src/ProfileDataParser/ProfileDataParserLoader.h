//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Profile data parser library loader utility
//==============================================================================


#ifndef _PROFILE_DATA_PARSER_LOADER_H_
#define _PROFILE_DATA_PARSER_LOADER_H_

#include "ATPParserInterface.h"
#include <algorithm>

#ifdef _WIN32
    typedef HMODULE LibHandle;
    #define RCPDP_MAX_PATH MAX_PATH

    #ifdef _WIN64
        #define RCPDP_IS64BIT
    #else
        #define RCPDP_IS32BIT
    #endif

    #ifdef UNICODE
        typedef WCHAR LocaleChar;
        typedef std::wstring LocaleString;
        #define RCPDP_LIB_NAME L"RCPProfileDataParser"
        #define RCPDP_LIB_PREFIX L""
        #define RCPDP_X86_64_ARCH_SUFFIX L"-x64"
        #define RCPDP_X86_ARCH_SUFFIX L""
        #define RCPDP_LIB_SUFFIX L".dll"
    #else
        typedef CHAR LocaleChar;
        typedef std::string LocaleString;
        #define RCPDP_LIB_NAME "RCPProfileDataParser"
        #define RCPDP_LIB_PREFIX ""
        #define RCPDP_X86_64_ARCH_SUFFIX "-x64"
        #define RCPDP_X86_ARCH_SUFFIX L""
        #define RCPDP_LIB_SUFFIX ".dll"
    #endif
#else
    #include <dlfcn.h>
    #include <unistd.h>
    typedef void* LibHandle;
    typedef char LocaleChar;
    #define RCPDP_MAX_PATH 4096

    #if defined __x86_64__
        #define RCPDP_IS64BIT
    #else
        #define RCPDP_IS32BIT
    #endif

    #ifdef UNICODE
        typedef std::wstring LocaleString;
        #define RCPDP_LIB_NAME L"RCPProfileDataParser"
        #define RCPDP_LIB_PREFIX L"lib"
        #define RCPDP_X86_64_ARCH_SUFFIX L""
        #define RCPDP_X86_ARCH_SUFFIX L"32"
        #define RCPDP_LIB_SUFFIX L".so"
    #else
        typedef std::string LocaleString;
        #define RCPDP_LIB_NAME "RCPProfileDataParser"
        #define RCPDP_LIB_PREFIX "lib"
        #define RCPDP_X86_64_ARCH_SUFFIX ""
        #define RCPDP_X86_ARCH_SUFFIX "32"
        #define RCPDP_LIB_SUFFIX ".so"
    #endif
#endif


typedef decltype(ParseAtpFile)* AtpParserFunc;
typedef decltype(GetAtpDataHandlerInterface)* AtpDataHandlerFunc;
typedef decltype(GetParserUUID)* AtpParserGetUUIDFunc;

inline LocaleString GetWorkingDirectoryPath()
{
    LocaleChar selfModuleName[RCPDP_MAX_PATH];
    LocaleString path;

#ifdef _WIN32
    GetModuleFileName(nullptr, selfModuleName, RCPDP_MAX_PATH);
    path = LocaleString(selfModuleName);
    std::replace(path.begin(), path.end(), '\\', '/');
#else
    int len;
    len = readlink("/proc/self/exe", selfModuleName, RCPDP_MAX_PATH - 1);

    if (len != -1)
    {
        selfModuleName[len] = '\0';
    }

    path = LocaleString(selfModuleName);
#endif

    size_t lastSlashPos = path.find_last_of('/');

    if (std::string::npos != lastSlashPos)
    {
        path = path.substr(0, lastSlashPos + 1);
    }

    return path;
}

/// ProfileDataParser Library loader utility class
class ProfileDataParserLoader
{
public:

    /// Get the static instance of the loader
    /// \return static instance of the library loader
    static ProfileDataParserLoader* Instance()
    {
        if (nullptr == m_pParserInterfaceLoader)
        {
            m_pParserInterfaceLoader = new(std::nothrow) ProfileDataParserLoader();
        }

        return m_pParserInterfaceLoader;
    }

    /// Get the status of the parser library
    /// \return true if library is loaded otherwise false
    bool IsParserLoaded() const
    {
        return m_isLoaded;
    }

    /// Loads the profiledataparser library
    /// \param[out] errorString contains error string if loading of the library fails
    /// \return true if loading of the library is successful otherwise false
    bool LoadParser(std::string& errorString)
    {
        bool success = false;

        if (!m_isLoaded)
        {

            LibHandle parserLibraryHandle;
            LocaleString parserModuleName = RCPDP_LIB_PREFIX RCPDP_LIB_NAME;

#ifdef RCPDP_IS64BIT
            parserModuleName += RCPDP_X86_64_ARCH_SUFFIX;
#else
            parserModuleName += RCPDP_X86_ARCH_SUFFIX;
#endif

            parserModuleName += RCPDP_LIB_SUFFIX;
            LocaleString path = GetWorkingDirectoryPath();

            parserModuleName = path + parserModuleName;

#ifdef _WIN32
            parserLibraryHandle = LoadLibrary(parserModuleName.c_str());

            if (nullptr != parserLibraryHandle)
            {
                m_pAtpDataHandlerFunc = reinterpret_cast<AtpDataHandlerFunc>(GetProcAddress(parserLibraryHandle, GET_ATP_DATA_HANDLER_FUNC_NAME));
                m_pAtpParserFunc = reinterpret_cast<AtpParserFunc>(GetProcAddress(parserLibraryHandle, PARSE_ATP_FILE_FUNC_NAME));
                m_pAtpParserGetUUIDFunc = reinterpret_cast<AtpParserGetUUIDFunc>(GetProcAddress(parserLibraryHandle, GET_PARSER_UUID_FUNC_NAME));
            }

#else
            parserLibraryHandle = dlopen(parserModuleName.c_str(), RTLD_NOW);

            if (nullptr != parserLibraryHandle)
            {
                m_pAtpDataHandlerFunc = reinterpret_cast<AtpDataHandlerFunc>(dlsym(parserLibraryHandle, GET_ATP_DATA_HANDLER_FUNC_NAME));
                m_pAtpParserFunc = reinterpret_cast<AtpParserFunc>(dlsym(parserLibraryHandle, PARSE_ATP_FILE_FUNC_NAME));
                m_pAtpParserGetUUIDFunc = reinterpret_cast<AtpParserGetUUIDFunc>(dlsym(parserLibraryHandle, GET_PARSER_UUID_FUNC_NAME));
            }

#endif

            if (nullptr != m_pAtpParserFunc && nullptr != m_pAtpDataHandlerFunc && nullptr != m_pAtpParserGetUUIDFunc)
            {
                if (m_pAtpParserGetUUIDFunc() == RCP_PROFILE_DATA_PARSER_UUID_VAL)
                {
                    success = true;
                    m_isLoaded = true;
                }
                else
                {
                    errorString = "Version mismatch in the parser library and the loader";
                }
            }
            else
            {
                errorString = "Unable to initialize one of the library functions";
            }
        }
        else
        {
            success = true;
        }

        return success;
    }

    /// Get the AtpParserFunctionPointer
    /// \return function pointer of the AtpParserFunc from the library
    AtpParserFunc GetAtpParserFunctionPointer() const
    {
        return m_pAtpParserFunc;
    }

    /// Get the AtpDataHandlerFunctionPointer
    /// \return function pointer of the AtpDataHandlerFunc from the library
    AtpDataHandlerFunc GetAtpDataHandlerFunc() const
    {
        return m_pAtpDataHandlerFunc;
    }

    /// Get the AtpUUIDFunctionPointer
    /// \return function pointer of the AtpParserGetUUIDFunc from the library
    AtpParserGetUUIDFunc GetAtpUUIDFunc() const
    {
        return m_pAtpParserGetUUIDFunc;
    }

private:

    /// private constructor
    ProfileDataParserLoader(): m_isLoaded(false),
        m_pAtpParserFunc(nullptr),
        m_pAtpDataHandlerFunc(nullptr),
        m_pAtpParserGetUUIDFunc(nullptr)
    {}

    static ProfileDataParserLoader* m_pParserInterfaceLoader; ///< static instance of the profile data parser loader class
    bool                            m_isLoaded;               ///< flag indicating library is loaded or not
    AtpParserFunc                   m_pAtpParserFunc;         ///< ATP parser function pointer
    AtpDataHandlerFunc              m_pAtpDataHandlerFunc;    ///< ATP parsed data handler function pointer
    AtpParserGetUUIDFunc            m_pAtpParserGetUUIDFunc;  ///< Profile data parser GetUUID function pointer
};

// Note: For usage - Add Initialization of the static instance in compiling unit
// ProfileDataParserLoader* ProfileDataParserLoader::m_pParserInterfaceLoader = nullptr;

#endif // _PROFILE_DATA_PARSER_LOADER_H_
