## ESP8266远程控制 电子门 

### 最前面介绍：

##### 	这是一个使用ESP8266 联网控制继电器，实现手机远程控制电子门，打开关闭，开关一次的物联网联手小项目

​	

三个周末，学习了下ESP8266 基础使用，做了个小玩具

​	实现手机端控制电子大门的开关，再也不用担心没带钥匙进不去大门了 （掩面）

​	使用场景：电子门支持红外遥控器，有时忘记遥控或门禁卡，只能呼叫同事开门，遂想把遥控开关放到手机里面

### 结构图说明：

![image-20220522185153483](Esp8266远程控制 电子门.assets/image-20220522185153483.png)

### 软硬说明：

​	硬件：ESP8266 主板,控制继电器，继电器连接红外遥控器

​	前端：Flutter 搭的Demo （前后用了半天，页面太粗）

​	后端：接口使用 FastAPI +Python ，数据库：MySql 

### 逻辑说明：

ESP8266 主要功能在`door.ino` 文件

​	setup() 主要做 连接wifi初始化 

​	loop() 函数循环请求服务器，查询最新一条命令，如果命令状态为1，表述需要执行，按照命令执行，执行完毕，更新数据状态为0，

​	执行命令依赖字段ACTION_NAME， 如A 则执行A 命令，A命令对应设置 A引脚低电平，等待200毫秒后，重置高电平

​	命令ABCD 和引脚的对应关系参见 ，

```
const uint8_t PORT_A = D1; // 对应引脚
const uint8_t PORT_B = D2; //
const uint8_t PORT_C = D6; //
const uint8_t PORT_D = D7; //`
```

​	服务端，使用FastApi 搭建三个接口，使用Docker部署

- addnasdaq  新建命令，客户端调用，新增一条命令
- querylast 查询最新一条命令，ESP8266查询是否有命令指定调用
- updatenasdaq 更新指定命令状态 ，ESP8266执行完毕命令调用

数据库使用MySql，数据库地址和参数在请 DbConfig.py文件配置



以上介绍完毕，如果你还想动手试试，参照一下步骤

### 前提说明：

​	如果要想将此教程跑起来，需要一些预备知识：

1. 了解ESP8266 开发环境搭建，基础的开发和上传步骤

   需要修改参数，调整wifi信息，服务器地址

   在此推荐太极创客 的基础教程，我看教程2个周末完成Esp部分。

   [物联网基础教程](http://www.taichi-maker.com/homepage/arduino-basic-tutorial-index/)，[ESP8266物联网教程](http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/)

2. 了解Docker基础，参照教程可以运行实例

   需要设置MySql地址和服务器地址

   Docker教程参见[B站教程](https://www.bilibili.com/video/BV1og4y1q7M4?spm_id_from=333.999.0.0)

   [FastAPI 部署教程](https://blog.csdn.net/weixin_42493346/article/details/105854898)

3. AndroidStudio使用，可以运行Flutter项目，也可以下载打包好的项目

   项目源码包含客户端源码，我运行过Android，未测试IOS，代码很简单，了解Dart基础语法就可以修改

   [Flutter教程](https://book.flutterchina.club/chapter1/dart.html)

### 如何使用

#### 准备材料：

- 无线遥控器，支持学习和对拷贝，
- ESP8266 主板
- 3.3V四路继电器
- 电源，这里使用3节1.5V 电池替代
- 工具类，电烙铁，万能表，焊锡丝，热熔枪，
- 服务器（之前就有），如果使用三方服务，请跳过
- 耐心



##### 遥控器改造

我是使用ESP8266控制遥控器，需要对遥控器改造

将 遥控器 ABCD 开关分别使用电烙铁 ，焊接处四条引线，后续接入 继电器

其中，负极接地是通用一根，参考图：

![image-20220522203020354](Esp8266远程控制 电子门.assets/image-20220522203020354.png)



如果接其他控制器，请参照类似方式，其实就是使用继电器将原本开关短路



##### ESP8266环境搭建

​	esp8266端代码在muc目录，`door.ino`

​	这里我参考的太极创客教程，物联网硬件开发0基础，学了两个周末，十分推荐，表示感谢

​	再次贴一下地址[ESP8266开发环境搭建教程地址](http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/nodemcu-arduino-ide/)

​	准备好环境，跑一下示例中的Blink代码，正常运行就表述环境搭建成功

​	在ESP8266 可以运行Demo后，可以导入`door.ino`文件，修改配置

​	注意需要修改的位置:

- wifi账号密码 

  `const char *ssid = "your_wifi_name";      // 需要连接到的WiFi名`

  `const char *password = "88888888"; // 连接的WiFi密码`

- 服务器地址和端口  

  `const char *host = "101.xx.xxx.xxx";`

  `const int httpPort = 8000;` 	

  

##### ESP8266连接继电器和红外开关

​	使用面包线用继电器将ESP8266和红外开关如图连接

​	注意ESP8266开发期间使用USB供电，输出3.3V给继电器供电，继电器接收到ESP8266的低电平会将两个触电短路，达到开关闭合的效果

​	将继电器负极并联，四路输出如图连接开关四个开关，注意连接是常闭的两个接口，不确定哪两个常闭就用万能表测试下

​	注：连接完毕之后，可以使用door.ino代码进行测试，看是否可以实现继电器的控制，如果控制正确，再使用网络请求



##### 服务器部署

​	服务端代码可以直接Docker部署到服务器，也可以本地部署，局域网测试

​	服务端代码目录在 services目录

​	注意需要修改的地方：

​	MySql 地址和 端口，

```
ipname = "101.35.199.224"
duankou = 3306
DB_NAME = "mcu"
userName = "mcu"
pwd = "3sYxS8MaZYKNNt23"
```

​	数据库初始化文件：nasdaq.sql

FastAPI本地部署：

​	项目services目录导入到Pycharm，直接运行，

FastAPI服务端部署

​	[参见Docker部署FastAPI](https://blog.csdn.net/weixin_42493346/article/details/105854898)



##### 客户端打包

​	客户端采用Flutter方案，目前就1个接口请求，我不想些两端代码（懒-_-!!）

​	配置好AndroidStudio的Flutter开发环境，直接导入就可以，配置教程可以参见[搭建Flutter开发环境](https://book.flutterchina.club/chapter1/install_flutter.html)

​	需要修改对应的服务器地址：`post('http://101.xxx.xxx.xxx:8000/addnasdaq',`

​	

##### 测试：

1. 先测试ESP8266连接WIFI是否正常
2. 测试ESP8266单独控制继电器是否正常
3. 本地部署服务器，使用PostMan接口测试数据库 增加，查询是否正常
4. 测试ESP8266 连接数据是否正常
5. 如果到此测试正常，标识项目正常运行



项目声明：此项目是学习练手项目，仅用于个人学习，

刚接触物联网硬件，项目中难免有不规范的地方，仅供学习，欢迎大家贡献代码





目前已经实现的功能

- [x] 支持远程控制
- [x] 支持 四个按键控制，可以扩展
- [x] 支持Android / IOS 两端客户端 Flutter实现，界面待优化

TODO

- [ ] wifi连接配置

- [ ] 按键自定义配置

- [ ] 接口优化，目前是Json有点大，待优化

- [ ] 自动化部署

- [ ] 教程优化

  

  







