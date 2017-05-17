//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief symbol file entry info data handler Implementation
//==============================================================================

#include "SymbolFileEntryInfoDataHandlerImp.h"

IStackEntryInfoDataHandler* SymbolFileEntryInfoDataHandler::GetStackEntryInfoHandler()
{
    if (IsStackEntryNull())
    {
        return nullptr;
    }

    return dynamic_cast<IStackEntryInfoDataHandler*>(this);
}

Address SymbolFileEntryInfoDataHandler::GetAddress() const
{
    Address address = NULL;

    if (nullptr != m_pSymbolFileEntry && !IsStackEntryNull())
    {
        address = m_pSymbolFileEntry->m_pStackEntry->m_dwAddress;
    }

    return address;
}

LineNum SymbolFileEntryInfoDataHandler::GetDisplacement() const
{
    LineNum lineNum = NULL;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        lineNum = m_pSymbolFileEntry->m_pStackEntry->m_dwDisplacement;
    }

    return lineNum;
}

LineNum SymbolFileEntryInfoDataHandler::GetLineNumber() const
{
    LineNum lineNum = NULL;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        lineNum = m_pSymbolFileEntry->m_pStackEntry->m_dwLineNum;
    }

    return lineNum;
}

std::string SymbolFileEntryInfoDataHandler::GetAddressString() const
{
    std::string addressString;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        addressString = m_pSymbolFileEntry->m_pStackEntry->m_strSymAddr;
    }

    return addressString;
}

std::string SymbolFileEntryInfoDataHandler::GetFileNameString() const
{
    std::string fileNameString;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        fileNameString = m_pSymbolFileEntry->m_pStackEntry->m_strFile;
    }

    return fileNameString;
}

std::string SymbolFileEntryInfoDataHandler::GetModuleNameString() const
{
    std::string moduleNameString;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        moduleNameString = m_pSymbolFileEntry->m_pStackEntry->m_strModName;
    }

    return moduleNameString;
}

std::string SymbolFileEntryInfoDataHandler::GetSymbolNameString() const
{
    std::string symbolNameString;

    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        symbolNameString = m_pSymbolFileEntry->m_pStackEntry->m_strSymName;
    }

    return symbolNameString;
}

std::string SymbolFileEntryInfoDataHandler::GetSymbolModuleName() const
{
    std::string symbolModuleName;

    if (nullptr != m_pSymbolFileEntry)
    {
        symbolModuleName = m_pSymbolFileEntry->m_strModName;
    }

    return symbolModuleName;
}

osThreadId SymbolFileEntryInfoDataHandler::GetsymbolThreadId() const
{
    osThreadId threadId = NULL;

    if (nullptr != m_pSymbolFileEntry)
    {
        threadId = m_pSymbolFileEntry->m_tid;
    }

    return threadId;
}

std::string SymbolFileEntryInfoDataHandler::GetSymbolApiName() const
{
    std::string symbolApiName;

    if (nullptr != m_pSymbolFileEntry)
    {
        symbolApiName = m_pSymbolFileEntry->m_strAPIName;
    }

    return symbolApiName;
}

bool SymbolFileEntryInfoDataHandler::IsStackEntryNull() const
{
    if (nullptr != m_pSymbolFileEntry  && nullptr != m_pSymbolFileEntry->m_pStackEntry)
    {
        return false;
    }

    return true;
}
