//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file defines the structure used to store platform info.
//==============================================================================

#include "CLPlatformInfo.h"

namespace CLPlatformInfo
{
bool CLPlatformInfoCompare::operator()(const CLPlatformInfo::PlatformInfo pi1, const CLPlatformInfo::PlatformInfo pi2) const
{
    if (pi1.m_deviceName < pi2.m_deviceName)
    {
        return true;
    }
    else
    {
        if (pi1.m_deviceName > pi2.m_deviceName)
        {
            return false;
        }
        else
        {
            if (pi1.m_platformName < pi2.m_platformName)
            {
                return true;
            }
            else
            {
                if (pi1.m_platformName > pi2.m_platformName)
                {
                    return false;
                }
                else  //case that pi1.m_platformName == pi2.m_platformName
                {
                    if (pi1.m_driverVersion < pi2.m_driverVersion)
                    {
                        return true;
                    }
                    else //driver ver pi1 >= driver ver. pi2
                    {
                        if (pi1.m_driverVersion > pi2.m_driverVersion)
                        {
                            return false;
                        }
                        else //pi1.m_driverVersion == pi2.m_driverVersion
                        {
                            if (pi1.m_addressBits < pi2.m_addressBits)
                            {
                                return true;
                            }
                            else
                            {
                                if (pi1.m_addressBits > pi2.m_addressBits)
                                {
                                    return false;
                                }
                                else
                                {
                                    if (pi1.m_runtimeVersion < pi2.m_runtimeVersion)
                                    {
                                        return true;
                                    }
                                    else
                                    {
                                        if (pi1.m_runtimeVersion > pi2.m_runtimeVersion)
                                        {
                                            return false;
                                        }
                                        else
                                        {
                                            if (pi1.m_boardName < pi2.m_boardName)
                                            {
                                                return true;
                                            }
                                            else
                                            {
                                                if (pi1.m_boardName > pi2.m_boardName)
                                                {
                                                    return false;
                                                }
                                                else
                                                {
                                                    if (pi1.m_pcieDeviceId < pi2.m_pcieDeviceId)
                                                    {
                                                        return true;
                                                    }
                                                    else
                                                    {
                                                        return false;
                                                    }
                                                }
                                            }
                                        }
                                    } // end if - pi1.m_runtimeVersion
                                }
                            } //end if - pi1.m_addressBits
                        }
                    }//end if - pi1.m_driverVersion
                }
            }// end if - pi1.m_platformName
        }
    }// end if - pi1.m_deviceName
}

}
