#ifndef INTERFACE
#define INTERFACE

#include "files.h"

#include <ncurses.h>
#include <sys/ioctl.h>
#include <signal.h>

void sigWinch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

	resizeterm(size.ws_row, size.ws_col);
}

void updateDir(WINDOW** dirWindow, const char* folder)
{
	wclear(*dirWindow);
	wprintw(*dirWindow, "Current directory: %s", folder);
	wrefresh(*dirWindow);
}

void initCurses(const char* workingDir, WINDOW ** mainWindow, WINDOW ** subWindow, WINDOW ** dirWindow, WINDOW ** listWindow, WINDOW** controlWindow, WINDOW ** InputWindow)
{
	int x, y;
	initscr(); 
	signal(SIGWINCH, sigWinch); 
	
	cbreak();
	curs_set(0);
	start_color();
	refresh();
	init_pair(1, COLOR_BLUE, COLOR_GREEN);
	init_pair(2, COLOR_YELLOW, COLOR_BLUE);
	init_pair(3, COLOR_GREEN, COLOR_BLUE);
	init_pair(4, COLOR_WHITE, COLOR_BLUE);
	refresh();
	
	*mainWindow = newwin(40, 80, 2, 4);
	wbkgd(*mainWindow, COLOR_PAIR(1));
	box(*mainWindow, '|', '-');

	*subWindow = derwin(*mainWindow, 38, 78, 1, 1);
	wbkgd(*subWindow, COLOR_PAIR(2));
	box(*subWindow, '|', '-');

	*dirWindow = derwin(*subWindow, 1, 76, 1, 1);
	wbkgd(*dirWindow, COLOR_PAIR(4));
	updateDir(dirWindow, workingDir);

	*listWindow = derwin(*subWindow, 33, 76, 2, 1);
	*controlWindow = derwin(*subWindow, 3, 76, 34, 1);	
	wattron(*controlWindow, COLOR_PAIR(4));	
	box(*controlWindow, ' ', '-');

	*InputWindow = derwin(*controlWindow, 1, 74, 1, 1);	
	wprintw(*InputWindow, "f1-rename f2-move f3-nano f4-exit\n");
	
	wrefresh(*InputWindow);
	wrefresh(*controlWindow);
	wrefresh(*subWindow);
	wrefresh(*mainWindow);
}

//вывод files
void printDir(struct DirList list, WINDOW ** listWindow)
{
	wclear(*listWindow);
	wbkgd(*listWindow, COLOR_PAIR(2));
    for (int i = 0; i < list.size; ++i)
	{
		int isFile = list.list[i].isFile;
		wattron(*listWindow, isFile ? COLOR_PAIR(2)|A_BOLD : COLOR_PAIR(3)|A_BOLD);
		wprintw(*listWindow, "%d. %s\n", i + 1, list.list[i].path);
		wattroff(*listWindow, isFile ? COLOR_PAIR(2)|A_BOLD : COLOR_PAIR(3)|A_BOLD);
    }
    wrefresh(*listWindow);
}

void restoreScreen(WINDOW** InputWindow)
{
	wclear(*InputWindow);
	wprintw(*InputWindow, "f1-rename f2-move f3-nano f4-exit\n");
	wrefresh(*InputWindow);
	//wechochar(*InputWindow, 'f1-edit f2-nano f3-exit\n');
}

char* askInput(WINDOW **Window, const char* promt)
{
	char *line = calloc(256, sizeof(char));

	wclear(*Window);
	wprintw(*Window, "%s", promt);
	wrefresh(*Window);
	wscanw(*Window, "%s", line);

	return line;
}

void commandCurses(char** workingDir, struct DirList list, WINDOW** mainWindow, WINDOW** subWindow, WINDOW** controlWindow, WINDOW** InputWindow, WINDOW** listWindow, WINDOW** dirWindow)
{
	int i = 0;
	int ch;
	int max_len = 10;
	char *line;

	keypad(*InputWindow, TRUE);
	noecho();
	while (ch = wgetch(*InputWindow))
	{	
		switch (ch)
		{
			case KEY_F(1):
			{
				curs_set(1);
				echo();
				line = askInput(InputWindow, "Line number: ");
				int result = atoi(line);
				if (result <= 0 || result > list.size) 
				{
					restoreScreen(InputWindow);
					break;
				}
				line = askInput(InputWindow, "New name: ");
				renameUnit(*workingDir, list.list[result - 1], line);

				// Новое имя
				list.list[result - 1].path = line;

				restoreScreen(InputWindow);
				printDir(list, listWindow);

				break;
			}

			case KEY_F(2):
			{
				curs_set(1);
				echo();
				line = askInput(InputWindow, "Enter folder name: ");
				char* newWorkingDir = moveDirectory(list, *workingDir, line);
				if (newWorkingDir == NULL) 
				{
					restoreScreen(InputWindow);
					break;
				}

				// Освобождение ресов
				free(*workingDir);
				deleteList(list);
				// Новое	
				*workingDir = newWorkingDir;
				list = makeList(*workingDir);

				// экраны
				restoreScreen(InputWindow);
				updateDir(dirWindow, *workingDir);
				printDir(list, listWindow);
				break;
			}

			case KEY_F(3):
			{
				curs_set(1);
				echo();
				line = askInput(InputWindow, "Line number: ");
				int result = atoi(line);
				if (result <= 0 || result > list.size) 
				{
					restoreScreen(InputWindow);
					break;
				}

				char filename[strlen(*workingDir) + 2 + strlen(list.list[result - 1].path)];
				char command[strlen(filename) + 11];
				sprintf(filename, "%s/%s", *workingDir, list.list[result - 1].path);
				sprintf(command, "/bin/nano %s", filename);

				system(command);
				return;
				break;
			}
			case KEY_F(4):
			{
				clearCurses(mainWindow, subWindow, dirWindow, listWindow, controlWindow, InputWindow);
				system("clear");
				exit(0);
			}
			
			default:
				continue;
		}

		curs_set(0);
		noecho();
	}
}

void clearCurses(WINDOW ** mainWindow, WINDOW ** subWindow, WINDOW ** dirWindow, WINDOW ** listWindow, WINDOW ** controlWindow, WINDOW ** InputWindow)
{
	
	delwin(*dirWindow);
	delwin(*listWindow);
	delwin(*InputWindow);
	delwin(*controlWindow);
	delwin(*subWindow);
	delwin(*mainWindow);
	getch();	
	endwin();
}


#endif