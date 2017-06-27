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

const char* SymbolFileEntryInfoDataHandler::GetAddressString() const
{
    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        return m_pSymbolFileEntry->m_pStackEntry->m_strSymAddr.c_str();
    }

    return nullptr;
}

const char* SymbolFileEntryInfoDataHandler::GetFileNameString() const
{
    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        return m_pSymbolFileEntry->m_pStackEntry->m_strFile.c_str();
    }

    return nullptr;
}

const char* SymbolFileEntryInfoDataHandler::GetModuleNameString() const
{
    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        return m_pSymbolFileEntry->m_pStackEntry->m_strModName.c_str();
    }

    return nullptr;
}

const char* SymbolFileEntryInfoDataHandler::GetSymbolNameString() const
{
    if (nullptr != m_pSymbolFileEntry  && !IsStackEntryNull())
    {
        return m_pSymbolFileEntry->m_pStackEntry->m_strSymName.c_str();
    }

    return nullptr;
}

const char* SymbolFileEntryInfoDataHandler::GetSymbolModuleName() const
{
    if (nullptr != m_pSymbolFileEntry)
    {
        return m_pSymbolFileEntry->m_strModName.c_str();
    }

    return nullptr;
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

const char* SymbolFileEntryInfoDataHandler::GetSymbolApiName() const
{
    if (nullptr != m_pSymbolFileEntry)
    {
        return m_pSymbolFileEntry->m_strAPIName.c_str();
    }

    return nullptr;
}

bool SymbolFileEntryInfoDataHandler::IsStackEntryNull() const
{
    if (nullptr != m_pSymbolFileEntry  && nullptr != m_pSymbolFileEntry->m_pStackEntry)
    {
        return false;
    }

    return true;
}

