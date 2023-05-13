#include <SoftwareSerial.h> // Inclure la librairie SoftwareSerial pour la communication série
#include <Wire.h> // Inclure la librairie wire pour la communication en I2C
#include <Adafruit_BMP085.h> // Inclure la librairie BMP085 pour la communication avec le BMP180

Adafruit_BMP085 bmp;
volatile float comptageImpulsion=0; //variable de type int dont l'état est enregistré dans la RAM afin d'être changé

long writingTimer = 17; // déclare une variable de type long 
long startTime = 0;// déclaration d'une variable de type long
long waitTime = 0;// déclaration d'une variable de type long
unsigned char check_connection=0; //déclaration d'une variable non signé check_connection qui à prend pour valeur 0
unsigned char times_check=0; //déclaration d'une variable non signé times_check pour l'attente de connexion

String apiKey = ""; //thingspeak WRITE API key
float a; // variable décimal
const float n = 0.116875406; // const décimal de hauteur d'eau

boolean DEBUG=true;
SoftwareSerial ESP8266(10,11);

void setup() 
{                
  DEBUG=true;          
  Serial.begin(115200); 
  ESP8266.begin(115200);  
  bmp.begin();
 attachInterrupt(0, gestionINT0, RISING);
   Serial.println("OK Capteurs !");
  ESP8266.println("AT+RST");         // commande de reset
  delay(1000);
ESP8266.println("AT+CWMODE=1");   // esp8266 mode client
  delay(1000);

 Serial.println("Connecting to Wifi");
   while(check_connection==0)
  {
    Serial.print(".");
  ESP8266.print("AT+CWJAP=\"Name-wifi\",\"key-wifi\"\r\n");
  ESP8266.setTimeout(2000);
// on doit lire "WIFI CONNECTED"
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
  // Lecture des capteurs
   float t = bmp.readTemperature();
   float h = bmp.readPressure()*0.01;
   float r = a;
        if (isnan(t) || isnan(h) || isnan(r)) {
        if (DEBUG) Serial.println("Failed to read from sensors");
      }
      else {
          if (DEBUG)  Serial.println("temp="+String(t)+"°C");
          if (DEBUG) Serial.println("pressure="+String(h)+" mbar");
          if (DEBUG) Serial.println("rain="+String(r)+" mm");
           thingSpeakWrite(t,h,r);                                      
      }  
  // thingspeak a besoin de 15 sec de delay entre chaque envois,     
  delay(20000);  
}
//======================================================================== afficher la réponse
void showResponse(int waitTime)
{
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (ESP8266.available()){
        c=ESP8266.read();
        if (DEBUG) Serial.print(c);
      }
    }               
}
void gestionINT0() 
{
comptageImpulsion=comptageImpulsion+1; // incrémentation de (+1) à chaque front montant
a = comptageImpulsion*n;
}
//========================================================================
boolean thingSpeakWrite(float value1, float value2, float value3)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connexion
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  ESP8266.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(ESP8266.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  } 
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr += "\r\n\r\n\r\n";
  
  // Envoi de données
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ESP8266.println(cmd);
  if (DEBUG)  Serial.println(cmd); 
  delay(100);
  if(ESP8266.find(">")){
    ESP8266.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    ESP8266.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}
