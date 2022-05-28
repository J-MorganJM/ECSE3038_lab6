#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_MLX90614.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <env.h>

#define ONBOARD_LED  2

int getWaterLevel();
int value = 0;


void setup() {
  
  Serial.begin(115200);
  pinMode(ONBOARD_LED,OUTPUT);

  //Temperature Sensor Initilization
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  WiFi.begin(Wifi_ssid,Wifi_passwd);
  while (WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("-Connecting-");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
  value=getWaterLevel();

  if(value>=27 && value<=35)
  {
    digitalWrite(ONBOARD_LED,HIGH);
  }
  else
  {
    digitalWrite(ONBOARD_LED,LOW);
  }

  HTTPClient http;

  // Your IP address with path or Domain name with URL path 
  http.begin("http://192.168.0.10:3000/tank");
  
  // Send HTTP POST request
  http.addHeader("Content-Type", "application/json");
  char output[128];
  const size_t CAPACITY = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<CAPACITY> doc;

  doc["tank_id"].set(WiFi.macAddress());
  doc["water_level"].set(value);
  
  serializeJson(doc,output);
  int httpResponseCode = http.POST(String(output));
  String payload = "{}"; 
  if (httpResponseCode>0) 
  {
    payload = http.getString();
  }
  else 
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  delay(5000);
  http.end();

}

int getWaterLevel()
{
  float temperature = mlx.readObjectTempC();
  return int(temperature);
}
