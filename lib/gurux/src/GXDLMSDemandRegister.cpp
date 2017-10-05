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

#include "../include/GXDLMSVariant.h"
#include "../include/GXDLMSClient.h"
#include "../include/GXDLMSConverter.h"
#include "../include/GXDLMSDemandRegister.h"

bool CGXDLMSDemandRegister::IsRead(int index)
{
    if (index == 4)
    {
        return m_Unit != 0;
    }
    return CGXDLMSObject::IsRead(index);
}

/**
 Constructor.
*/
CGXDLMSDemandRegister::CGXDLMSDemandRegister() : CGXDLMSObject(DLMS_OBJECT_TYPE_DEMAND_REGISTER)
{
    m_Period = m_NumberOfPeriods = m_Unit = m_Scaler = 0;
}

/**
 Constructor.

 @param ln Logical Name of the object.
*/
CGXDLMSDemandRegister::CGXDLMSDemandRegister(std::string ln) : CGXDLMSObject(DLMS_OBJECT_TYPE_DEMAND_REGISTER, ln)
{
    m_Period = m_NumberOfPeriods = m_Unit = m_Scaler = 0;
}

/**
 Constructor.

 @param ln Logical Name of the object.
 @param sn Short Name of the object.
*/
CGXDLMSDemandRegister::CGXDLMSDemandRegister(int sn) : CGXDLMSObject(DLMS_OBJECT_TYPE_DEMAND_REGISTER, sn)
{
    m_Period = m_NumberOfPeriods = m_Unit = m_Scaler = 0;
}

/**
 Current avarage value of COSEM Data object.
*/
CGXDLMSVariant CGXDLMSDemandRegister::GetCurrentAvarageValue()
{
    return m_CurrentAvarageValue;
}
void CGXDLMSDemandRegister::SetCurrentAvarageValue(CGXDLMSVariant value)
{
    m_CurrentAvarageValue = value;
}

/**
 Last avarage value of COSEM Data object.
*/
CGXDLMSVariant CGXDLMSDemandRegister::GetLastAvarageValue()
{
    return m_LastAvarageValue;
}
void CGXDLMSDemandRegister::SetLastAvarageValue(CGXDLMSVariant value)
{
    m_LastAvarageValue = value;
}

/**
 Scaler of COSEM Register object.
*/
double CGXDLMSDemandRegister::GetScaler()
{
    return pow((float)10, m_Scaler);
}

void CGXDLMSDemandRegister::SetScaler(double value)
{
    m_Scaler = (signed char)log10(value);
}

// Unit of COSEM Register object.
int CGXDLMSDemandRegister::GetUnit()
{
    return m_Unit;
}

void CGXDLMSDemandRegister::SetUnit(unsigned char value)
{
    m_Unit = value;
}

/**
 Scaler of COSEM Register object.
*/
CGXDLMSVariant CGXDLMSDemandRegister::GetStatus()
{
    return m_Status;
}
void CGXDLMSDemandRegister::SetStatus(CGXDLMSVariant value)
{
    m_Status = value;
}

/**
 Capture time of COSEM Register object.
*/
CGXDateTime CGXDLMSDemandRegister::GetCaptureTime()
{
    return m_CaptureTime;
}
void CGXDLMSDemandRegister::SetCaptureTime(CGXDateTime value)
{
    m_CaptureTime = value;
}

/**
 Current start time of COSEM Register object.
*/
CGXDateTime CGXDLMSDemandRegister::GetStartTimeCurrent()
{
    return m_StartTimeCurrent;
}
void CGXDLMSDemandRegister::SetStartTimeCurrent(CGXDateTime value)
{
    m_StartTimeCurrent = value;
}

unsigned long CGXDLMSDemandRegister::GetPeriod()
{
    return m_Period;
}

void CGXDLMSDemandRegister::SetPeriod(unsigned long value)
{
    m_Period = value;
}

int CGXDLMSDemandRegister::GetNumberOfPeriods()
{
    return m_NumberOfPeriods;
}

void CGXDLMSDemandRegister::SetNumberOfPeriods(int value)
{
    m_NumberOfPeriods = value;
}

/*
 * Reset e.GetValue().
 */
void CGXDLMSDemandRegister::Reset()
{

}

/*
 * Next period.
 */
void CGXDLMSDemandRegister::NextPeriod()
{

}

void CGXDLMSDemandRegister::GetValues(std::vector<std::string>& values)
{
    values.clear();
    std::string ln;
    GetLogicalName(ln);
    values.push_back(ln);
    values.push_back(m_CurrentAvarageValue.ToString());
    values.push_back(m_LastAvarageValue.ToString());
    std::string str = "Scaler: ";
    //if there is no fractal part.
    double s = GetScaler();
    if (s - (long)s == 0)
    {
        str += CGXDLMSVariant((long)s).ToString();
    }
    else
    {
        str += CGXDLMSVariant(s).ToString();
    }
    str += " Unit: ";
    str += CGXDLMSConverter::GetUnitAsString(m_Unit);
    values.push_back(str);
    values.push_back(m_Status.ToString());
    values.push_back(m_CaptureTime.ToString());
    values.push_back(m_StartTimeCurrent.ToString());
    values.push_back(CGXDLMSVariant(m_Period).ToString());
    values.push_back(CGXDLMSVariant(m_NumberOfPeriods).ToString());
}

