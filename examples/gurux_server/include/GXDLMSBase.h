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

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock.h> //Add support for sockets	
#endif

#include "../../development/include/GXDLMSSecureServer.h"
extern char DATAFILE[FILENAME_MAX];

class CGXDLMSBase : public CGXDLMSSecureServer
{
private:
#if defined(_WIN32) || defined(_WIN64)//If Windows 
    SOCKET m_ServerSocket;
    HANDLE m_ReceiverThread;
#else //If Linux.
    int m_ServerSocket;
    pthread_t m_ReceiverThread;
#endif

public:

    /////////////////////////////////////////////////////////////////////////
    //Constructor.
    /////////////////////////////////////////////////////////////////////////
    CGXDLMSBase(
        bool UseLogicalNameReferencing = true,
        DLMS_INTERFACE_TYPE IntefaceType = DLMS_INTERFACE_TYPE_HDLC) :
        CGXDLMSSecureServer(UseLogicalNameReferencing, IntefaceType)
    {
#if defined(_WIN32) || defined(_WIN64)//If Windows 
        m_ReceiverThread = INVALID_HANDLE_VALUE;
        m_ServerSocket = INVALID_SOCKET;
#else //If Linux.
        m_ServerSocket = -1;
        m_ReceiverThread = -1;
#endif
        SetMaxReceivePDUSize(1024);
    }


    /////////////////////////////////////////////////////////////////////////
    //Destructor.
    /////////////////////////////////////////////////////////////////////////
    ~CGXDLMSBase(void)
    {
        StopServer();
    }

    bool IsConnected();

    int GetSocket();

    int StartServer(int port);

    int StopServer();

    int Init(int port);

    CGXDLMSObject* FindObject(
        DLMS_OBJECT_TYPE objectType,
        int sn,
        std::string& ln);

    void PreRead(
        std::vector<CGXDLMSValueEventArg*>& args);

    void PreWrite(
        std::vector<CGXDLMSValueEventArg*>& args);

    void PreAction(
        std::vector<CGXDLMSValueEventArg*>& args);

    void PostRead(
        std::vector<CGXDLMSValueEventArg*>& args);

    void PostWrite(
        std::vector<CGXDLMSValueEventArg*>& args);

    void PostAction(
        std::vector<CGXDLMSValueEventArg*>& args);

    bool IsTarget(
        unsigned long int serverAddress,
        unsigned long clientAddress);

    DLMS_SOURCE_DIAGNOSTIC ValidateAuthentication(
        DLMS_AUTHENTICATION authentication,
        CGXByteBuffer& password);

    /**
    * Get attribute access mode.
    *
    * @param arg
    *            Value event argument.
    * @return Access mode.
    * @throws Exception
    *             Server handler occurred exceptions.
    */
    DLMS_ACCESS_MODE GetAttributeAccess(CGXDLMSValueEventArg* arg);

    /**
    * Get method access mode.
    *
    * @param arg
    *            Value event argument.
    * @return Method access mode.
    * @throws Exception
    *             Server handler occurred exceptions.
    */
    DLMS_METHOD_ACCESS_MODE GetMethodAccess(CGXDLMSValueEventArg* arg);

    /**
    * Accepted connection is made for the server. All initialization is done
    * here.
    */
    void Connected(
        CGXDLMSConnectionEventArgs& connectionInfo);

    /**
     * Client has try to made invalid connection. Password is incorrect.
     *
     * @param connectionInfo
     *            Connection information.
     */
    void InvalidConnection(
        CGXDLMSConnectionEventArgs& connectionInfo);
    /**
     * Server has close the connection. All clean up is made here.
     */
    void Disconnected(
        CGXDLMSConnectionEventArgs& connectionInfo);

    /**
    * Get selected value(s). This is called when example profile generic
    * request current value.
    *
    * @param type
    *            Update type.
    * @param args
    *            Value event arguments.
    */
    void PreGet(
        std::vector<CGXDLMSValueEventArg*>& args);

    /**
    * Get selected value(s). This is called when example profile generic
    * request current value.
    *
    * @param type
    *            Update type.
    * @param args
    *            Value event arguments.
    */
    void PostGet(
        std::vector<CGXDLMSValueEventArg*>& args);
};
