#include<stdio.h>
//структура - вывод побитово

struct test {
	unsigned char b1 : 1;
	unsigned char b2 : 1;
	unsigned char b3 : 1;
	unsigned char b4 : 1;
	unsigned char b5 : 1;
	unsigned char b6 : 1;
	unsigned char b7 : 1;
	unsigned char b8 : 1;
};

int main(void) {

	int i;
	char a = 0b11011001;
	struct test* ptr;

	ptr = (struct test*)&a;
	ptr->b5 = 0;

	printf("%u", *ptr);
	return 0;
}