#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char APPasswd[] = "12345678";
const char APName[] = "to_desktop";
int reqSwhich=0;
bool out;
String ip="";
WiFiClient client;
HTTPClient http;

WiFiServer server(80);

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

void setup() 
{
  Serial.begin(9600);
  pinMode(4,INPUT);
  setupAP();
  server.begin();
}

void loop() 
{
  if(reqSwhich==0){
    WiFiClient client = server.available();//检测是否有客户端连接
    if(!client){
      return;
    }
    while(!client.available()){//等待客户端数据
      delay(1);
    }
    String req=client.readStringUntil('\r');
    int x=findIndex(req, ' ', 0);
    int y=findIndex(req, ' ', x+1);
    String str=req.substring(x+2,y);
    int z=findIndex(str, '&', 0);
    int w=findIndex(str, '&', z+1);
    if(z==str.length()){
      client.print("HTTP/1.1 200 OK\r\n\r\n");
      client.print("error");
      return;
    }
    String ssid=str.substring(0,z);
    String password=str.substring(z+1,w);
    ip=str.substring(w+1);
    setupSTA(ssid,password);
    client.print("HTTP/1.1 200 OK\r\n\r\n");
    client.println(WiFi.localIP());
    client.println(ip);
    reqSwhich=1;
  }
  out=digitalRead(4);
  if(out==HIGH){
    Serial.println("High");
    if(http.begin(client, "http://" + ip + ":5000/d")){
      int httpCode = http.GET();
      http.end();
    }
    delay(15000);
  }else{
    Serial.println("low");
  }
}
