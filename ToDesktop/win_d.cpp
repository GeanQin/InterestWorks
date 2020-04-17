#include <winsock2.h>
#include <iostream>
#include <Windows.h>
 
#include <string.h>
using namespace std;
 
#pragma comment(lib, "ws2_32.lib")			//add ws2_32.lib
 
 
const int DEFAULT_PORT = 6666;

int qfindc(char *buf,char c,int from);
void qsplits(char *ret,char *buf,int x,int y);

int main(int argc,char* argv[])
{
 
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	wVersionRequested	=	MAKEWORD(2,2);//create 16bit data
//(1)Load WinSock
	err	=	WSAStartup(wVersionRequested,&wsaData);	//load win socket
	if(err!=0)
	{
		cout<<"Load WinSock Failed!";
		return -1;
	}
//(2)create socket
	SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,0);
	if(sockSrv == INVALID_SOCKET){
		cout<<"socket() fail:"<<WSAGetLastError()<<endl;
		return -2;
	}
//(3)server IP
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);//Auto IP, byte sequence change
	addrSrv.sin_port = htons(DEFAULT_PORT);
//(4)bind
	err = bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(err!=0)
	{
 
		cout<<"bind()fail"<<WSAGetLastError()<<endl;
		return -3;
	}
	//
//(5)listen
	err = listen(sockSrv,5);
	if(err!=0)
	{
 
		cout<<"listen()fail"<<WSAGetLastError()<<endl;
		return -4;
	}
	cout<<"Server waitting...:"<<endl;
//(6)client ip
	SOCKADDR_IN addrClt;
	int len = sizeof(SOCKADDR);
 
	while(1)
	{
//(7)accept
		SOCKET sockConn = accept(sockSrv,(SOCKADDR*)&addrClt,&len);

		char revData[255];
		int ret = recv(sockConn, revData, 255, 0);
		//printf(ret);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			int x=qfindc(revData,'/',1);
			int y=qfindc(revData,' ',x+1);
			char str[1024];
			qsplits(str,revData,x+1,y);
			//printf("x=%d,y=%d\n",x,y);
			printf(str);
			if(str[0]='d'){
				keybd_event(0x5B,0,0,0);
				keybd_event(0x44,0,0,0);
				keybd_event(0x44,0,KEYEVENTF_KEYUP,0);
				keybd_event(0x5B,0,KEYEVENTF_KEYUP,0);
			}
		}

		closesocket(sockConn);
	}
//(10)close server sock
	closesocket(sockSrv);
//(11)clean up winsock
	WSACleanup();
	return 0;
}

int qfindc(char *buf,char c,int from){
	int i,n;
	for(i=from-1;i<strlen(buf);i++){
		if(buf[i]==c){
			n=i+1;
			break;
		}
	}
	return n;
}

void qsplits(char *ret,char *buf,int x,int y){
	int i;
	int j=0;
	for(i=x-1;i<y;i++){
		ret[j]=buf[i];
		j=j+1;
	}
	ret[j]='\0';
}
