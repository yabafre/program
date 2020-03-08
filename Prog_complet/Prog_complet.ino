String myAPIkey = "Y6EU1TA63JGHBIZL";  // chaine de charactère de la clé_API

#include <SoftwareSerial.h> // Inclure la librairie SoftwareSerial pour la communication série
#include <Wire.h> // Inclure la librairie wire pour la communication en I2C
#include <Adafruit_BMP085.h> // Inclure la librairie BMP085 pour la communication avec le BMP180

Adafruit_BMP085 bmp;
volatile float comptageImpulsion=0;
float pressure, temp_f; // définir les variables pressure et temp_f de types flottante

long writingTimer = 10; // déclare une variable de type long 
long startTime = 0;// déclare une variable de type long
long waitTime = 0;// déclare une variable de type long

boolean relay1_st = false; // déclaration d'une variable de type binaire appelée relay1_st et initialisée à false
boolean relay2_st = false; // déclaration d'une variable de type binaire appelée relay2_st et initialisée à false
unsigned char check_connection=0; //déclaration d'une variable non signé check_connection qui à prend pour valeur 0
unsigned char times_check=0; //déclaration d'une variable non signé times_check qui à prend pour valeur 0
boolean error; // déclaration d'une variable de type binaire appelée error

SoftwareSerial ESP8266(10, 11); // Rx,  Tx

void setup()
{
  Serial.begin(115200); 
  ESP8266.begin(115200); 
   bmp.begin();
   attachInterrupt(0, gestionINT0, RISING);
  startTime = millis(); 
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  ESP8266.println("AT+RST");
  delay(2000);
  Serial.println("Connecting to Wifi");
   while(check_connection==0)
  {
    Serial.print(".");
  ESP8266.print("AT+CWJAP=\"frelike\",\"68810012470345\"\r\n");
  ESP8266.setTimeout(5000);
 if(ESP8266.find("WIFI CONNECTED\r\n")==1)
 {
 Serial.println("WIFI CONNECTED");
 break;
 }
 times_check++;
 if(times_check>3) 
 {
  times_check=0;
   Serial.println("Trying to Reconnect..");
  }
  }
}

void loop()
{
  waitTime = millis()-startTime;   
  if (waitTime > (writingTimer*1000)) 
  {
    readSensors();
    writeThingSpeak();
    startTime = millis();   
  }
}


void readSensors(void)
{
    temp_f = bmp.readTemperature();
    pressure = bmp.readPressure()*0.01;
}


void gestionINT0() {
comptageImpulsion=comptageImpulsion+1*0.307; 
}

void writeThingSpeak(void)
{
  startThingSpeakCmd();
  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += myAPIkey;
  getStr +="&field1=";
  getStr += String(temp_f);
  getStr +="&field2=";
  getStr += String(pressure);
  getStr +="&field3=";
  getStr += String(comptageImpulsion);
  getStr += "\r\n\r\n\r\n";
  GetThingspeakcmd(getStr); 
}

void startThingSpeakCmd(void)
{
  ESP8266.flush();
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com IP address
  cmd += "\",80";
  ESP8266.println(cmd);
  Serial.print("Start Commands: ");
  Serial.println(cmd);

  if(ESP8266.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

String GetThingspeakcmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ESP8266.println(cmd);
  Serial.println(cmd);

  if(ESP8266.find(">"))
  {
    ESP8266.print(getStr);
    Serial.println(getStr);
    delay(500);
    String messageBody = "";
    while (ESP8266.available()) 
    {
      String line = ESP8266.readStringUntil('\n');
      if (line.length() == 1) 
      { 
        messageBody = ESP8266.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    ESP8266.println("AT+CIPCLOSE");     
    Serial.println("AT+CIPCLOSE"); 
  } 
}
