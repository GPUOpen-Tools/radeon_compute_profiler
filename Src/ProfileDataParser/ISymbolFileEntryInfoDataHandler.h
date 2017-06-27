//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Symbol file entry indo data handler Interface header
//==============================================================================

#ifndef _I_SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_H_
#define _I_SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_H_

// std
#include <string>

// common
#include <AMDTOSWrappers/Include/osOSDefinitions.h>

#ifdef WIN32
    #ifndef LineNum
        #define LineNum DWORD
    #endif
    #ifndef Address
        #define Address DWORD64
    #endif
#else
    #ifndef LineNum
        #define LineNum size_t
    #endif
    #ifndef Address
        #define Address void*
    #endif
#endif

/// Stack Entry Info Data Handler Interface
class IStackEntryInfoDataHandler
{
public:
    /// Get the Address
    /// \return address of the entry
    virtual Address GetAddress() const = 0;

    /// Get the displacement
    /// \return displacement of the entry
    virtual LineNum GetDisplacement() const = 0;

    /// Get the line number
    /// \return line number of the entry
    virtual LineNum GetLineNumber() const = 0;

    /// Get the address as char array
    /// \return stack address as char array
    virtual const char* GetAddressString() const = 0;

    /// Get the file name
    /// \return file name of the symbol entry
    virtual const char* GetFileNameString() const = 0;

    /// Get the module name
    /// \return module name as char array
    virtual const char* GetModuleNameString() const = 0;

    /// Get the Symbol Name as char array
    /// \return symbol name as char array
    virtual const char* GetSymbolNameString() const = 0;

    /// Virtual Destructor
    virtual ~IStackEntryInfoDataHandler() = default;
};


/// Symbol file Entry Info Data Handler Interface
class ISymbolFileEntryInfoDataHandler
{
public:

    /// Get the stack entry info handler
    /// \return stack entry info handler interface pointer
    virtual IStackEntryInfoDataHandler* GetStackEntryInfoHandler() = 0;

    /// Get the module name
    /// \return module name as char array
    virtual const char* GetSymbolModuleName() const = 0;

    /// Get the thread Id of the symbol
    /// \return thread id of the symbol
    virtual osThreadId GetsymbolThreadId() const = 0;

    /// Get the api name of the symbol
    /// \return api namme char array
    virtual const char* GetSymbolApiName() const = 0;

    /// Checks the stack entry is availbale or not
    /// \return flag indicating availability of the stack entry with the symbol or not
    virtual bool IsStackEntryNull() const = 0;

    /// virtual Destructor
    virtual ~ISymbolFileEntryInfoDataHandler() = default;
};

#endif // _I_SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_H_
