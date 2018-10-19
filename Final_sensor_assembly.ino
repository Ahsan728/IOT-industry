#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 6, 7, 8);
#include "HX711.h"

HX711 cell(2, 3);
#define DT 2
#define SCK 3


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


void setup()
{
  Serial.begin(9600);
  lcd.begin(20, 4);
   dht.begin(); 
  
  lcd.setCursor(0,0);
  lcd.print("Industry Monitoring");
  lcd.setCursor(0,1);
  lcd.print("Over Website");
  delay(1000);
  lcd.clear();

  pinMode(A5, INPUT);
  pinMode(18, OUTPUT);
  state= digitalRead(A5);
}

void loop()
{
  ////////////////////////weight scale///////////////////
{
  count = count + 1;
  
  // Use only one of these
  val = ((count-1)/count) * val    +  (1/count) * cell.read(); // take long term average
 // val = 0.5 * val    +   0.5 * cell.read(); // take recent average
 val = cell.read(); // most recent reading
  Serial.println ((val-8268730)/-256.02);
lcd.setCursor(0,0);
lcd.print((val-8261259)/-362.53);
lcd.print("-Weight in (gm)");
}
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

///////////////////GAS SENSOR////////////////////
{
  sensorValue=analogRead(GasSensorPin);
  lcd.setCursor(0,2);
  lcd.print("Gas:");
  lcd.print(sensorValue,DEC);
   lcd.print("ppm");


}
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


}


