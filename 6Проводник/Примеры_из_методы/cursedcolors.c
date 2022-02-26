#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
void sig_winch(int signo){
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
	}
	
	/*по умолчанию при инициализации цвета окно stdscr заполняется атрибутами цветовой пары 0, как если бы была вызвана функция wbkgd(stdscr, COLOR_PAIR(0));*/

int main(int argc, char ** argv){
	WINDOW * wnd;
	WINDOW * subwnd;
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(FALSE);
	start_color();/*start_color() инициализирует управление цветом ncurses*/
	refresh();
	/*цветовые пары создаются с помощью функции init_pair().*/
	/*Новые цветовые пары*/
	/*1-номер пары/цвет символа/цвет фона
	Цветовая пара с номером 0 определена в ncurses как «белый на черном» и изменить ее нельзя.*/
	init_pair(1, COLOR_BLUE, COLOR_GREEN);
	init_pair(2, COLOR_YELLOW, COLOR_BLUE);
	wnd = newwin(5, 18, 2, 4);
	/*установка в attron/wattron цветового атрибута COLOR_PAIR(X)
	A_BOLD влияет на яркость цвета символов (но не на яркость цвета фона)
	яркость фона - комбинировать этот атрибут с атрибутом A_REVERSE*/
	wattron(wnd, COLOR_PAIR(1));
	box(wnd, '|', '-');
	subwnd = derwin(wnd, 3, 16, 1, 1);
	/*заполнить структуру данных, соответствующую массиву символов окна, заданными атрибутами текста.*/
	wbkgd(subwnd, COLOR_PAIR(2));
	/*Функция wbkgd() уже заполнила символьный массив окна subwnd нужными атрибутами цвета и нам остается только указать атрибут яркости.*/
	wattron(subwnd, A_BOLD);
	wprintw(subwnd, "Hello, brave new curses world!\n");
	wrefresh(subwnd);
	wrefresh(wnd);
	delwin(subwnd);
	delwin(wnd);
	wmove(stdscr, 8, 1);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
	}
