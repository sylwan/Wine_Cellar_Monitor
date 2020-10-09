
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <Schedule.h>
#include <BlynkSimpleEsp8266_SSL.h>
// #include <DHTesp.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// #include <WidgetRTC.h>


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
#define TEN_MINUTES_IN_uS 600000000
#define INTERVAL 10000L
#define LED              2   // The pin (marked D4) that activates the LED (internal)
#define DEBUG_OUTPUT Serial 
// #define DEBUG_OUTPUT Terminal
#define DEEPSLEEP


// Static IP details...Use static because it's much faster

IPAddress ip(192,168,100,251);
IPAddress gateway(192,168,100,1);
IPAddress mask(255,255,255,0);
// DHTesp dht;
char ssid[] = "Maccheccazzo";
char pass[] = "bigrubberbunny";
WidgetTerminal Terminal(PIN_TERMINAL);
DHT_Unified dht(DHTPIN, DHTTYPE);
bool OTA_updating=false;
// WidgetRTC rtc;
// BLYNK_ATTACH_WIDGET(rtc, V8);

void sendTemperature()                            // temperature measuring function
{
  sensors_event_t event;
  DEBUG_OUTPUT.print("["); 
  DEBUG_OUTPUT.print(millis());
  DEBUG_OUTPUT.print("] \n");
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

}

// Ticker Polling(sendTemperature, INTERVAL, 0, MILLIS); 



void setup() {
  Serial.begin(115200);
  WiFi.hostname(DEVICENAME); // DHCP Hostname 
  WiFi.config(ip, gateway, mask);
  Blynk.begin(AUTH, ssid, pass);
  while(Blynk.connect() == false){};
  // rtc.begin();
  // DEBUG_OUTPUT.print(" \n\n["); 
  // DEBUG_OUTPUT.print(millis());
  // DEBUG_OUTPUT.println("] Setup");

  // ArduinoOTA.setHostname("ESP8266");
  // ArduinoOTA.setPassword("esp8266");
  ArduinoOTA.onStart([]() {
    DEBUG_OUTPUT.println("OTA Start...");
    OTA_updating = true;
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_OUTPUT.println("\nFinished updating");
    OTA_updating = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_OUTPUT.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_OUTPUT.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) DEBUG_OUTPUT.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DEBUG_OUTPUT.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DEBUG_OUTPUT.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DEBUG_OUTPUT.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) DEBUG_OUTPUT.println("OTA End Failed");
  });

  ArduinoOTA.begin();
  dht.begin();
  // digitalWrite(LED, HIGH);
  // Polling.start();
  DEBUG_OUTPUT.print("IP Address: ");
  DEBUG_OUTPUT.print(WiFi.localIP().toString());
  
  sendTemperature();
  delay(200);
  Blynk.run();
  ArduinoOTA.handle();
  // DEBUG_OUTPUT.println("success! sleep");                // send to serial control messsage
  // ESP.deepSleep(TEN_MINUTES_IN_uS);                       // put device to 10 minutes sleep, adjust if other sleep time is reqired between measurements

  if(!OTA_updating){  
    // ESP.deepSleep(10e6);                       // put device to 10 seconds for testing purposes
    // delay(200);        // recommended to use with deepsleep
    ESP.restart();   // for testing purposes simulating a deepsleep
  }
  // DEBUG_OUTPUT.print("["); 
  // DEBUG_OUTPUT.print(millis());
  // DEBUG_OUTPUT.println("] END Setup");
}

void loop() {
  // Polling.update();
  // Blynk.run();
  ArduinoOTA.handle();
}