void CGXDLMSDemandRegister::GetAttributeIndexToRead(std::vector<int>& attributes)
{
    //LN is static and read only once.
    if (CGXDLMSObject::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
    //ScalerUnit
    if (!IsRead(4))
    {
        attributes.push_back(4);
    }
    //CurrentAvarageValue
    if (CanRead(2))
    {
        attributes.push_back(2);
    }
    //LastAvarageValue
    if (CanRead(3))
    {
        attributes.push_back(3);
    }
    //Status
    if (CanRead(5))
    {
        attributes.push_back(5);
    }
    //CaptureTime
    if (CanRead(6))
    {
        attributes.push_back(6);
    }
    //StartTimeCurrent
    if (CanRead(7))
    {
        attributes.push_back(7);
    }
    //Period
    if (CanRead(8))
    {
        attributes.push_back(8);
    }
    //NumberOfPeriods
    if (CanRead(9))
    {
        attributes.push_back(9);
    }
}

// Returns amount of attributes.
int CGXDLMSDemandRegister::GetAttributeCount()
{
    return 9;
}

// Returns amount of methods.
int CGXDLMSDemandRegister::GetMethodCount()
{
    return 2;
}

int CGXDLMSDemandRegister::Invoke(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        Reset();
        return DLMS_ERROR_CODE_OK;
    }
    e.SetError(DLMS_ERROR_CODE_READ_WRITE_DENIED);
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMSDemandRegister::GetDataType(int index, DLMS_DATA_TYPE& type)
{
    if (index == 1)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 2)
    {
        type = m_CurrentAvarageValue.vt;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 3)
    {
        type = m_LastAvarageValue.vt;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 4)
    {
        type = DLMS_DATA_TYPE_STRUCTURE;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 5)
    {
        type = m_Status.vt;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 6)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 7)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 8)
    {
        type = DLMS_DATA_TYPE_UINT32;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 9)
    {
        type = DLMS_DATA_TYPE_UINT16;
        return DLMS_ERROR_CODE_OK;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}

int CGXDLMSDemandRegister::GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        int ret;
        CGXDLMSVariant tmp;
        if ((ret = GetLogicalName(this, tmp)) != 0)
        {
            return ret;
        }
        e.SetValue(tmp);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 2)
    {
        e.SetValue(m_CurrentAvarageValue);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 3)
    {
        e.SetValue(m_LastAvarageValue);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 4)
    {
        e.GetValue().Clear();
        e.GetValue().vt = DLMS_DATA_TYPE_STRUCTURE;
        e.GetValue().Arr.push_back(m_Scaler);
        e.GetValue().Arr.push_back(m_Unit);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 5)
    {
        e.SetValue(m_Status);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 6)
    {
        e.SetValue(m_CaptureTime);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 7)
    {
        e.SetValue(m_StartTimeCurrent);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 8)
    {
        e.SetValue(m_Period);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 9)
    {
        e.SetValue(m_NumberOfPeriods);
        return DLMS_ERROR_CODE_OK;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}

int CGXDLMSDemandRegister::SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        return SetLogicalName(this, e.GetValue());
    }
    else if (e.GetIndex() == 2)
    {
        if (m_Scaler != 0)
        {
            SetCurrentAvarageValue(CGXDLMSVariant(e.GetValue().ToDouble() * m_Scaler));
        }
        else
        {
            SetCurrentAvarageValue(e.GetValue());
        }
    }
    else if (e.GetIndex() == 3)
    {
        if (m_Scaler != 0)
        {
            SetLastAvarageValue(CGXDLMSVariant(e.GetValue().ToDouble() * GetScaler()));
        }
        else
        {
            SetLastAvarageValue(e.GetValue());
        }
    }
    else if (e.GetIndex() == 4 && e.GetValue().vt == DLMS_DATA_TYPE_STRUCTURE)
    {
        m_Scaler = e.GetValue().Arr[0].bVal;
        m_Unit = e.GetValue().Arr[1].bVal;
    }
    else if (e.GetIndex() == 5)
    {
        SetStatus(e.GetValue().lVal);
    }
    else if (e.GetIndex() == 6)
    {
        CGXDLMSVariant tmp;
        CGXDLMSClient::ChangeType(e.GetValue(), DLMS_DATA_TYPE_DATETIME, tmp);
        SetCaptureTime(tmp.dateTime);
    }
    else if (e.GetIndex() == 7)
    {
        CGXDLMSVariant tmp;
        CGXDLMSClient::ChangeType(e.GetValue(), DLMS_DATA_TYPE_DATETIME, tmp);
        SetStartTimeCurrent(tmp.dateTime);
    }
    else if (e.GetIndex() == 8)
    {
        SetPeriod(e.GetValue().ulVal);
    }
    else if (e.GetIndex() == 9)
    {
        SetNumberOfPeriods(e.GetValue().ToInteger());
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
