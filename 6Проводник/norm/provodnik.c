#include "interface.h"
#include "files.h"

/*
|---------------------------|
|   Текущая директория : $wantedDir
|---------------------------|
|   1. interface.h
|   2.
|
|
|
|
|
|
|--------------------------|
| rename - f1 - расшарить до функционала
|--------------------------|
*/

int main(int argc, char **argv)
{
    DIR* dir;
    if (argc > 1) 
    {
        dir = opendir(argv[1]);
        if (dir) closedir(dir);
        else 
        {
            printf("Directory does not exist or insufficient privileges!\n");
            exit(1);
        }
    }
    char* workingDir = argc > 1 ? argv[1] : getcwd(NULL, 0);

    WINDOW  *mainWindow,
            *subWindow,
			*dirWindow,
			*listWindow,
			*controlWindow,
            *InputWindow;

    signal(SIGWINCH, sigWinch); /*обработчик сигнала SIGWINCH (resize)*/

    while (true)
    {
        initCurses(workingDir, &mainWindow, &subWindow, &dirWindow, &listWindow, &controlWindow, &InputWindow);
        struct DirList list = makeList(workingDir);
        printDir(list, &listWindow);
        commandCurses(&workingDir, list, &mainWindow, &subWindow, &controlWindow, &InputWindow, &listWindow, &dirWindow);
    }
}
