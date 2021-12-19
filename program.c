#include "myheader.h"

int main(int argc, char *argv[])
{
    int fd;
    char *NeptunID = "QXCRCB";
    int n;
    int sending;
    switch (argc)
    {
    case 1:

        fd = BrowseForOpen();

        if (fd == -1)
        {
            fprintf(stderr, "HIBA A FÁJL MEGNYITÁSAKOR!\n");
            return 1;
        }

        signal(SIGALRM, WhatToDo);
        signal(SIGINT, WhatToDo);
        alarm(1);

        char *pxarr = ReadPixels(fd, &n);
        char *end = Unwrap(pxarr, n);
        alarm(0);

        sending = Post(NeptunID, end, n);
        if (sending != 0)
        {
            fprintf(stderr, "Valami hiba a POST metódus során!\n");
        }

        free(end);
        break;
    case 2:
        if (strcmp(argv[1], "--version") == 0)
        {
            printf("\033[1;33m");
            printf("\n----------- RKP PROJEKTMUNKA -----------\n");
            printf("\033[0;36m");
            printf("\n\t latest version: 6.0\n");
            printf("\n\t latest update: 2021. Ápr. 19.\n");
            printf("\n\t Made by Veres József\n");
            printf("\n\t\t : - )\n");
            printf("\033[1;33m");
            printf("\n-----------------------------------------\n");
            printf("\033[0;0m");
        }
        else if (strcmp(argv[1], "--help") == 0)
        {
            printf("Használat: ./a.out [FAJL] || [KAPCSOLO]\n");
            printf("Ez a program egy bmp fájlból szöveges információt információt kódol ki,\nés elküldi az eredményt egy webszervernek!\n");
            printf(" --version\t program version.\n");
            printf(" --help\t\t súgó.\n\n");
            printf("Ha kapcsoló vagy fájlnév nélkül indítod el a programot akkor egy filebrowser segítségével\noda tudsz navigálni a beolvasni kívánt bmp fájlhoz.\n\n");
            
        }
        else
        {
            fd = open(argv[1], O_RDONLY);

            if (fd == -1)
            {
                fprintf(stderr, "HIBA A FÁJL MEGNYITÁSAKOR!\n");
                return 1;
            }

            signal(SIGALRM, WhatToDo);
            signal(SIGINT, WhatToDo);
            alarm(1);

            char *pxarr = ReadPixels(fd, &n);
            char *end = Unwrap(pxarr, n);
            alarm(0);

            sending = Post(NeptunID, end, n);
            if (sending != 0)
            {
                fprintf(stderr, "Valami hiba a POST metódus során!\n");
            }

            free(end);
        }
        break;
    }

    return 0;
}