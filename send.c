#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#include "send.h"
#include "connect.h"
#include "bumerang_messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OS_WIN
    #include <windows.h>
#else
    #include <sys/socket.h>
    #define SOCKET_ERROR    -1
#endif

#define FILENAMELEN     256
#define BUFSIZE         10000


static int write_socket(int s, const void *pBuf, int n);

int sendFile(int socket, const char *path, int key)
{

    char buf[BUFSIZE];
    FILE *f;
    int filesize;
    char filename[FILENAMELEN];
    int result_read;
    int sum = 0;

    srand(key);

    if ((f = fopen(path, "rb")) == NULL)
        bumerang_error("The file could not be read");

    if (strrchr(path, '/'))
        sprintf(filename, "%s", strrchr(path, '/') + 1);
    else
        sprintf(filename, "%s", path);

    fseek(f, 0, SEEK_END);
    filesize = ftell(f) / 1024;
    fseek(f, 0, SEEK_SET);

    printf("* [%s] file is being sent...(Size: %d Kb)\n\n", filename, filesize);

    if (send(socket, filename, FILENAMELEN, 0) == SOCKET_ERROR)
        bumerang_error("Failed to send file properties");
    send(socket, &filesize, 4, 0);

    while ((result_read = fread(buf, 1, BUFSIZE, f))) {
        int i, n, k;

        //encryption
        for (i = 0; result_read != i; i++)
            buf[i] = buf[i] ^ rand();

        sum += write_socket(socket, buf, result_read);

        //graphic (==>(%100))
        if (filesize >= 1) {
            k = n = (sum / 1024 * 100) / filesize;
            n /= 2;
            while (n--)
                printf("=");
            printf(">(%%%d)\r", k);
        }
    }

    if (filesize == sum / 1024)
        bumerang_info("File sent");
    else
        bumerang_error("File transfer was not completed");

    fclose(f);

    return 0;
}

static int write_socket(int s, const void *pBuf, int n) {
    int result;
    int index = 0;
    int left = n;

    while (left > 0) {
        result = send(s, (const char *) pBuf + index, left, 0);

        if (result == 0)
            return index;

        if (result == SOCKET_ERROR)
            return SOCKET_ERROR;

        index += result;
        left -= result;
    }

    return index;
}
