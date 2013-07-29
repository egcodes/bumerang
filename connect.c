#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#include "connect.h"
#include "bumerang_messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OS_WIN
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
    #define SOCKET_ERROR    -1
#endif

#define PORTNO          33644
#define BACKLOG         8


#ifdef OS_WIN
    static void getIpAddressForNT(char *addrp);
#else
    static void getIpAddressForPosix(char *addrp);
#endif

static void connectError(char *errorname);

int connectForSend(void)
{
    int servfd;
    int clientfd;
    int yes = 1;
    struct sockaddr_in servaddr, clientaddr;
    char ipaddr[100];

    #ifdef OS_WIN
        WSADATA wsadata;
        int addr_len;
        WSAStartup(MAKEWORD(2,2), &wsadata);
    #else
        socklen_t addr_len; 
    #endif

    bumerang_info("Trying to connection");

    #ifdef OS_WIN
        getIpAddressForNT(ipaddr);
    #else
        getIpAddressForPosix(ipaddr);
    #endif

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORTNO);
    memset(&servaddr.sin_zero, '\0', 8);
    servaddr.sin_addr.s_addr = inet_addr(ipaddr);


    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        connectError("socket");

    if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof (int)) == SOCKET_ERROR)
        connectError("setsockopt");

    if (bind(servfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) == SOCKET_ERROR)
        connectError("bind");

    if (listen(servfd, BACKLOG) == SOCKET_ERROR)
        connectError("listen");

    bumerang_info("Connection successfull");
    bumerang_info("Waiting the client");

    addr_len = sizeof (clientaddr);
    if ((clientfd = accept(servfd, (struct sockaddr *) &clientaddr, &addr_len)) == SOCKET_ERROR)
        connectError("accept");

    return clientfd;
}


int connectForGet(const char *ipaddr)
{
    int servfd;
    struct sockaddr_in servaddr;

    #ifdef OS_WIN
        WSADATA wsadata;
        WSAStartup(MAKEWORD(2,2), &wsadata);
    #endif

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORTNO);
    memset(&servaddr.sin_zero, '\0', 8);
    servaddr.sin_addr.s_addr = inet_addr(ipaddr);

    bumerang_info("Trying to connection");

    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        connectError("socket");

    if (connect(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
        connectError("connect");

    bumerang_info("Connection successfull");
    bumerang_info("Waiting the file");

    return servfd;
}

void closeConnect(int socket)
{
    shutdown(socket, 2);
    if (close(socket) == SOCKET_ERROR)
        connectError("close");
}

#ifdef OS_WIN
static void getIpAddressForNT(char *addrp)
{
    struct hostent *host = gethostbyname("");

    sprintf(addrp, "%s", inet_ntoa(*(struct in_addr *)*host->h_addr_list));
}
#else
static void getIpAddressForPosix(char *addrp)
{
    int fd;
    struct ifreq ifr;
   
    fd = socket(AF_INET, SOCK_DGRAM, 0);
   
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
   
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
   
    ioctl(fd, SIOCGIFADDR, &ifr);
   
    close(fd);

 	/* display result */
 	sprintf(addrp, "%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}
#endif

static void connectError(char *errorname)
{
    #ifdef OS_WIN
        wprintf(L"%s: %ld\n", errorname, WSAGetLastError());
        WSACleanup();
    #else
        perror(errorname);
    #endif

    exit(EXIT_FAILURE);
}

