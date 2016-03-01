#include "tcp_server.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF		"\r\n"
#define PORT	 	4059
#define MAX_CLIENTS 10

#define BUF_SIZE	1024


typedef struct
{
   SOCKET sock;
} peer;

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}


static void clear_peers(peer *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_peers(peer *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(peer));
   /* number client - 1 */
   (*actual)--;
}

/*
static void broadcast(peer *clients, peer sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      // we don't send message to the sender
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
         //   strncpy(message, sender.name, BUF_SIZE - 1);
        //    strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
       //  strncat(message, buffer, sizeof message - strlen(message) - 1);
      //   write_peer(clients[i].sock, message);
      }
   }
}
*/

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_peer(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disconnect the client */
      n = 0;
   }
   return n;
}

static void write_peer(SOCKET sock, const char *buffer, size_t size)
{
   if(send(sock, buffer, size, 0) < 0)
   {
      perror("send()");
    //  exit(errno);
   }
}

static void app(data_handler data_func)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   peer peers[MAX_CLIENTS];

   fd_set working_set;
   fd_set master_set;

   FD_ZERO(&master_set);

   /* add the connection socket */
   FD_SET(sock, &master_set);

   puts("[TCP Server] TCP Server started");

   while(1)
   {


      /* add socket of each client */
      /*
      for(i = 0; i < actual; i++)
      {
         FD_SET(peers[i].sock, &master_set);
      }
      */

      memcpy(&working_set, &master_set, sizeof(master_set));

      if(select(max + 1, &working_set, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }


      if(FD_ISSET(sock, &master_set))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         int sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;
         FD_SET(csock, &master_set);

         peer c = { csock };
         peers[actual] = c;
         actual++;

         puts("[TCP server] New connection!");
      }

     for(int i = 0; i < actual; i++)
     {
        /* a client is talking */
        if(FD_ISSET(peers[i].sock, &master_set))
        {
           int c = read_peer(peers[i].sock, buffer);
           /* client disconnected */
           if(c == 0)
           {
              closesocket(peers[i].sock);
              remove_peers(peers, i, &actual);
              puts("Client disconnected !");
           }
           else
           {
               puts("[TCP server] New data received!");
               if (data_func != NULL)
               {
                   // FIXME: change channel number with instance when multi threaded server is available
                   int ret = data_func(0U, buffer, c);
                   if (ret > 0)
                   {
                        write_peer(peers[i].sock, buffer, ret);
                   }
               }

               //broadcast(peers, client, actual, buffer, 0);
           }
        }
    }
   }

   clear_peers(peers, actual);
   end_connection(sock);
}


int tcp_server_init(data_handler data_func)
{
   init();

   app(data_func);

   end();

   return EXIT_SUCCESS;
}
