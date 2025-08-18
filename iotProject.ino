
#define BLYNK_TEMPLATE_ID "TMPL6-nszRwyf"
#define BLYNK_TEMPLATE_NAME "Smart Watering System"
#define BLYNK_AUTH_TOKEN "i4ZN8OiyWFhxRwZ9Grm0XpXMtXRQoEgG"


#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"


char ssid[] = "smart";
char pass[] = "ashib123";


#define DHT_PIN 5     
#define SOIL_PIN 34    
#define PUMP_PIN 4      


const int DRY_THRESHOLD = 3500; 
const int PUMP_DURATION = 3000; 


#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

BlynkTimer timer;


void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soilMoistureValue = analogRead(SOIL_PIN);


  Blynk.virtualWrite(V0, soilMoistureValue);  
  Blynk.virtualWrite(V2, temperature);       
  Blynk.virtualWrite(V3, humidity);           

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C  |  Humidity: ");
  Serial.print(humidity);
  Serial.print("%  |  Soil Moisture: ");
  Serial.println(soilMoistureValue);

 
  if (soilMoistureValue > DRY_THRESHOLD) {
    Serial.println("Soil is DRY → Pump ON");
    digitalWrite(PUMP_PIN, HIGH);
    Blynk.virtualWrite(V1, 1); 
    Blynk.virtualWrite(V4, 1); 

    delay(PUMP_DURATION);

    digitalWrite(PUMP_PIN, LOW);
    Blynk.virtualWrite(V1, 0); 
    Blynk.virtualWrite(V4, 0); 

    Serial.println("Watering finished.");
  } else {
    Serial.println("Soil is moist → Pump OFF");
    digitalWrite(PUMP_PIN, LOW);
    Blynk.virtualWrite(V1, 0); 
    Blynk.virtualWrite(V4, 0); 
  }
  Serial.println("-------------------------------------------------");
}


BLYNK_WRITE(V1) {
  int pumpState = param.asInt();
  digitalWrite(PUMP_PIN, pumpState);
  Blynk.virtualWrite(V4, pumpState); 
  Serial.println(pumpState ? "Pump turned ON manually" : "Pump turned OFF manually");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);  
  
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(5000L, sendSensorData); 
}

void loop() {
  Blynk.run();
  timer.run();
}
