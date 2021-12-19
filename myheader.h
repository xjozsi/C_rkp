#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <omp.h>
#include <signal.h>
#define BUFSIZE 1024

void WhatToDo(int sig)
{
    if (sig == SIGALRM)
    {
        fprintf(stderr, "Sorry, time out!\n");
        exit(7);
    }
    else if (sig == SIGINT)
    {
        pid_t p;
        p = fork();
        if (p == 0)
        {
            printf("\nSorry, Ctrl + C shortcut has been disabled!\n");
            kill(getpid(), SIGKILL);
        }
    }
}

char *Unwrap(char *Pbuff, int NumCh)
{
    char *str = (char *)calloc(NumCh, sizeof(char));
    if (str == NULL)
    {
        fprintf(stderr, "HIBA! NEM SIKERÜLT A MEMÓRIAFOGLALÁS!\n");
        exit(1);
    }

#pragma omp parallel for
    for (int i = 0; i < 3 * NumCh; i++)
    {

        str[i / 3] = (i % 3 == 0) ? (str[i / 3] | (Pbuff[i] << 6)) : ((i % 3 == 1) ? (str[i / 3] | ((Pbuff[i] << 3) & 56)) : (str[i / 3] | (Pbuff[i] & 7)));
    }
   
    free(Pbuff);
    return str;
}

char *ReadPixels(int f, int *NumCh)
{
    
    char *valid = (char *)calloc(2, sizeof(char));
    read(f, valid, 2);
    if (strcmp("BM", valid) != 0)
    {
        fprintf(stderr, "HIBA! EZ NEM EGY BMP FÁJL!\n");
        exit(2);
    }
    lseek(f, 6, SEEK_SET);
    read(f, NumCh, 4);

    int kezd = 0;
    read(f, &kezd, 4);
    char *result = (char *)malloc(((*NumCh) * 3) * sizeof(char));
    lseek(f, kezd, SEEK_SET);
    read(f, result, (*NumCh) * 3);
    close(f);
    return result;
}

int BrowseForOpen()
{
    DIR *d;
    struct dirent *e;
    struct stat inode;
    int f;
    char *userinput = (char *)malloc(FILENAME_MAX * sizeof(char));

    chdir(getenv("HOME"));
    while (1)
    {

        d = opendir(".");
        while ((e = readdir(d)) != NULL)
        {
            if ((*e).d_type == DT_DIR)
            {
                printf("\033[1;35m");
                printf("%s\t", (*e).d_name);
                printf("\033[0;0m");
            }
            else
            {
                printf("%s\t", (*e).d_name);
            }
        }
        closedir(d);
        printf("\n");
        do
        {
            printf(">> ");
            scanf("%s", userinput);
        } while (stat(userinput, &inode) != 0);
        if (inode.st_mode & S_IFREG)
        {
            f = open(userinput, O_RDONLY);
            signal(SIGALRM, WhatToDo);
            signal(SIGINT, WhatToDo);
            alarm(1);
            break;
        }
        chdir(userinput);
        system("clear");
    }

    free(userinput);

    return f;
}

int Post(char *neptunID, char *message, int NumCh)
{

    char buffer[BUFSIZE];
    struct sockaddr_in server;
    int s;
    int err;
    int recvdata;
    char *path = "/~vargai/post.php";
    char *host = "irh.inf.unideb.hu";
    char *good = "The message has been received.";

    sprintf(buffer, "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nNeptunID=%s&PostedText=%s\r\n", path, host, NumCh + 27, neptunID, message);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("193.6.135.162");
    server.sin_port = htons(80);
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Socket létrehozása sikertelen.\n");
        return 3;
    }

    err = connect(s, (struct sockaddr *)&server, sizeof(server));
    if (err < 0)
    {
        fprintf(stderr, "Kapcsolat létrehozása sikertelen.\n");
        return 4;
    }
    

    write(s, buffer, strlen(buffer));

    recvdata = recv(s, buffer, BUFSIZE, 0);
    if (recvdata < 0)
    {
        fprintf(stderr, "A webszerver válaszának fogadása sikertelen.\n");
        return 5;
    }

    if (strstr(buffer, good) == NULL)
    {
        fprintf(stderr, "The message has NOT been received!\n");
        return 6;
    }
    puts(good);

    close(s);

    return 0;
}