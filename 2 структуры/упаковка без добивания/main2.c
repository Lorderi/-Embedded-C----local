#include<stdio.h>
//структура с добиванием и без(упакованная) 

struct test1 {
	char a;
	int b;
};
struct test2 {
	char a;
	int b;
};//__attribute__((packed));

int main(void) {

	char str[10] = { 'A',0,0,0,0,'B',0,0,0,0 };
	//	struct test1 st1;
	//struct test2 st2;
	struct test1* ptr1;
	struct test2* ptr2;

	ptr1 = (struct test1*)&str[10];
	ptr2 = (struct test2*)&str[10];

	printf("test1 a = %d\n", ptr1->a);
	printf("test1 b = %d\n\n", ptr1->b);

	printf("test2 a = %d\n", ptr2->a);
	printf("test2 b = %d\n\n\n", ptr2->b);
	//или

	ptr1 = (struct test1*)&str[10];
	printf("test1 a = %d\n", *ptr1);
	ptr1++;
	printf("test1 b = %d\n\n", *ptr1);

	ptr2 = (struct test2*)&str[10];
	printf("test2 a = %d\n", *ptr2);
	ptr2++;
	printf("test2 b = %d\n", *ptr2);




	//	printf("1st block of test1: %p\n", *ptr1);
	//	printf("1st block of test2: %p\n", *ptr2);
	//	ptr1++;
	//	ptr2++;
	//	printf("2nd block of test1: %p\n", *ptr1);
	//	printf("2nd block of test2: %p\n", *ptr2);


		/////побайтово выводить через указатель?

	return 0;
}