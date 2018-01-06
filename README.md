# Intelligent desk lamp
智能台灯系统
==
## 准备
智能台灯系统基于Arduino平台，采用两块ATmega8主控芯片，综合运用物联网技术、传感技术。
需要使用[贝壳物联](https://www.bigiot.net/)注册一个账号，获得设备id和APIKEY
将获得id和apikey填入master主机代码中对应位置

## 功能
实现自动调光功能，人来灯亮，人走灯灭（热释电传感
器），并具有坐姿提醒功能，当使用者距离台灯太近时，
台灯能及时产生提示。<br>
能够实时显示时间和日期，并具有闹铃提醒功能。<br>
能够检测环境温度和电池电量，并实时显示，在电量不
足时能发出提示。<br>
实现台灯定时开关。<br>
可实现远程控制台灯亮灭，并能对亮度进行调整。<br>
可实现对光的切换，如白光切换成暖光。<br>
上述提醒功能均为语音提示，开关为触摸开关。<br>

## 电路图
![](https://github.com/Villivateur/Smart_Lamp_Programs/blob/master/Diagram.BMP)


## 使用方法
在贝壳物联客户端输入5位数字
10XXX   调节冷光亮度，XXX从0到255<br>
20XXX   调节暖光亮度，XXX从0到255<br>
3XXXX   同时调节冷光和暖光，XXX从0到255<br>
4XXXX   定时打开台灯，XXXX为24小时制时间<br>
5XXXX   定时关闭台灯，XXXX为24小时制时间<br>
6XXXX   闹钟功能，XXXX为24小时制时间<br>
7000X   人体感应功能，X为1表示打开，0为关闭<br>

## 3D模型
台灯全部采用3D打印，打印文件于stl压缩包中
![](https://github.com/h1997l1997/Intelligent-desk-lamp/blob/master/base%20v7.png)

欢迎star本项目，同时follow本账户
---
欢迎访问[我的博客](http://www.h1997l1997.cn/) 
---
