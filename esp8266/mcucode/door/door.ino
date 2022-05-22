/**********************************************************************
项目名称/Project          : 远程控制电子门遥控器
程序名称/Program name     : door.ino
团队/Team                : yaron.wong
作者/Author              : yaron.wong
日期/Date（YYYYMMDD）     : 2020.05.22
程序目的/Purpose          :
这是一个使用ESP8266 联网控制继电器，实现手机远程控制电子门，打开关闭，开关一次的物联网联手小项目



***********************************************************************/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char *host = "101.xx.xxx.xxx"; // 网络服务器地址
const int httpPort = 8000;           // http端口80

const int EXE_TIME = 500; // http端口80

const char *ACTION_ON = "1";  // 可执行
const char *ACTION_OFF = "0"; //不可执行

const char *ACTION_A = "A"; //执行A
const char *ACTION_B = "B"; //执行B
const char *ACTION_C = "C"; //执行C
const char *ACTION_D = "D"; //执行D
const char *ACTION_E = "E"; //执行E

const uint8_t PORT_A = D1; // 对应引脚
const uint8_t PORT_B = D2; //
const uint8_t PORT_C = D6; //
const uint8_t PORT_D = D7; //

#define OPEN = HIGH
#define CLOSE = LOW

String querylast = "/querylast";       // 查询最新地址
String updatenasdaq = "/updatenasdaq"; // post地址

// 设置wifi接入信息(请根据您的WiFi信息进行修改)

const String ssid = "you_wifi_name";      // 需要连接到的WiFi名
const String password = "88888888"; // 连接的WiFi密码



