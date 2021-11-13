#include <stdio.h>

int main(void)
{
	int adress = 0x00DDAABB,i,num;
	unsigned char byte;
	
	printf("started adrs: %x\n",adress);
	for (i = 0;i < 4;i++){
		num = i + 1;
		byte = (adress >> (i*8) & 0xff);
		printf("%dst byte of a = %x\n",num,byte); //побитовый вывод
	}
//dd: 11011101	
//cc: 11001100 / & 0x00ccffff
	
	adress = 0x00DDAABB;
	adress = adress & 0x00ccffff;
	printf("final adrs: %x\n",adress);

	return 0;
}