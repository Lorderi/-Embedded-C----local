#include<stdio.h>
//указатели - вывод побайтово

int main(void) {
	int a = 270533154, i, n; //34.02.32.16
	char* ptr;

	ptr = (char*)&a;  //явное приведение типа

	printf("Number at start:\n%d\n\n", a);
	for (i = 0; i < 4; i++) {
		n = i + 1;
		printf("%d byte of a = %d\n", n, *ptr);
		if (n == 3) {
			*ptr = 23;
		}
		ptr++;
	}
	printf("\n\n\n");

	printf("Number after another:\n%d\n\n", a);

	ptr = (char*)&a;
	printf("1st byte of a = %d\n", *ptr); // -->34
	ptr++;
	printf("2nd byte of a = %d\n", *ptr); // -->02
	ptr++;
	printf("3rd byte of a = %d\n", *ptr); // -->32????
	ptr++;
	printf("4rd byte of a = %d\n", *ptr); // -->16
	return 0;
}