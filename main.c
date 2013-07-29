#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "bumerang_messages.h"
#include "send.h"
#include "get.h"
#include "connect.h"

#define VERSION         "0.1"

int checkArgc(int argc);
void usage(void);

int main(int argc, char *argv[]) {

    int socket;
    char c;
    char *filePath = NULL;
    int key;
    int check_send = 0;
    int check_get = 0;
    char *ipaddr;

    if (checkArgc(argc))
        usage();

    while ((c = getopt (argc, argv, "hsg:p:l:")) != -1)
        switch (c)
        {
            case 'h':
                usage();
                break;
            case 's':
                check_send = 1;
                break;
            case 'g':
                ipaddr = optarg;
                check_get = 1;
                break;
            case 'p':
                key = atoi(optarg);
                break;
            case 'l':
                filePath = optarg;
                break;
            default:
                usage();
        }

    if (check_send) {
        socket = connectForSend();
        sendFile(socket, filePath, key);
        closeConnect(socket);
    }

    if (check_get) {
        socket = connectForGet(ipaddr);
        getFile(socket, filePath, key);
        closeConnect(socket);
    }

    return 0;
}

int checkArgc(int argc)
{
    if (argc < 2)
        usage();
    return 0;
}

void usage(void)
{
    printf("\nBumerang v%s (Usage)\n"
           "Send a file  : bumerang -s -p <password> -l <filepath>\n"
           "Get a file   : bumerang -g <ipaddr> -p <password> -l <folderpath>\n"
           "Get a file-2 : bumerang -g <ipaddr> -p <password>\n" 
           "Help         : bumerang -h\n\n", VERSION);

    exit(EXIT_FAILURE);
}
