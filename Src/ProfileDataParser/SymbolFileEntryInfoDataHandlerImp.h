//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief symbol file entry info data handler Implementation Header
//==============================================================================

#ifndef _SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_IMP_H_
#define _SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_IMP_H_

// profiler common
#include <StackTraceAtpFile.h>

#include "ISymbolFileEntryInfoDataHandler.h"

/// Symbol file entry data info handler class
class SymbolFileEntryInfoDataHandler : public IStackEntryInfoDataHandler,
    public ISymbolFileEntryInfoDataHandler
{
public:

    ///Constructor
    /// \param[in] pSymbolFileEntry symbol file entry
    SymbolFileEntryInfoDataHandler(SymbolFileEntry* pSymbolFileEntry) : m_pSymbolFileEntry(pSymbolFileEntry)
    {
    }

    /// Get the stack entry info handler
    /// \return stack entry info handler interface pointer
    IStackEntryInfoDataHandler* GetStackEntryInfoHandler() override;

    /// Get the Address
    /// \return address of the entry
    Address GetAddress() const override;

    /// Get the displacement
    /// \return displacement of the entry
    LineNum GetDisplacement() const override;

    /// Get the line number
    /// \return line number of the entry
    LineNum GetLineNumber() const override;

    /// Get the address as a string
    /// \return stack address as string
    std::string GetAddressString() const override;

    /// Get the file name
    /// \return file name of the symbol entry
    std::string GetFileNameString() const override;

    /// Get the module name
    /// \return module name as string
    std::string GetModuleNameString() const override;

    /// Get the Symbol Name as String
    /// \return symbol name as string
    std::string GetSymbolNameString() const override;

    /// Get the module name
    /// \return module name as string
    std::string GetSymbolModuleName() const override;

    /// Get the thread Id of the symbol
    /// \return thread id of the symbol
    osThreadId GetsymbolThreadId() const override;

    /// Get the api name of the symbol
    /// \return api namme string
    std::string GetSymbolApiName() const override;

    /// Checks the stack entry is availbale or not
    /// \return flag indicating availability of the stack entry with the symbol or not
    bool IsStackEntryNull() const override;

    /// Destructor
    ~SymbolFileEntryInfoDataHandler() = default;

private:
    SymbolFileEntry* m_pSymbolFileEntry;             ///< pointer to symbol file entry data
};

#endif // _SYMBOL_FILE_ENTRY_INFO_DATA_HANDLER_IMP_H_
