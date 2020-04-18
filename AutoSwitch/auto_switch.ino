#include <ESP8266WiFi.h>

const char APPasswd[] = "12345678";
const char APName[] = "auto_switch";
int reqSwhich=0;

WiFiServer server(80);

void setup() 
{
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  setupAP();
  server.begin();
}

void loop() 
{
  WiFiClient client = server.available();//检测是否有客户端连接
  if(!client){
    return;
  }
  while(!client.available()){//等待客户端数据
    delay(1);
  }
  String req=client.readStringUntil('\r');
  if(reqSwhich==0){
    int x=findIndex(req, ' ', 0);
    int y=findIndex(req, ' ', x+1);
    String str=req.substring(x+2,y);
    int z=findIndex(str, '&', 0);
    if(z==str.length()){
      client.print("HTTP/1.1 200 OK\r\n\r\n");
      client.print("error");
      return;
    }
    String ssid=str.substring(0,z);
    String password=str.substring(z+1);
    setupSTA(ssid,password);
    client.print("HTTP/1.1 200 OK\r\n\r\n");
    client.print(WiFi.localIP());
    reqSwhich=1;
  }else{
    int x=findIndex(req, ' ', 0);
    int y=findIndex(req, ' ', x+1);
    String str=req.substring(x+2,y);
    int angle=atoi(str.c_str());
    Serial.println(angle);
    for(int i=0;i<50;i++){
      servopulse(angle);  //引用脉冲函数
    }
    client.print("HTTP/1.1 200 OK\r\n\r\n");
    client.print("OK!");
  }
}
 
void setupAP(){
  WiFi.softAP(APName, APPasswd);
}

void setupSTA(String ssid,String password){
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
  }
}

int findIndex(String str, char c, int addr){
  int n;
  for(n=addr;n<str.length();n++){
    if(str[n]==c){
      break;
    }
  }
  return n;
}

void servopulse(int angle){
  int pulsewidth=(angle*11)+500;
  digitalWrite(2,HIGH);//将舵机接口电平至高
  delayMicroseconds(pulsewidth);//延时脉宽值的微秒数
  digitalWrite(2,LOW);//将舵机接口电平至低
  delayMicroseconds(20000-pulsewidth);
}
