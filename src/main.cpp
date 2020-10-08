
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <Schedule.h>
#include <BlynkSimpleEsp8266_SSL.h>
// #include <DHTesp.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


// Uncomment the type of sensor in use:
// #define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview



ADC_MODE(ADC_VCC);                                // enabled measure of 3.3 V level on A0 port but disables that port for ussage

#define DEVICENAME "CellarMonitor"
#define DHTPIN 5                                  
#define PIN_TERMINAL V8

#define AUTH "JhlCbpzEzkKnklsp4rCHSREdXzYpMBW-"
#define INTERVAL 10000L
#define LED              2   // The pin (marked D4) that activates the LED (internal)
// #define DEBUG_OUTPUT Serial
#define DEBUG_OUTPUT Terminal

// DHTesp dht;
char ssid[] = "Maccheccazzo";
char pass[] = "bigrubberbunny";
WidgetTerminal Terminal(PIN_TERMINAL);
DHT_Unified dht(DHTPIN, DHTTYPE);


void sendTemperature()                            // temperature measuring function
{
  sensors_event_t event;
  DEBUG_OUTPUT.print("["); 
  DEBUG_OUTPUT.print(millis());
  DEBUG_OUTPUT.print("] ");
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    DEBUG_OUTPUT.println(F("Error reading temperature!"));
  } else {
    Blynk.virtualWrite(0, event.temperature);   // send to Blynk virtual pin 0 temperature value
    DEBUG_OUTPUT.print(F("Temperature: "));
    DEBUG_OUTPUT.print(event.temperature);
    DEBUG_OUTPUT.print(F("°C  |  "));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    DEBUG_OUTPUT.println(F("Error reading humidity!"));
  } else {
    Blynk.virtualWrite(1, event.relative_humidity);                       // send to Blynk virtual pin 1 humidity value
    DEBUG_OUTPUT.print(F("Humidity: "));
    DEBUG_OUTPUT.print(event.relative_humidity);
    DEBUG_OUTPUT.println(F("%"));
  }
  // DEBUG_OUTPUT.println("success sleep");                // send to serial control messsage
  // ESP.deepSleep(600000000);                       // put device to 10 minutes sleep, adjust if other sleep time is reqired between measurements
  // delay(200);                                     // recommended to use with deepsleep
}

Ticker Polling(sendTemperature, INTERVAL, 0, MILLIS); 

void setup() {
  Serial.begin(115200);
  DEBUG_OUTPUT.print(" \n\n["); 
  DEBUG_OUTPUT.print(millis());
  DEBUG_OUTPUT.println("] Setup");
  dht.begin();
  // digitalWrite(LED, HIGH);
  WiFi.hostname(DEVICENAME); // DHCP Hostname 
  Blynk.begin(AUTH, ssid, pass);
  Polling.start();
  DEBUG_OUTPUT.print("IP Address: ");
  DEBUG_OUTPUT.println(WiFi.localIP().toString());
  ArduinoOTA.begin();
  DEBUG_OUTPUT.println(F("DHTxx Unified Sensor Example")); 
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  DEBUG_OUTPUT.println(F("------------------------------------"));
  DEBUG_OUTPUT.println(F("Temperature Sensor"));
  DEBUG_OUTPUT.print  (F("Sensor Type: ")); DEBUG_OUTPUT.println(sensor.name);
  DEBUG_OUTPUT.print  (F("Driver Ver:  ")); DEBUG_OUTPUT.println(sensor.version);
  DEBUG_OUTPUT.print  (F("Unique ID:   ")); DEBUG_OUTPUT.println(sensor.sensor_id);
  DEBUG_OUTPUT.print  (F("Max Value:   ")); DEBUG_OUTPUT.print(sensor.max_value); DEBUG_OUTPUT.println(F("°C"));
  DEBUG_OUTPUT.print  (F("Min Value:   ")); DEBUG_OUTPUT.print(sensor.min_value); DEBUG_OUTPUT.println(F("°C"));
  DEBUG_OUTPUT.print  (F("Resolution:  ")); DEBUG_OUTPUT.print(sensor.resolution); DEBUG_OUTPUT.println(F("°C"));
  DEBUG_OUTPUT.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  DEBUG_OUTPUT.println(F("Humidity Sensor"));
  DEBUG_OUTPUT.print  (F("Sensor Type: ")); DEBUG_OUTPUT.println(sensor.name);
  DEBUG_OUTPUT.print  (F("Driver Ver:  ")); DEBUG_OUTPUT.println(sensor.version);
  DEBUG_OUTPUT.print  (F("Unique ID:   ")); DEBUG_OUTPUT.println(sensor.sensor_id);
  DEBUG_OUTPUT.print  (F("Max Value:   ")); DEBUG_OUTPUT.print(sensor.max_value); DEBUG_OUTPUT.println(F("%"));
  DEBUG_OUTPUT.print  (F("Min Value:   ")); DEBUG_OUTPUT.print(sensor.min_value); DEBUG_OUTPUT.println(F("%"));
  DEBUG_OUTPUT.print  (F("Resolution:  ")); DEBUG_OUTPUT.print(sensor.resolution); DEBUG_OUTPUT.println(F("%"));
  DEBUG_OUTPUT.println(F("------------------------------------"));

  DEBUG_OUTPUT.print("["); 
  DEBUG_OUTPUT.print(millis());
  DEBUG_OUTPUT.println("] END Setup");
}

void loop() {
  Polling.update();
  Blynk.run();
  ArduinoOTA.handle();
}









