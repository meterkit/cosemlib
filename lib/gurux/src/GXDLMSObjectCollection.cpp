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

#include "../include/GXDLMSObjectCollection.h"
#include <sstream>

CGXDLMSObjectCollection::~CGXDLMSObjectCollection()
{
}

CGXDLMSObject* CGXDLMSObjectCollection::FindByLN(DLMS_OBJECT_TYPE type, std::string& ln)
{
    unsigned char tmp[6];
    GXHelpers::SetLogicalName(ln.c_str(), tmp);
    for (CGXDLMSObjectCollection::iterator it = this->begin(); it != end(); ++it)
    {
        if ((type == DLMS_OBJECT_TYPE_ALL || (*it)->GetObjectType() == type))
        {
            if (memcmp(tmp, (*it)->m_LN, 6) == 0)
            {
                return *it;
            }
        }
    }
    return NULL;
}

CGXDLMSObject* CGXDLMSObjectCollection::FindByLN(DLMS_OBJECT_TYPE type, unsigned char ln[6])
{
    for (CGXDLMSObjectCollection::iterator it = this->begin(); it != end(); ++it)
    {
        if (type == DLMS_OBJECT_TYPE_ALL || (*it)->GetObjectType() == type)
        {
            if (memcmp(ln, (*it)->m_LN, 6) == 0)
            {
                return *it;
            }
        }
    }
    return NULL;
}

CGXDLMSObject* CGXDLMSObjectCollection::FindBySN(unsigned short sn)
{
    for (CGXDLMSObjectCollection::iterator it = begin(); it != end(); ++it)
    {
        if ((*it)->GetShortName() == sn)
        {
            return *it;
        }
    }
    return NULL;
}

void CGXDLMSObjectCollection::GetObjects(DLMS_OBJECT_TYPE type, CGXDLMSObjectCollection& items)
{
    for (CGXDLMSObjectCollection::iterator it = begin(); it != end(); ++it)
    {
        if ((*it)->GetObjectType() == type)
        {
            items.push_back(*it);
        }
    }
}

void CGXDLMSObjectCollection::push_back(CGXDLMSObject* item)
{
    std::vector<CGXDLMSObject*>::push_back(item);
}

void CGXDLMSObjectCollection::Free()
{
    for (CGXDLMSObjectCollection::iterator it = begin(); it != end(); ++it)
    {
        delete (*it);
    }
    std::vector<CGXDLMSObject*>::clear();
}

std::string CGXDLMSObjectCollection::ToString()
{
    std::stringstream sb;
    sb << '[';
    bool empty = true;
    for (CGXDLMSObjectCollection::iterator it = begin(); it != end(); ++it)
    {
        if (!empty)
        {
            sb << ", ";
        }
        empty = false;
        std::string str = (*it)->GetName().ToString();
        sb.write(str.c_str(), str.size());
    }
    sb << ']';
    return sb.str();
}
