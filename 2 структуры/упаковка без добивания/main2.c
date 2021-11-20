#include<stdio.h>
//структура с добиванием и без(упакованная) 

struct test1 {
	char a;
	int b;
};
struct test2 {
	char a;
	int b;
} __attribute__((packed));

int main(void) {

	char str[10] = { 'A',0,0,0,0,'B',0,0,0,0 }; //0.66.0.0
	//	struct test1 st1;
	//struct test2 st2;
	struct test1* ptr1;;
	struct test2* ptr2;;

	ptr1 = (struct test1*)str;   //-& и -[10]
	ptr2 = (struct test2*)str; //изменение +1
	printf("test1 - добивание\ntest2 - упакованный\n\n");
	printf("ptr0:\n\ntest1 a = %x\n", ptr1->a); //a=41
	printf("test1 b = %x\n\n", ptr1->b); //b=4200

	printf("test2 a = %x\n", ptr2->a); //a=41
	printf("test2 b = %x\n\n\n", ptr2->b); //b=0, т.к. без добивания он до b не дошел
	
	ptr1++;
	ptr2++;
	
	printf("ptr++:\n\ntest1 a = %x\n", ptr1->a); //a=0, т.к. ещё не дошел до b
	printf("test1 b = %x\n\n", ptr1->b); //b=рандом т.к. выходит за пределы

	printf("test2 a = %x\n", ptr2->a); //a=42
	printf("test2 b = %x\n\n\n", ptr2->b); //b=0, т.к. без добивания за рамки массива не вышел

//	ptr11 = (struct test1*)str;   //-& и -[10]
//	ptr12 = (struct test1*)str + 1; //изменение +1
//	ptr21 = (struct test2*)str;   //-& и -[10]
//	ptr22 = (struct test2*)str + 1; //изменение +1

//	printf("test1 a = %p\n", ptr11);
//	printf("test1 b = %p\n\n", ptr11 + 1);

//	printf("test2 a = %p\n", ptr21);
//	printf("test2 b = %p\n\n\n", ptr21 + 1);


//	ptr1 = (struct test1*)&str[10];
//	printf("test1 a = %d\n", *ptr1);
//	ptr1++;
//	printf("test1 b = %d\n\n", *ptr1);

//	ptr2 = (struct test2*)&str[10];
//	printf("test2 a = %d\n", *ptr2);
//	ptr2++;
//	printf("test2 b = %d\n", *ptr2);




	//	printf("1st block of test1: %p\n", *ptr1);
	//	printf("1st block of test2: %p\n", *ptr2);
	//	ptr1++;
	//	ptr2++;
	//	printf("2nd block of test1: %p\n", *ptr1);
	//	printf("2nd block of test2: %p\n", *ptr2);


		/////побайтово выводить через указатель?
		
		
		

	return 0;
}

