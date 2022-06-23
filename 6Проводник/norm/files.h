#ifndef FILES
#define FILES

#include <string.h>
#include <malloc.h>
#include <locale.h>
#include <stdlib.h>
#include <termios.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct ListPosition {
    char *path;
    int isFile;
} listpos;

struct ListPosition initPosition(const char *path, int file)
{
    struct ListPosition pos;
    pos.path = path;
    pos.isFile = file;

    return pos;
}

struct DirList {
    struct ListPosition *list;
    int size;
} dirlist;

struct DirList initList()
{
    struct DirList list;
    list.size = 0;
    list.list = calloc(list.size, sizeof(struct ListPosition));

    return list;
}

void deleteList(struct DirList list) 
{
    free(list.list);
}

//обработчик is file?
int isFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//указатель на директорию
DIR* recvDir(const char *dir)
{   
    DIR *d;

    if (dir == NULL)
        d = opendir(".");
    else d = opendir(dir);

    return d;
}

void renameUnit(const char *directory, struct ListPosition old, const char *new)
{
    char *oldUnit = calloc(strlen(directory) + 1 + strlen(old.path), sizeof(char)),
         *newUnit = calloc(strlen(directory) + 1 + strlen(new), sizeof(char));
    sprintf(oldUnit, "%s/%s", directory, old.path);
    sprintf(newUnit, "%s/%s", directory, new);
    rename(oldUnit, newUnit);
}

char *moveDirectory(struct DirList list, const char *current, const char* next)
{
    char *newDirectory = calloc(256, sizeof(char));
    if (!strcmp(next, "."))
    {
        char* lastFolder = strrchr(current, '/');
        strncpy(newDirectory, current, (lastFolder - current) / sizeof(char));
    } 
    else 
    {
        int found = 0;
        for (int i = 0; i < list.size; ++i)
            if (!strcmp(list.list[i].path, next) && !list.list[i].isFile)
            {
                found = 1;
                break;
            }
        if (found)
            sprintf(newDirectory, "%s/%s", current, next);
        else return NULL;
    }

    return newDirectory;
}

//прочтение содержимого текущего каталога
struct DirList makeList(const char *path)
{
    //Если NULL - выводить текущую директорию
    char* workingPath = path == NULL ? "." : path;
    
    DIR *d;
    struct dirent *dir;

    struct DirList list = initList();
    if (!(d = recvDir(workingPath))) return list;

    while ((dir = readdir(d)) != NULL)
    {
        if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            continue;

        list.list = realloc(list.list, sizeof(struct ListPosition) * (list.size + 1));
        list.list[list.size] = initPosition(dir->d_name, isFile(dir->d_name));
        ++list.size;
    }

    return list;
}

#endif