void setup()
{
    //初始化串口设置
    Serial.begin(9600);
    Serial.println("");

    //设置ESP8266工作模式为无线终端模式
    WiFi.mode(WIFI_STA);

    Serial.println(ssid);

    Serial.println(password);

    Serial.println("尝试联网中:");
    //开始连接wifi
    WiFi.begin(ssid, password);

    //等待WiFi连接,连接成功打印IP
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        Serial.print(WiFi.status() );

    }
    Serial.println("");
    Serial.println("WiFi Connected!");

    //设置低电位

    pinMode(PORT_A, OUTPUT);
    pinMode(PORT_B, OUTPUT);
    pinMode(PORT_C, OUTPUT);
    pinMode(PORT_D, OUTPUT);

    digitalWrite(PORT_A, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(PORT_B, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(PORT_C, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(PORT_D, HIGH); // turn the LED on (HIGH is the voltage level)
}

// 测试继电器
void testLoop()
{
    digitalWrite(PORT_A, LOW); // turn the LED on (LOW is the voltage level)
    delay(3000);
    digitalWrite(PORT_A, HIGH); // turn the LED on (LOW is the voltage level)

    digitalWrite(PORT_B, LOW); // turn the LED on (LOW is the voltage level)
    delay(3000);
    digitalWrite(PORT_B, HIGH); // turn the LED on (LOW is the voltage level)

    digitalWrite(PORT_C, LOW); // turn the LED on (LOW is the voltage level)
    delay(3000);
    digitalWrite(PORT_C, HIGH); // turn the LED on (LOW is the voltage level)

    digitalWrite(PORT_D, LOW); // turn the LED on (LOW is the voltage level)
    delay(3000);
    digitalWrite(PORT_D, HIGH); // turn the LED on (LOW is the voltage level)

    delay(3000);
}

void loop()
{

    // testLoop();

    testwifimac();

    runLoopTask();
}

void testwifimac()
{

    uint8_t macAddr[6];

    WiFi.macAddress(macAddr);

    Serial.println("获取MAC地址");

    Serial.printf("用转存到数组的方式获取MAC地址: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    // MAC地址会储存在这个数组里面

    //  delay(3000);
}

// 执行 查询命令 执行 修改 任务
// 这是主要任务
void runLoopTask()
{
    String querylastRespon = wifiClientRequestGET(querylast);
    if (querylastRespon != "")
    {
        Serial.println("请求成功！！");
        Serial.println(querylastRespon);

        // 解析json
        presJosn(querylastRespon);
    }
    else
    {
        Serial.println("请求失败！！");
        Serial.println(querylastRespon);
    }
}

// 向服务器发送HTTP请求
String wifiClientRequestPost(String postUrl, String postData)
{

    // 建立WiFi客户端对象，对象名称client
    WiFiClient client;

    // 建立字符串，用于HTTP请求
    String postRequest = (String)("POST ") + postUrl + " HTTP/1.1\r\n" +
                         "accept: application/json\r\n" +
                         "User-Agent: BuildFailureDetectorESP8266\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Host: " + host + ":" + httpPort + "\r\n" +
                         "Accept-Encoding: " + "gzip, deflate, br" + "\r\n" +
                         "Content-Length: " + postData.length() + "\r\n" +
                         "Connection: Keep Alive\r\n\r\n";

    // 通过串口输出连接服务器名称以便查阅连接服务器的网址
    Serial.print("Connecting to ");
    Serial.print(host);

    String responStr = "";

    // 连接网络服务器，以下段落中的示例程序为本程序重点1
    if (client.connect(host, httpPort))
    {
        Serial.println(" Success!"); // 连接成功后串口输出“Success”信息

        client.print(postRequest); // 向服务器发送HTTP请求

        Serial.println("Sending postrequet: "); // 通过串口输出HTTP请求信息内容以便查阅
        Serial.println(postRequest);            // 通过串口输出HTTP请求信息内容以便查阅

        client.print(postData); // 向服务器发送POST 请求

        Serial.println(postData);

        Serial.println("Web Server Response:");

        // 获取并显示服务器响应状态行
        String status_response = client.readStringUntil('\n');
        Serial.print("status_response: ");
        Serial.println(status_response); // 一般是HTTP/1.1 200 OK

        // 使用find跳过HTTP响应头
        if (client.find("\r\n\r\n"))
        {
            Serial.println("Found Header End. Start Parsing.");
        }

        //读取json
        responStr = readBodyInfo(client);

        client.stop();                      // 断开与服务器的连接
        Serial.print("Disconnected from "); // 并且通过串口输出断开连接信息
        Serial.print(host);
        Serial.print(postRequest);
    }
    else
    { // 如果连接不成功则通过串口输出“连接失败”信息
        Serial.println(" connection failed!");
        client.stop();
    }

    // Serial.println("return:");
    // Serial.println(responStr);

    return responStr;
}

// 向服务器发送HTTP请求
String wifiClientRequestGET(String queryUrl)
{
    // 建立WiFi客户端对象，对象名称client
    WiFiClient client;

    // 建立字符串，用于HTTP请求
    String postRequest = (String)("GET ") + queryUrl + " HTTP/1.1\r\n" +
                         "accept: application/json\r\n" +
                         "User-Agent: BuildFailureDetectorESP8266\r\n" +
                         "Content-Type: text/html;charset=utf-8\r\n" +
                         "Host: " + host + ":" + httpPort + "\r\n" +
                         "Connection: Keep Alive\r\n\r\n";

    // 通过串口输出连接服务器名称以便查阅连接服务器的网址
    Serial.print("Connecting to ");
    Serial.print(host);

    String responStr = "";

    // 连接网络服务器，以下段落中的示例程序为本程序重点1
    if (client.connect(host, httpPort))
    {
        Serial.println(" Success!"); // 连接成功后串口输出“Success”信息

        client.print(postRequest); // 向服务器发送HTTP请求

        Serial.println("Sending request: "); // 通过串口输出HTTP请求信息内容以便查阅
        Serial.println(postRequest);

        Serial.println("Web Server Response:");

        // 获取并显示服务器响应状态行
        String status_response = client.readStringUntil('\n');
        Serial.print("status_response: ");
        Serial.println(status_response); // 一般是HTTP/1.1 200 OK

        // 使用find跳过HTTP响应头
        if (client.find("\r\n\r\n"))
        {
            Serial.println("Found Header End. Start Parsing.");
        }

        //读取json
        responStr = readBodyInfo(client);

        client.stop();                      // 断开与服务器的连接
        Serial.print("Disconnected from "); // 并且通过串口输出断开连接信息
        Serial.print(host);
        Serial.print(postRequest);
    }
    else
    { // 如果连接不成功则通过串口输出“连接失败”信息
        Serial.println(" connection failed!");
        client.stop();
    }

    // Serial.println("return:");
    // Serial.println(responStr);

    return responStr;
}

String readBodyInfo(WiFiClient client)
{
    String read = "";
    while (client.connected() || client.available())
    {
        if (client.available())
        {
            String line = client.readStringUntil('\n');
            read += line;
            // Serial.println(line);
        }
    }
    return read;
}

void presJosn(String jsonStr)
{

    Serial.println("开始解析JSON！");
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    if (error)
    {
        Serial.println(F("解析JSON 失败 "));
        Serial.println(error.f_str());
        return;
    }

    Serial.print(F("解析JSON  完毕 "));
    const char *action_name = doc["action_name"];     // "E"
    const char *action_status = doc["action_status"]; // "1"
    const char *create_user = doc["create_user"];     // "张三"
    const char *mcu_ip = doc["mcu_ip"];               // "192.168.1.1"
    int id = doc["id"];                               // 23
    const char *update_time = doc["update_time"];     // "2022-05-17 10:23:58"
    const char *creat_ip = doc["creat_ip"];           // "123.118.78.34"\

    Serial.println(("开始打印JSON "));

    Serial.println(action_name);
    Serial.println(action_status);
    Serial.println(create_user);
    Serial.println(mcu_ip);
    Serial.println(id);
    Serial.println(update_time);
    Serial.println(creat_ip);

    Serial.print("根据 action_status 是否执行 ：");

    Serial.println(*action_status);

    Serial.println(*ACTION_ON);

    Serial.println(*action_status == *ACTION_ON);

    if (*action_status == *ACTION_ON)
    {
        // 根据action_name 执行不同的动作
        Serial.print("判断执行哪个分支！！");

        if (*action_name == *ACTION_A)
        {

            Serial.print("执行 ACTION_A");
            digitalWrite(PORT_A, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_A, HIGH); //
        }
        else if (*action_name == *ACTION_B)
        {
            Serial.print("执行 ACTION_B");
            digitalWrite(PORT_B, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_B, HIGH); //
        }
        else if (*action_name == *ACTION_C)
        {
            Serial.print("执行 ACTION_C");
            digitalWrite(PORT_C, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_C, HIGH); //
        }
        else if (*action_name == *ACTION_D)
        {
            Serial.print("执行 ACTION_D");
            digitalWrite(PORT_D, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_D, HIGH); //
        }
        else if (*action_name == *ACTION_E)
        {

            Serial.print("执行 ACTION_E");
            // 打开门一次  ，先常开 再自动
            digitalWrite(PORT_B, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_B, HIGH); //

            // 等待5秒
            delay(5000);

            digitalWrite(PORT_D, LOW); //
            delay(EXE_TIME);
            digitalWrite(PORT_D, HIGH); //
        }

        // 执行完毕 更新数据库
        String postData = "";
        StaticJsonDocument<192> doc;

        doc["id"] = id;
        doc["creat_ip"] = creat_ip;
        doc["action_status"] = ACTION_OFF;
        doc["create_user"] = create_user;
        doc["action_name"] = action_name;
        doc["mcu_ip"] = WiFi.localIP();

        // JSON 序列化
        serializeJson(doc, postData);

        wifiClientRequestPost(updatenasdaq, postData);
    }
    else if (action_status == ACTION_OFF)
    {
        // 命令状态关闭，无需处理
        Serial.print(" 查询到 命令状态为关闭 无需处理 ！！！！！！！ ");
    }

    // printfStr("nameStr = %s", action_name);
    // printfStr("action_status = %s", action_status);
    // printfStr("create_user = %s", create_user);
    // printfStr("mcu_ip = %s",  mcu_ip);
    // printfInt("id = %s", id);
    // printfStr("update_time = %s", update_time);
    // printfStr("creat_ip = %s", creat_ip);
}

void printfStr(char *str1, String str2)
{

    Serial.printf(str1, str2);
    Serial.println();
}

void printfInt(char *str1, int str2)
{

    Serial.printf(str1, str2);
    Serial.println();
}
