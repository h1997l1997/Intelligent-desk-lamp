#include <aJSON.h>   
#include <dht11.h>                        //  dht11头文件
#include <SPI.h>                          //=======================
#include <Wire.h>                         //        Oled
#include <Adafruit_GFX.h>                 //       头文件
#include <Adafruit_SSD1306.h>             //=======================

#define LED_warm 5 // LED暖光正极连接针脚5
#define LED_cold 6 // LED冷光正极连接针脚6
#define Buzzer 7 //蜂鸣器
#define ADDRESS_DS1307 0x68 //I2C address
#define Button 2 //The Touching Button
#define Pyro 3 //热释电
//=============  BigIoT登录信息 ===========
String DEVICEID="3670"; //ID
String  APIKEY="31166021a"; //APIKEY
//=========================================

//================ BigIoT数据 =============
unsigned long lastCheckInTime = 0; //记录上次报到时间
const unsigned long postingInterval = 20000; // 每隔20秒向服务器报到一次
unsigned long lastUpdateTime = 0;//上次上传数据时间
String inputString = ""; //从BigIoT获取的数据
boolean stringComplete = false;
boolean CONNECT = true; 
char* parseJson(char *jsonString);
//=========================================

//========温度湿度=======
dht11 DHT11;
#define DHT11PIN 4
//=======================

//======== 外部时钟 =====
byte timeBcd[] = {0, 0, 0, 0, 0, 0, 0};
int Time[3]={0,0,0}; //hour,minute,second
//time = {year, month, date, day, hours, minutes, seconds};
//=======================

//====== Clock Time ====
int Clock[2]={24,61};
//======================

//== 全局计时 ==
int count1=0;
int count2=0; //发送数据
int count3=0; //选择温湿度发送
int count4=0; //热释电延时
//==============

//== 蜂鸣器开启 ==
bool BuzzerOpen=0;
//================

//=== Timing ====
int Timing_Open[2]={24,60};
int Timing_Close[2]={24,60};
//===============

//=== LED Control ====
int LED_warm_power=0;
int LED_cold_power=0;
//====================

//==== Battery Voltage =====
float Battery=0.0;
//==========================

void setup() 
{
  attachInterrupt(digitalPinToInterrupt(Button), Button_Pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(Pyro), Pyro_Change, CHANGE);
  pinMode(LED_warm, OUTPUT);
  pinMode(LED_cold, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  Serial.begin(115200);
  Wire.begin();                // 加入 I2C 总线，作为主机
}

void loop() 
{
  
  Get_Time();
  Clock_on();
  Check_Battery();
  Check_Timing();
 
  Wire.beginTransmission(7); //发送数据到设备号为7的从机
  if(count1>900)               //循环时间函数
  {
    count2++;
    count4++;
    count1=0;
    Serial.println(count2);
    Serial.print(Time[0]);Serial.print(':');Serial.print(Time[1]);Serial.print(':');Serial.println(Time[2]);
    Serial.print("Battery: ");Serial.println(Battery);
    Serial.print("Pyro: ");Serial.println(digitalRead(Pyro));
  }

  if(count2>5) 
  {
    checkIn(); //登陆函数
    SendTempHum();
    count2=0;
  }
  if (count4>60)
  {
    LED_warm_power=0;
    LED_cold_power=0;
    count4=0;
  }
  analogWrite(LED_warm,LED_warm_power);
  analogWrite(LED_cold,LED_cold_power);

  F_BigIoT();  
  count1++;
}

void Check_Timing()
{
  if (Timing_Open[0]==Time[0]&&Timing_Open[1]==Time[1])
  {
    LED_warm_power=255;
    LED_cold_power=255;
    count4=0;
  }
  if (Timing_Close[0]==Time[0]&&Timing_Close[1]==Time[1])
  {
    LED_warm_power=0;
    LED_cold_power=0;
  }
}

void Pyro_Change()
{
  if ((LED_warm_power==0)&&(LED_cold_power==0))
  {
    LED_warm_power=255;
    LED_cold_power=255;
    count4=0;
  }
  else count4=0;
}

void Button_Pressed()
{
  if (LED_warm_power!=255&&LED_warm_power!=204&&LED_warm_power!=153&&LED_warm_power!=102&&LED_warm_power!=51&&LED_warm_power!=0) LED_warm_power=306;
  LED_warm_power-=51;
  if (LED_warm_power<0) LED_warm_power=255;
  if (LED_cold_power!=255&&LED_cold_power!=204&&LED_cold_power!=153&&LED_cold_power!=102&&LED_cold_power!=51&&LED_cold_power!=0) LED_cold_power=306;
  LED_cold_power-=51;
  if (LED_cold_power<0) LED_cold_power=255;
  count4=0;
}

void Check_Battery()
{
  int sensorValue = analogRead(A0);
  Battery = sensorValue * (5.0 / 1023.0);
  if (Battery<3.4) digitalWrite(Buzzer,HIGH);
  else digitalWrite(Buzzer,LOW);
}

void Clock_on()
{
  if (Clock[0]==Time[0]&&Clock[1]==Time[1]&&Time[3]%2==0) digitalWrite(Buzzer, HIGH);
  else digitalWrite(Buzzer, LOW);
}

void Get_Time()
{
  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_DS1307, 7);
  if (Wire.available() >= 7)
  {
    for (int i = 0; i < 7; i++)
    {
     timeBcd[6-i] = Wire.read();
    }
  }
  Time[0]=(timeBcd[4]/16)*10+timeBcd[4]%16;
  Time[1]=(timeBcd[5]/16)*10+timeBcd[5]%16;
  Time[2]=(timeBcd[6]/16)*10+timeBcd[6]%16;
}

