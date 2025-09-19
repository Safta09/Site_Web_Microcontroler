#include <DHT.h>
#include <ESP8266WiFi.h>

// Definire pin senzor și tip
#define DHT_SENSOR_PIN D4     // Pin digital (ex: D4 = GPIO2 pe NodeMCU)
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nTest DHT11 cu ESP8266");

  // Inițializare WiFi în mod station și deconectare de la orice rețea
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Inițializare senzor DHT
  dht_sensor.begin();

  Serial.println("Senzor DHT inițializat!");
  delay(2000);
}

void loop() {
  delay(2000);

  float humidity = dht_sensor.readHumidity();
  float temperature = dht_sensor.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Eroare la citirea din senzor DHT!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print("°C, Umiditate: ");
    Serial.print(humidity);
    Serial.println("%");
  }
}