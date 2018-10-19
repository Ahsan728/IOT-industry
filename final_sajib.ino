#include <LiquidCrystal.h>
#include "HX711.h"
HX711 cell(2, 3);

LiquidCrystal lcd(12, 11, 5, 6, 7, 8);
#include<Timer.h>
Timer t;
#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10);
#define heart 13
#include <DHT.h>  
#define dht_dpin 14  
#define DHTTYPE DHT11 
DHT dht(dht_dpin, DHTTYPE);

int humi,tem;

long val = 0;
float count = 0;

int state=LOW; 
int laststate=LOW;
int counter=0; 

int sensorValue;
int GasSensorPin= 15;

const int MIC = 4; //the microphone amplifier output is connected to pin A0
int adc;
int dB, PdB;


char *api_key="VIJ43IHU2C7X68O9";     // Enter your Write API key from ThingSpeak
static char postUrl[150];
void httpGet(String ip, String path, int port=80);

void setup()
{
  Serial.begin(9600);
   mySerial.begin(115200);
  lcd.begin(20, 4);
   dht.begin(); 

  lcd.setCursor(0,0);
  lcd.print("Industry Monitoring");
  lcd.setCursor(0,1);
  lcd.print("Over Website");
  delay(3000);
  lcd.clear();
 lcd.print("WIFI Connecting");
 lcd.setCursor(0,1);
 lcd.print("Please wait....");
 lcd.setCursor(0,2);
 Serial.println("Connecting Wifi....");
 lcd.print("Connecting Wifi....");
 connect_wifi("AT",1000);
 connect_wifi("AT+CWMODE=1",1000);
 connect_wifi("AT+CWQAP",1000);  
 connect_wifi("AT+RST",5000);
 connect_wifi("AT+CWJAP=\"WEBindustry\",\"12345678\"",10000);
 Serial.println("Wifi Connected"); 
 lcd.clear();
 lcd.print("WIFI Connected.");
 pinMode(heart, OUTPUT);
 delay(5000);
 t.oscillate(heart, 1000, LOW);
 t.every(20000, send2server);
   
  pinMode(A5, INPUT);
  pinMode(18, OUTPUT);
  state= digitalRead(A5);
}

void loop()
{
  ///////////////////////load cell/////////////
  count = count + 1;
  
  // Use only one of these
  val = ((count-1)/count) * val    +  (1/count) * cell.read(); // take long term average
 // val = 0.5 * val    +   0.5 * cell.read(); // take recent average
  Serial.println ((val-8268730)/-256.02);
lcd.setCursor(0,0);
lcd.print((val-8261259)/-362.53);
lcd.print("-Weight in (gm)");
////////////////////////product  counter///////////////////
  {
if (state==1 && laststate==0)
{
  counter=counter+1;
delay(500);
lcd.setCursor(0,1);
lcd.print("    -No of Prod.goes");
lcd.setCursor(0,1);
lcd.print(counter);

}
laststate=state;
state=digitalRead(A5);
} 
 delay(10);

///////////////////GAS SENSOR////////////////////
{
  sensorValue=analogRead(GasSensorPin);
  lcd.setCursor(0,2);
  lcd.print("Gas:");
  lcd.print(sensorValue,DEC);
   lcd.print("ppm");


}
 delay(10);
///////////////////////////////SOUND////////////////

{

  PdB = dB; //Store the previous of dB here
  
adc= analogRead(MIC); //Read the ADC value from amplifer 
Serial.println (adc);//Print ADC for initial calculation 
dB = (adc+60) / 19.5; //Convert ADC value to dB using Regression values
if (PdB!=dB)
lcd.setCursor(0,3);
lcd.print ("Sound:");
lcd.print (dB);
lcd.print ("dB");


}
 delay(10);
{
  lcd.setCursor(0,4);
  lcd.print("Humi: ");
  tem= dht.readTemperature();
  lcd.print(humi);   // printing Humidity on Serial
  lcd.print("%;");
  lcd.print("Tem:");
  humi= dht.readHumidity();
  lcd.print(tem);   // Printing temperature on Serial
  lcd.print(char(223));
  lcd.print("C");

}
 delay(2000);
  t.update();

if (sensorValue > 220 | dB> 120 )
{digitalWrite(18,HIGH);}
else
{digitalWrite(18,LOW);}

}



void send2server()
{
  char tempStr[8];
  char humidStr[8];
  char counterStr[8];
  char weightStr[8];
  char GasStr[8];
  char SoundStr[8];
  dtostrf(tem, 5, 3, tempStr);
  dtostrf(humi, 5, 3, humidStr);
  dtostrf((counter=counter+1), 5, 3, counterStr);
  dtostrf(((val-8261259)/-362.53), 5, 3, weightStr);
  dtostrf(sensorValue, 5, 3, GasStr);
  dtostrf(dB, 5, 3, SoundStr);
  sprintf(postUrl, "update?api_key=%s&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field6=%s",api_key,tempStr,humidStr,counterStr,weightStr,GasStr,SoundStr);
  httpGet("api.thingspeak.com", postUrl, 80);
}

//GET https://api.thingspeak.com/update?api_key=VIJ43IHU2C7X68O9&field1=0
//https://api.thingspeak.com/update?api_key=VIJ43IHU2C7X68O9&field1=0
void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd = "GET /"+path+" HTTP/1.0\r\n\r\n";
  //AT+CIPSTART="TCP","192.168.20.200",80
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"";
  connect_wifi(atTcpPortConnectCmd,1000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd+=len;
  connect_wifi(atSendCmd,1000);
  connect_wifi(atHttpGetCmd,1000);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    lcd.clear();
    lcd.print(cmd);
    Serial.println(cmd);
    mySerial.println(cmd); 
    while(mySerial.available())
    {
      if(mySerial.find("OK"))

      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
   Serial.println("OK");
        lcd.setCursor(0,1);
      lcd.print("OK");
  }
  else
  {
   Serial.println("Error");
         lcd.setCursor(0,1);
      lcd.print("Error");
  }
}


