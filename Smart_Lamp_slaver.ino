/*循环接收主机发送来的数据包，同时显示在串口监视器上

  把数据包的最后一个字节，再上传回主机
*/
#include <Wire.h>//声明I2C库文件
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
byte timeBcd[] = {0, 0, 0, 0, 0, 0, 0};
#define ADDRESS_DS1307 0x68
char a[10]=" ";
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16 //定义显示高度  
#define LOGO16_GLCD_WIDTH  16 //定义显示宽度  

#define Speaker_Dist 8


int x;//变量x值决定主机的LED是否点亮
//初始化
#if (SSD1306_LCDHEIGHT != 64)
error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//============= 超声波模块=================
const int TrigPin = 2;
const int EchoPin = 3;
float distance;
//=========================================

void setup()
{ Serial.print(a[0]);
  Wire.begin(7);                // 加入 i2c 总线，设置从机地址为 #4
  Wire.onReceive(receiveEvent); //注册接收到主机字符的事件
  //Wire.onRequest(requestEvent); // 注册主机通知从机上传数据的事件
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);//设置串口波特率


  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(Speaker_Dist, OUTPUT);
  digitalWrite(Speaker_Dist, LOW);
}
//主程序
char c;
void loop()
{ 
  Serial.print(a[0]);
  Wire.onReceive(receiveEvent);
  //=========时间函数============
  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_DS1307, 7);
  if (Wire.available() >= 7)
  {
    for (int i = 0; i < 7; i++)
    {
      timeBcd[6 - i] = Wire.read();
    }
  }

//print
  Serial.print("20"); Serial.print(timeBcd[0], HEX); Serial.print("/");
  Serial.print(timeBcd[1], HEX); Serial.print("/"); Serial.print(timeBcd[2], HEX);
  Serial.print(" "); 
  Serial.print(timeBcd[4], HEX); Serial.print(":");
  Serial.print(timeBcd[5], HEX); Serial.print(":");
  Serial.print(timeBcd[6], HEX); Serial.println();

  if (a[0]>1&&timeBcd[0]!=0)
  {
    display.clearDisplay();
    display.setTextSize(1);             //设置字体大小
    display.setTextColor(WHITE);        //设置字体颜色白色
    display.setCursor(0, 0);          //设置字体的起始位置
    display.print("Temperature : ");
    display.print(a[0]);//输出字符并换行
    display.print(a[1]);
    display.println(" C");
    display.print("Humidity : ");
    display.print(a[3]);
    display.print(a[4]);
    display.println("%");
    display.print("BatteryCent : ");
    display.print(a[6]);
    display.print(a[7]);
    display.println("%");
    display.print("20"); display.print(timeBcd[0], HEX); display.print("/");
    display.print(timeBcd[1], HEX); display.print("/"); display.print(timeBcd[2], HEX);
    display.print(" ");
    display.print(timeBcd[4], HEX); display.print(":");
    display.print(timeBcd[5], HEX); display.print(":");
    display.print(timeBcd[6], HEX); display.println();
    display.display();
    if(a[6]==0)
    {
        display.clearDisplay();
        display.setTextSize(2);             //设置字体大小
        display.setTextColor(WHITE);        //设置字体颜色白色
        display.setCursor(0, 0);          //设置字体的起始位置
        display.println("   LOW");
        display.println("BATTERY!!!");
        display.display();
      }

  }
  AlarmDist();
}

// 当从机接收到主机字符，执行该事件
void receiveEvent(int howMany)
{  
    int i=0;
    
 while ( Wire.available() > 1) // 循环执行，直到数据包只剩下最后一个字符
  { 
    c = Wire.read(); // 作为字符接收字节
    Serial.println(c);
    a[i]=c;
    i++;// 把字符打印到串口监视器中
  }
}

void AlarmDist()
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  
  distance = pulseIn(EchoPin, HIGH) / 58.0;
  if (distance<=20&&distance>0)
  {
    digitalWrite(Speaker_Dist, HIGH);
    delayMicroseconds(2);
  }
  digitalWrite(Speaker_Dist, LOW);
}


