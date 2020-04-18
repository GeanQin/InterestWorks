#include<stdio.h>
#include<wiringPi.h>

int main(int argc,char* argv[]){
	wiringPiSetupGpio();

	int direction;
	if(argv[1][0]=='y'){
		direction=135;
	}else if(argv[1][0]=='z'){
		direction=180;
	}else{
		direction=158;
	}
	printf("argv[1][0]=%c,direction=%d\n",argv[1][0],direction);

	pinMode(18, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(2000);
	pwmSetClock(192);
	pwmWrite(18,direction);
	return 0;
}
