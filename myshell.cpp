#include "mycommands.h"

#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>

int cat(const char* filename) {
    FILE* file;
    if((file = fopen(filename, "r")) == NULL)
    {
        printf("cat: %s: no such file or directory found\n", filename);
    }
    else
    {
        int getFileContents = fgetc(file);
        while(getFileContents != EOF)
        {
            printf("%c", getFileContents);
            getFileContents = fgetc(file);
        }
    }
    fclose(file);
    return 0;
}

int ls(void) {
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    dir = opendir("."); //opens at current directory
    while ((dp=readdir(dir)) != NULL) { //if first entry in dirent structure is null
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            continue;
        } else {
            file_name = dp->d_name; // file_name is allocated to char pointer
            printf("%s\n",file_name); //print directory
        }
    }
    closedir(dir);
    return 0;
}

int l(void){        //to go with ls function as ls -l
    DIR *dir;
    struct dirent *dp;
    struct stat fileStat;
    char * file_name;
    char buf[512];
    dir = opendir(".");
    char *t;
    printf("Permissions   SLinks  Size  Type      Last Accessed\t    Name\n");
    while ((dp=readdir(dir)) != NULL) {
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            continue;
        } else {
            file_name = dp->d_name; // store directory name in file_name
            sprintf(buf, "%s", file_name);
            stat(buf, &fileStat);
            t = ctime(&fileStat.st_atime);
            printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");//user read
            printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");//user write
            printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");//user execute
            printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");//group read
            printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");//group write
            printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");//group execute
            printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");//others read
            printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");//others write
            printf( (fileStat.st_mode & S_IXOTH) ? "x\t" : "-\t");//others execute
            printf("%-5d ", fileStat.st_nlink);
            int max_width, value;
            max_width = 8;
            value = fileStat.st_size;
            printf("%-5d ",value);
            
            switch (fileStat.st_mode & S_IFMT) {
                case S_IFBLK:  printf("%-5s","b  "); break; //Block special file
                case S_IFCHR:  printf("%-5s","c  "); break; //Character special file
                case S_IFDIR:  printf("%-5s","d  "); break; //Directory file
                case S_IFIFO:  printf("%-5s","p  "); break; //FIFO or pipe
                case S_IFLNK:  printf("%-5s","l  "); break; //Symbolic link
                case S_IFSOCK: printf("%-5s","s  "); break; //Socket
                case S_IFREG:  printf("%-5s","r  "); break; //regular file
                    //File type isn't identified
                default:       printf("%-5s","-  "); break;
            }
        }
        if (t[strlen(t)-1] == '\n')
            t[strlen(t)-1] = '\0';
        printf("%-10s  ", t);
        printf("%-10s \n",file_name); //print directory
    }
    closedir(dir);
    return 0;
}

int cp(const char* src, const char* dest)
{
    FILE *f1;
    FILE *f2;
    long length;
    
    f1 = fopen(src, "r");
    f2 = fopen(dest, "w");
    if(f1 == NULL || f2 == NULL)
        return 3;
    fseek(f1,0L,SEEK_END);
    length = ftell(f1);
    rewind(f1);
    
    char *buf = (char*) malloc(sizeof(char)*length);
    size_t bytes = fread(buf, length, sizeof(char), f1);
    bytes = fwrite(buf, length, sizeof(char), f2);
    
    fclose(f1);
    fclose(f2);
    free(buf);
    return 0;
}

int grep() {
    const char input[] = "hello test.txt";
    char* line;
    size_t length = 0;
    ssize_t read;
    FILE* file = fopen("test.txt", "r");
    if(file == NULL) {
        printf("Invalid file\n");
        return 1;
    }
    while((read = getline(&line, &length, file)) != -1);
    
    return 0;
}

int help() {
    printf("cat\nls\ncp\n\grep\n");
    return 0;
}

int main() {
    const char prompt[] = "ksh$ ";
    char input[80];
    const char d[] = " \n";
    printf("Welcome to the Kennesaw Shell!\nType 'help' for available commands.\n");
    do {
        printf("%s", prompt);
        fflush(stdout);
        fgets(input, 80, stdin);
        const char* token = strtok(input, d);
        if (strcmp(token, "ls") == 0) {
            const char* arg = strtok(NULL, d);
            if(arg == NULL) {
                ls();
            }
            else if (strcmp(arg, "-l") == 0) {
                l();
            }
            else {
                printf("failure");
            }
        }
        else if (strcmp(token, "cat") == 0) {
            char* filename = strtok(NULL, d);
            cat(filename);
        }
        else if (strcmp(token, "cp") == 0) {
            char* src = strtok(NULL, d);
            if (src != NULL)
            {
                const char *dest = strtok(NULL, d);
                if (dest != NULL)
                    cp(src, dest);
                else
                    printf("Not a valid ls argument1\n");
            }
            else
                printf("Not a valid ls argument2\n");
        }
        else if (strcmp(token, "grep") == 0) {
            grep();
        }
        else if (strcmp(token, "help") == 0) {
            help();
        }
        else {
            if(strcmp("exit", token) != 0)
                printf("Not a valid command\n");
        }
    } while (strcmp("exit", input) != 0);
    
    return 0;
}