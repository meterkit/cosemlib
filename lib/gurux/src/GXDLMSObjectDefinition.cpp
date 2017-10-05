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

#include "../include/GXDLMSObjectDefinition.h"
#include <sstream>
#include "../include/GXDLMSObjectFactory.h"

CGXDLMSObjectDefinition::CGXDLMSObjectDefinition()
{

}

/*
 * Constructor
 */
CGXDLMSObjectDefinition::CGXDLMSObjectDefinition(DLMS_OBJECT_TYPE classId, std::string logicalName)
{
    m_ClassId = classId;
    m_LogicalName = logicalName;
}

std::string CGXDLMSObjectDefinition::ToString()
{
    std::stringstream sb;
    sb << CGXDLMSObjectFactory::ObjectTypeToString(m_ClassId).c_str();
    sb << " ";
    sb << m_LogicalName.c_str();
    return sb.str();
}

DLMS_OBJECT_TYPE CGXDLMSObjectDefinition::GetClassId()
{
    return m_ClassId;
}

void CGXDLMSObjectDefinition::SetClassId(DLMS_OBJECT_TYPE value)
{
    m_ClassId = value;
}

std::string CGXDLMSObjectDefinition::GetLogicalName()
{
    return m_LogicalName;
}
void CGXDLMSObjectDefinition::SetLogicalName(std::string value)
{
    m_LogicalName = value;
}
