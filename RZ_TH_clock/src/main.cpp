#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define WEATHER_URL "http://101.34.133.37:5000/get_weather_icon"
#define RZ_TH_CLOCK_SSID "RZ_TH_CLOCK"
#define REQUEST_CONFIG "config"
#define CONFIG_FILE "config.info"
#define RESET_BUTTON_NUM 12
#define TIMEZONE 8    // 东八区
#define DAYSAVETIME 0 // 使用夏令时为1
#define DHTPIN 2      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

WiFiServer server(80);                                       // 创建服务器
DHT dht(DHTPIN, DHTTYPE);                                    // 温湿度计初始化
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); // 定义OLED屏幕的分辨率

int init_flag = 0;
String weather_icon = String("");
int weather_wait = 7200;

const unsigned char PROGMEM sun[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x06,
    0x01, 0x00, 0xC0, 0x00, 0x00, 0x07, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x03, 0x87, 0x83, 0x80, 0x00,
    0x00, 0x01, 0x1F, 0xE3, 0x00, 0x00, 0x00, 0x00, 0x38, 0x70, 0x00, 0x00, 0x00, 0x00, 0x60, 0x18,
    0x00, 0x00, 0x00, 0x00, 0xC0, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x06, 0x00, 0x00, 0x00, 0x01,
    0x80, 0x03, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0x00, 0x00, 0x00, 0x79, 0x80, 0x03, 0x3C, 0x00,
    0x00, 0x79, 0x80, 0x03, 0x3C, 0x00, 0x00, 0x01, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x80, 0x06,
    0x00, 0x00, 0x00, 0x00, 0xC0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x60, 0x1C, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x38, 0x00, 0x00, 0x00, 0x01, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x03, 0x87, 0xC3, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x03, 0x80, 0x00, 0x00, 0x0E, 0x01, 0x01, 0xC0, 0x00, 0x00, 0x04, 0x03, 0x80,
    0xC0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char PROGMEM cloud[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x18, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x06,
    0x00, 0x00, 0x00, 0x01, 0xC0, 0x07, 0x00, 0x00, 0x00, 0x07, 0x80, 0x07, 0xC0, 0x00, 0x00, 0x1F,
    0x80, 0x01, 0xF0, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x30, 0x00, 0x00, 0x18, 0x00,
    0x00, 0x60, 0x00, 0x00, 0x0C, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x0E, 0x00, 0x00, 0xE0, 0x00, 0x00,
    0x0E, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x0E, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x60,
    0x00, 0x00, 0x0C, 0x00, 0x00, 0x30, 0x00, 0x00, 0x18, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00,
    0x00, 0x1F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char PROGMEM rain[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x70, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x07, 0x80, 0x00, 0x00, 0x07, 0xC0, 0x03,
    0xE0, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x70, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18,
    0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00,
    0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x0E, 0x3B, 0xB8, 0x70, 0x00, 0x00, 0x07, 0xBB, 0xBB, 0xC0, 0x00, 0x00, 0x01,
    0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xB8,
    0x00, 0x00, 0x00, 0x00, 0x3B, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * @description: write content to file
 * @param {char} *path (file path)
 * @param {char} *content (content need to write)
 * @return {int} 0: success, -1: failed
 */
int writeFile(const char *path, const char *content)
{
  int ret = 0;
  Serial.printf("Writing to file: %s", path);
  Serial.println();

  File file = LittleFS.open(path, "w");
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return -1;
  }
  if (file.print(content))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
    ret = -1;
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
  return ret;
}

/**
 * @description: append content to file
 * @param {char} *path  (file path)
 * @param {char} *content (content need to append)
 * @return {int} 0: success, -1: failed
 */
int appendFile(const char *path, const char *content)
{
  int ret = 0;

  Serial.printf("Appending to file: %s", path);
  Serial.println();

  File file = LittleFS.open(path, "a");
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return -1;
  }

  if (file.print(content))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
    ret = -1;
  }

  file.close();

  return ret;
}

/**
 * @description: delete file
 * @param {char} *path  (file path)
 * @return {int} 0: success, -1: failed
 */
int deleteFile(const char *path)
{
  Serial.printf("Deleting file: %s", path);
  Serial.println();

  if (LittleFS.remove(path))
  {
    Serial.println("File deleted");
    return 0;
  }
  else
  {
    Serial.println("Delete failed");
    return -1;
  }
}

/**
 * @description: read content from file
 * @param {char} *path  (file path)
 * @param {String} *content (content have read)
 * @return {int}  0: success, -1: failed
 */
