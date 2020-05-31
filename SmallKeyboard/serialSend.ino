#include <ESP8266WiFi.h>

const char APPasswd[] = "12345678";
const char APName[] = "my_keyboard";
int reqSwhich=0;

WiFiServer server(80);

void setup() 
{
  Serial.begin(9600);
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
    Serial.println(str);
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
