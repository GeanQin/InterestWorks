#include <Windows.h>

int main(void){
	keybd_event(0x5B,0,0,0);
	keybd_event(0x44,0,0,0);
	keybd_event(0x44,0,KEYEVENTF_KEYUP,0);
	keybd_event(0x5B,0,KEYEVENTF_KEYUP,0);
	return 0;
}