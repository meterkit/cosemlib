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
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include "../include/getopt.h"
#include <tchar.h>
#include <conio.h>
#include <Winsock.h> //Add support for sockets	
#include <time.h>
#include <process.h>//Add support for threads
#else //Linux includes.
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#endif
#include "../include/GXDLMSServerSN.h"
#include "../include/GXDLMSServerLN.h"
#include "../include/GXDLMSServerSN_47.h"
#include "../include/GXDLMSServerLN_47.h"

int Start(int port, int trace)
{
    int ret;
    //Create Network media component and start listen events.
    //4059 is Official DLMS port.
    ///////////////////////////////////////////////////////////////////////
    //Create Gurux DLMS server component for Short Name and start listen events.
    CGXDLMSServerSN SNServer;
    if ((ret = SNServer.Init(port)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server in port %d.\r\n", port);
    printf("Example connection settings:\n");
    printf("Gurux.DLMS.Client.Example.Net -r sn -h localhost -p %d\n", port);
    printf("----------------------------------------------------------\n");
    ///////////////////////////////////////////////////////////////////////
    //Create Gurux DLMS server component for Short Name and start listen events.
    CGXDLMSServerLN LNServer;
    if ((ret = LNServer.Init(port + 1)) != 0)
    {
        return ret;
    }
    printf("Logical Name DLMS Server in port %d.\r\n", port + 1);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port + 1);
    printf("----------------------------------------------------------\n");
    ///////////////////////////////////////////////////////////////////////
    //Create Gurux DLMS server component for Short Name and start listen events.
    CGXDLMSServerSN_47 SN_47Server;
    if ((ret = SN_47Server.Init(port + 2)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server with IEC 62056-47 in port %d.\r\n", port + 2);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -r sn -h localhost -p %d -w\n", port + 2);
    printf("----------------------------------------------------------\n");
    ///////////////////////////////////////////////////////////////////////
    //Create Gurux DLMS server component for Short Name and start listen events.
    CGXDLMSServerLN_47 LN_47Server;
    if ((ret = LN_47Server.Init(port + 3)) != 0)
    {
        return ret;
    }
    printf("Logical Name DLMS Server with IEC 62056-47 in port %d.\r\n", port + 3);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -w\n", port + 3);
    printf("----------------------------------------------------------\n");
    printf("Press Enter to close application.\r\n");
    getchar();
    return 0;
}

void ShowHelp()
{
    printf("Gurux DLMS example Server implements four DLMS/COSEM devices.\r\n");
    printf(" -t [Error, Warning, Info, Verbose] Trace messages.\r\n");
    printf(" -p Start port number. Default is 4060.\r\n");
}

#if defined(_WIN32) || defined(_WIN64)//Windows includes
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    strcpy(DATAFILE, argv[0]);
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    char *p = strrchr(DATAFILE, '\\');
    *p = '\0';
    strcat(DATAFILE, "\\data.csv");
#else
    char *p = strrchr(DATAFILE, '//');
    *p = '\0';
    strcat(DATAFILE, "//data.csv");
#endif

    int opt, port = 4060, trace = 0;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    while ((opt = getopt(argc, argv, "t:p:")) != -1)
    {
        switch (opt)
        {
        case 't':
            //Trace.
            trace = 1;
            break;
        case 'p':
            //Port.
            port = atoi(optarg);
            break;
        case '?':
        {
            if (optarg[0] == 'p') {
                printf("Missing mandatory port option.\n");
            }
            else if (optarg[0] == 't') {
                printf("Missing mandatory trace option.\n");
            }
            else
            {
                ShowHelp();
                return 1;
            }
        }
        break;
        default:
            ShowHelp();
            return 1;
        }
    }
    Start(port, trace);
#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
}