void SendTempHum()   // 温湿度模块 
{
   int chk = DHT11.read(DHT11PIN);      
   int Temperature_float=(float)DHT11.temperature;      //温度
   int Humidity_float=(float)DHT11.humidity;         //湿度
   if(count3==1)
         update1(DEVICEID,"3382",Temperature_float);    //上传温度      
   if(count3==2)  
         update1(DEVICEID,"3384",Humidity_float);    //上传湿度 

    char Sending[10]="";
    char Temprature[2]="";
    char Humidity[2]="";
    char BatteryCent[2]="";
    itoa(Temperature_float,Temprature,10);
    itoa(Humidity_float,Humidity,10);
    itoa(((Battery-3.5)/0.8)*100,BatteryCent,10);
    if (Temperature_float<10.0)
    {
      Temprature[1]=Temprature[0];
      Temprature[0]='0';
    }
    if (Humidity_float<10.0)
    {
      Humidity[1]=Humidity[0];
      Humidity[0]='0';
    }
    if (((Battery-3.5)/0.8)*100<10.0)
    {
      BatteryCent[1]=BatteryCent[0];
      BatteryCent[0]='0';
    }
    strcat(Sending,Temprature);
    strcat(Sending,",");
    strcat(Sending,Humidity);
    strcat(Sending,",");
    strcat(Sending,BatteryCent);
    strcat(Sending,"\n");
    Serial.println(Sending);
    Wire.beginTransmission(7);    //发送数据到设备号为7的从机
    Wire.write(Sending);       // 发送变量
    Wire.endTransmission();        // 停止发送
    
    if (count3==2) count3=0;
    count3++;
}

void checkIn() 
{    
  //登陆函数
   if (!CONNECT) 
   {
     Serial.print("+++");
     delay(500);  
     Serial.print("\r\n"); 
     delay(1000);
     Serial.print("AT+RST\r\n"); 
     delay(6000);
     CONNECT=true;
     lastCheckInTime=0;
   }
   else
   {
     Serial.print("{\"M\":\"checkin\",\"ID\":\"");
     Serial.print(DEVICEID);
     Serial.print("\",\"K\":\"");
     Serial.print(APIKEY);
     Serial.print("\"}\r\n");
     lastCheckInTime = millis();   
   }
}

void processMessage(aJsonObject *msg)
{    //接受函数
   aJsonObject* method = aJson.getObjectItem(msg, "M");
   aJsonObject* content = aJson.getObjectItem(msg, "C");     
   aJsonObject* client_id = aJson.getObjectItem(msg, "ID");  
   if (!method) {
     return;
   }
     String M=method->valuestring;
     String C=content->valuestring;
     String F_C_ID=client_id->valuestring;
     if(M=="say")
     {    //接收字符串
      long data=C.toInt();
      switch (data/10000)
      {
        case 0:
          LED_warm_power=LED_cold_power=0;
          break;
        case 1:
          LED_warm_power=data%1000;
          LED_cold_power=0;
          break;
        case 2:
          LED_warm_power=0;
          LED_cold_power=data%1000;
          break;
        case 3:
          LED_warm_power=data%1000;
          LED_cold_power=data%1000;
          break;
        case 4:
          Clock[0]=(data%10000)/100;
          Clock[1]=data%100;
          break;
        case 5:
          Timing_Open[0]=(data%10000)/100;
          Timing_Open[1]=data%100;
          break;
        case 6:
          Timing_Close[0]=(data%10000)/100;
          Timing_Close[1]=data%100;
          break;
      }
     }
}

void sayToClient(String client_id, String content)
{
   Serial.print("{\"M\":\"say\",\"ID\":\"");
   Serial.print(client_id);
   Serial.print("\",\"C\":\"");
   Serial.print(content);
   Serial.print("\"}\r\n");
   lastCheckInTime = millis();
}

void update1(String did, String inputid, float value)
{ 
  //上传函数
  Serial.print("{\"M\":\"update\",\"ID\":\"");
  Serial.print(did);
  Serial.print("\",\"V\":{\"");
  Serial.print(inputid);
  Serial.print("\":\"");
  Serial.print(value);
  Serial.println("\"}}");
  lastCheckInTime = millis();
  lastUpdateTime= millis(); 
}

void serialEvent() 
{
   while (Serial.available()) 
   {
     char inChar = (char)Serial.read();
     inputString += inChar;
     if (inChar == '\n') 
     {
       stringComplete = true;
     }
   }
}

void F_BigIoT()
{
  serialEvent();
  if (stringComplete)
  {
  inputString.trim();
     if(inputString=="CLOSED")
     {
      Serial.println("connect closed!");
     CONNECT=false;        
       }
       else
       {
    int len = inputString.length()+1;    
   if(inputString.startsWith("{") && inputString.endsWith("}"))
   {
    char jsonString[len];
   inputString.toCharArray(jsonString,len);
     aJsonObject *msg = aJson.parse(jsonString);
    processMessage(msg);
    aJson.deleteItem(msg);          
      }
   }
   // clear the string:
     inputString = "";
    stringComplete = false;   
  }
}