int readFile(const char *path, String *content)
{
  Serial.printf("Reading file: %s", path);
  Serial.println();

  File file = LittleFS.open(path, "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return -1;
  }

  while (file.available())
  {
    char tmp = file.read();
    *content += tmp;
  }
  file.close();

  if (content->length() <= 0)
  {
    return -1;
  }

  return 0;
}

/**
 * @description: check file if exist
 * @param {char} *path  (file path)
 * @return {int} 0: exist, -1: do not exist
 */
int checkFile(const char *path)
{
  Dir root = LittleFS.openDir("/");
  while (root.next())
  {
    // Serial.println(root.fileName());
    if (strncmp(root.fileName().c_str(), path, strlen(path)) == 0)
    {
      return 0;
    }
  }
  return -1;
}

// str: GET /config?wifi_name=gean&wifi_pwd=13611506326 HTTP/1.1
int save_ssid_and_passwd(String str)
{
  String content = "";
  String ssid = str.substring(str.indexOf('=') + 1, str.indexOf('&'));
  String passwd = str.substring(str.lastIndexOf('=') + 1, str.lastIndexOf(' '));
  Serial.printf("ssid: %s, passwd: %s", ssid.c_str(), passwd.c_str());
  Serial.println();
  if (ssid.length() <= 0 && passwd.length() <= 0)
  {
    return -1;
  }

  LittleFS.format();
  Serial.println("Formatting LittleFS filesystem OK!");
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
    return -1;
  }
  Serial.println("Mount LittleFS");

  deleteFile(CONFIG_FILE);
  if (writeFile(CONFIG_FILE, ssid.c_str()))
  {
    return -1;
  }
  if (appendFile(CONFIG_FILE, "\n"))
  {
    return -1;
  }
  if (appendFile(CONFIG_FILE, passwd.c_str()))
  {
    return -1;
  }
  if (readFile(CONFIG_FILE, &content))
  {
    return -1;
  }
  Serial.println(content);

  LittleFS.end();

  return 0;
}

bool getLocalTime(struct tm *info, uint32_t ms)
{
  uint32_t count = ms / 10;
  time_t now;

  time(&now);
  localtime_r(&now, info);

  if (info->tm_year > (2016 - 1900))
  {
    return true;
  }

  while (count--)
  {
    delay(10);
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900))
    {
      return true;
    }
  }
  return false;
}

int http_get(const char *url, String *ret_str)
{
  int ret = 0;
  HTTPClient http_client;
  WiFiClient wifi_client;

  http_client.begin(wifi_client, String(url));
  Serial.print("URL: ");
  Serial.println(url);

  //启动连接并发送HTTP请求
  int http_code = http_client.GET();
  if (http_code == HTTP_CODE_OK)
  {
    *ret_str = http_client.getString();
    Serial.print("URL GET: ");
    Serial.println(ret_str->c_str());
  }
  else
  {
    Serial.print("URL GET ERR: ");
    Serial.println(http_code);
    ret = -1;
  }

  wifi_client.stop();
  http_client.end();

  return ret;
}


void setup()
{
  Serial.begin(115200);

  init_flag = 0;
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
  }
  else
  {
    Serial.println("Mount LittleFS");
    if (checkFile(CONFIG_FILE) == 0)
    {
      String content = "";
      if (readFile(CONFIG_FILE, &content))
      {
        Serial.println("Get ssid and passwd from file failed");
      }
      else
      {
        String ssid = content.substring(0, content.indexOf('\n'));
        String passwd = content.substring(content.indexOf('\n') + 1);
        Serial.printf("ssid: %s, passwd: %s", ssid.c_str(), passwd.c_str());
        Serial.println();

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), passwd.c_str()); // 连接路由器
        while (WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.print(".");
        }
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        init_flag = 1;
        LittleFS.end();

        Serial.println("Contacting Time Server");
        configTime(3600 * TIMEZONE, DAYSAVETIME, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org"); // 配置时间服务器

        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 设置OLED的I2C地址
        display.setTextColor(SSD1306_WHITE);       // 设置字体颜色

        dht.begin(); // 开启温湿度计
      }
    }
  }
  if (!init_flag)
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(RZ_TH_CLOCK_SSID); // 建立热点
    server.begin();                // 开启服务器
  }
  else
  {
    pinMode(RESET_BUTTON_NUM, INPUT); // 重置按钮
  }
}

