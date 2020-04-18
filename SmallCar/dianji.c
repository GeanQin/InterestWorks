#include<wiringPi.h>
#include<stdio.h>

int main(int argc,char* argv[]){

	wiringPiSetup();
	pinMode (25, OUTPUT);
	pinMode (0, OUTPUT);

	if(argv[1][0]=='q')
	{
		printf("qian\n");
		digitalWrite(0, LOW);
		digitalWrite(25, HIGH);
	}
	else if(argv[1][0]=='h')
	{
		printf("hou\n");
		digitalWrite(25, LOW);
		digitalWrite(0, HIGH);
	}
	else
	{
		printf("tin\n");
		digitalWrite(25, LOW);
               	digitalWrite(0, LOW);
	}
	return 0;
}
