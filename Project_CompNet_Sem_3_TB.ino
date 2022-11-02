#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <ThingsBoard.h> // ThingsBoard SDK
#include <DHT.h>  // Including library for dht
// WiFi access point
#define WIFI_AP_NAME "WiFi-SSID"
// WiFi password
#define WIFI_PASSWORD "WiFi-Password"
// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token

#define TOKEN "Use user device token"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER "thingsboard IP Address"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200


#define DHTPIN D4          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);

const int AirValue = 790;   //you need to replace this value with Value_1
const int WaterValue = 390;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int relaypin = D5;

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

void setup() {
// SET UP SERIAL PORT
Serial.begin(SERIAL_DEBUG_BAUD);

pinMode(relaypin, OUTPUT);
dht.begin();

// SET UP WIFI
WiFi.begin (WIFI_AP_NAME,WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED)
{
delay (1000);
Serial.println ("Connecting to WiFi network...");
}
Serial.println ("Connected!");
}

void loop() {
float h = dht.readHumidity();
float t = dht.readTemperature();

Serial.print("Humidity: ");
Serial.println(h);
Serial.print("Temperature: ");
Serial.println(t);

soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
Serial.println(soilMoistureValue);
  
soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

if(soilmoisturepercent > 100)
{
  Serial.println("100 %");

  delay(250);
}

else if(soilmoisturepercent <0)
{
  Serial.println("0 %");
 
  delay(250);
  
}

else if(soilmoisturepercent >=0 && soilmoisturepercent <= 100)
{
  Serial.print(soilmoisturepercent);
  Serial.println("%");

  delay(250);
  
}

if(soilmoisturepercent >=0 && soilmoisturepercent <= 30)
  {
    digitalWrite(relaypin, HIGH);
    Serial.println("Motor is ON");
  }
  else if (soilmoisturepercent >30 && soilmoisturepercent <= 100)
  {
    digitalWrite(relaypin, LOW);
    Serial.println("Motor is OFF");
  }

// Connect to the ThingsBoard
Serial.print("Connecting to: ");
Serial.print(THINGSBOARD_SERVER);
Serial.print(" with token ");
Serial.println(TOKEN);
if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
Serial.println("Failed to connect");
return;
}
if (isnan(h) || isnan(t)) {
Serial.println("Failed to read from DHT sensor!");
} else {
tb.sendTelemetryFloat("temperature",t);
tb.sendTelemetryFloat("humidity",h);

tb.sendTelemetryInt("soilMoisture",soilMoistureValue);
tb.sendTelemetryInt("soilMoisturepercent",soilmoisturepercent);

tb.loop();
}
}