void loop()
{
  int ret = 0;

  if (!init_flag)
  {
    WiFiClient client = server.available(); // 获取客户端对象
    if (!client)
    {
      return;
    }

    while (!client.available()) // 等待客户端连接
    {
      delay(1);
    }

    String request_str = client.readStringUntil('\r');                  // 读取客户端发来的请求
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"); // 收到请求回复200

    Serial.printf("Get: %s", request_str.c_str());
    Serial.println();

    if (strstr(request_str.c_str(), REQUEST_CONFIG) != NULL) // 如果请求URL: IP/config
    {
      Serial.printf("ENTER: %s", REQUEST_CONFIG);
      Serial.println();

      if (request_str[11] == '?') // 提交了配网信息
      {                           // GET /config?wifi_name=gean&wifi_pwd=13611506326 HTTP/1.1
        ret = save_ssid_and_passwd(request_str);
        if (ret == 0)
        {
          Serial.println("Have saved ssid and passwd.");
          String ret_str = "Your device's mac is " + WiFi.softAPmacAddress() + " <br>Please reboot now.";
          client.print(ret_str); // 打印MAC，用于日后与账号绑定
          return;
        }
        else
        {
          client.print("Get ssid or passwd err.");
        }
      }
      else if (request_str[11] == ' ') // 直接访问
      {                                // GET /config HTTP/1.1
        client.print(" \
        <!DOCTYPE html> \
        <html lang=\"en\"> \
          <head> \
            <meta charset=\"UTF-8\"> \
            <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \
            <meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"> \
            <title>配置页面</title> \
          </head> \
          <body> \
            <form action=\"/config\" method=\"get\"> \
              <div class=\"form-item\"> \
                <label for=\"wifi_name\">Wifi 名称:</label> \
                <input type=\"text\" name=\"wifi_name\"> \
              </div> \
              <div class=\"form-item\"> \
                <label for=\"wifi_pwd\">Wifi 密码：</label> \
                <input type=\"password\" name=\"wifi_pwd\"> \
              </div> \
              <div class=\"form-control\"> \
                <button type=\"submit\">提交</button> \
                <button type=\"reset\">清空</button> \
              </div> \
              </form> \
            </body> \
          </html>");                   // 配网网页
      }
      else
      {
        client.print("Invalid parameter.");
      }
    }
    else
    {
      client.print("NOT FOUND");
    }
  }
  else
  {
    display.clearDisplay(); // 清空屏幕

    // 获取时间
    struct tm tmstruct;
    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct, 5000);

    char hms[32] = {0};
    snprintf(hms, 32, "%02d:%02d:%02d", tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

    char md[32] = {0};
    snprintf(md, 32, "%02d/%02d", (tmstruct.tm_mon) + 1, tmstruct.tm_mday);

    Serial.print(hms);
    Serial.print(" ");
    Serial.println(md);

    display.setTextSize(2);  // 设置字体大小2 高12
    display.setCursor(32, 0); // 设置开始显示文字的坐标
    display.println(hms);

    display.setTextSize(1);   // 设置字体大小1 高6
    display.setCursor(96, 16); // 设置开始显示文字的坐标
    display.println(md);

    // 获取天气
    if(weather_wait >= 7200){  // 每小时更新一次
      http_get(WEATHER_URL, &weather_icon);
      weather_wait = 0;
    }
    weather_wait = weather_wait + 1;
    if (weather_icon == "rain")
    {
      display.drawBitmap(8, 32, rain, 48, 32, WHITE);
    }
    else if (weather_icon == "sun")
    {
      display.drawBitmap(8, 32, sun, 48, 32, WHITE);
    }
    else
    {
      display.drawBitmap(8, 32, cloud, 48, 32, WHITE);
    }

    // 获取温湿度
    float h = dht.readHumidity();    // 湿度
    float t = dht.readTemperature(); // 温度

    if (isnan(h) || isnan(t))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    String temperature = String(t);
    temperature += " C";
    Serial.println(temperature);
    String humidity = String(h);
    humidity += " %";
    Serial.println(humidity);

    display.setTextSize(1);    // 设置字体大小
    display.setCursor(64, 40); // 设置开始显示文字的坐标
    display.println(temperature);

    display.setTextSize(1);    // 设置字体大小
    display.setCursor(64, 48); // 设置开始显示文字的坐标
    display.println(humidity); // 输出的字符

    display.display(); // 使更改的显示生效

    // reset
    int reset_flag = digitalRead(RESET_BUTTON_NUM);
    if (reset_flag == 0)
    {
      Serial.println("Get reset cmd.");
      LittleFS.format();
      while (1)
      {
        delay(100);
      }
    }

    delay(500);
  }
}