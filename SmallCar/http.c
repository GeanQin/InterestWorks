#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void qSqlit(char *ret,char *buf,int x,int y);
int qSearch(char *buf,char c,int x);

int main(){
        //创建套接字
        int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        //将套接字和IP、端口绑定
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
        serv_addr.sin_family = AF_INET;  //使用IPv4地址
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //具体的IP地址
        serv_addr.sin_port = htons(1234);  //端口
        bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        //进入监听状态，等待用户发起请求
        listen(serv_sock, 20);
        while(1){
                //接收客户端请求
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_size = sizeof(clnt_addr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

                FILE *cin = fdopen(clnt_sock, "r");
                setbuf(cin, (char *) 0);
                char buf[1024];
                fgets(buf, 1024, cin); //读取第一行

		int x=qSearch(buf,'/',0);
		int y=qSearch(buf,' ',x);
		char s[1024];
		qSqlit(s,buf,x+1,y-1);
		printf("\n%s\n",s);

                char res[1024];
                char str[] = "HTTP/1.1 200 OK\r\nServer: PI\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s";
                sprintf(res,str,strlen(buf),buf);
		int pid=fork();
		if(pid==0){
			if(s[0]=='q'||s[0]=='h'||s[0]=='t'){
                		execl("/home/pi/workspace/car/dianji","dianji",s, NULL);
			}else if(s[0]=='y'||s[0]=='z'||s[0]=='x'){
				execlp("sudo", "sudo", "./duoji", s, NULL);
			}
		}
                write(clnt_sock, res, sizeof(res));
                //关闭套接字
                close(clnt_sock);
        }
        close(serv_sock);
        return 0;
}

void qSqlit(char *ret,char *buf,int x,int y){
        int i=0;
        int n=y-x+1;
        int m=x-1;
        for(i=0;i<n;i++){
                ret[i]=buf[m];
                m=m+1;
        }
        ret[n]='\0';
}

int qSearch(char *buf,char c,int x){
        int n,j;
        int i=strlen(buf);
        for(j=x-1;j<i;j++){
                if(buf[j]==c){
                        n=j+1;
                        break;
                }
        }
        return n;
}
