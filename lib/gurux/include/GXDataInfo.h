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

#pragma once

//This class is used in DLMS data parsing.
class CGXDataInfo
{
private:
    // Last array index.
    int m_Index;

    // Items count in array.
    int m_Count;
    // Object data type.
    DLMS_DATA_TYPE m_Type;
    // Is data parsed to the end.
    bool m_Compleate;

public:
    //Constructor.
    CGXDataInfo()
    {
        Clear();
    }

    // Get Last array index.
    int GetIndex()
    {
        return m_Index;
    }

    //Set Last array index.
    void SetIndex(int value)
    {
        m_Index = value;
    }

    // Get items count in array.
    int GetCount()
    {
        return m_Count;
    }

    // Set Items count in array.
    void SetCount(int value)
    {
        m_Count = value;
    }

    //Get Object data type.
    DLMS_DATA_TYPE GetType()
    {
        return m_Type;
    }

    // Set Object data type.
    void SetType(DLMS_DATA_TYPE value)
    {
        m_Type = value;
    }

    // Is all data parsed.
    bool IsCompleate()
    {
        return m_Compleate;
    }

    //Is all data parsed.
    void SetCompleate(bool value)
    {
        m_Compleate = value;
    }

    //Clear settings.
    void Clear()
    {
        m_Index = 0;
        m_Count = 0;
        m_Type = DLMS_DATA_TYPE_NONE;
        m_Compleate = true;
    }
};