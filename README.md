# InterestWorks
Some product made by myself.
文件名 | 功能
:----- | :-----
ToDesktop/win_d.cpp | 通过windows下socket远程模拟win+d回到桌面
ToDesktop/getRequest.py | flask实现接收请求，调用toDesktop程序
ToDesktop/toDesktop.c | 模拟windows下win+d回到桌面
ToDesktop/toDesktop.ino | esp8266远程发送回到桌面请求给电脑
SmallKeyboard/keyboard.ino | arduino实现的小键盘
SmallKeyboard/smallKey.ino | 可根据串口信息执行操作的小键盘
SmallKeyboard/serialSend.ino | 通过串口转发http请求的内容
AutoSwitch/auto_switch.ino | esp8266远程控制物理开关
SmallCar/dianji.c | 树莓派通过wiring控制电机
SmallCar/duoji.c | 树莓派通过wiring控制舵机
SmallCar/http.c | 树莓派通过socket接收http请求
RZ_TH_clock | esp8266时间同步天气温湿度计
