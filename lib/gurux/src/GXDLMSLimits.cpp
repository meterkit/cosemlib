//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "../include/GXDLMSLimits.h"

CGXDLMSLimits::CGXDLMSLimits()
{
    m_MaxInfoTX = DEFAULT_MAX_INFO_TX;
    m_MaxInfoRX = DEFAULT_MAX_INFO_RX;
    m_WindowSizeTX = DEFAULT_WINDOWS_SIZE_TX;
    m_WindowSizeRX = DEFAULT_WINDOWS_SIZE_RX;
}

CGXDLMSVariant CGXDLMSLimits::GetMaxInfoTX()
{
    return m_MaxInfoTX;
}

void CGXDLMSLimits::SetMaxInfoTX(CGXDLMSVariant value)
{
    m_MaxInfoTX = value;
}

CGXDLMSVariant CGXDLMSLimits::GetMaxInfoRX()
{
    return m_MaxInfoRX;
}

void CGXDLMSLimits::SetMaxInfoRX(CGXDLMSVariant value)
{
    m_MaxInfoRX = value;
}

CGXDLMSVariant CGXDLMSLimits::GetWindowSizeTX()
{
    return m_WindowSizeRX;
}

void CGXDLMSLimits::SetWindowSizeTX(CGXDLMSVariant value)
{
    m_WindowSizeRX = value;
}

CGXDLMSVariant CGXDLMSLimits::GetWindowSizeRX()
{
    return m_WindowSizeTX;
}

void CGXDLMSLimits::SetWindowSizeRX(CGXDLMSVariant value)
{
    m_WindowSizeTX = value;
}
