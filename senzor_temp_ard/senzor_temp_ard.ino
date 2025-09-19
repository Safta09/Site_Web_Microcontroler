#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <SD.h>


// PIN și tip senzor
#define DHT_SENSOR_PIN D4
#define DHT_SENSOR_TYPE DHT11
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// WiFi

#define WIFI_SSID "Android"
#define WIFI_PASSWORD ""


//#define WIFI_SSID ""
//#define WIFI_PASSWORD ""

// Firebase
#define API_KEY "Api Key "
#define DATABASE_URL "https://senzor-temperatura-17dc0-default-rtdb.firebaseio.com/" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// Buffer pentru medie
const int numReadings = 5;
float tempReadings[numReadings] = {0};
float humReadings[numReadings] = {0};
int readIndex = 0;
int totalReadings = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  dht_sensor.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Se asteapta conectare.");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  //config.token_status_callback = tokenStatusCallback;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up OK");
    signupOK = true;
  } else {
    Serial.printf("Sign-up error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Eroare la citirea senzorului DHT11!");
    return;
  }

  // Salvăm citirea curentă în buffer
  tempReadings[readIndex] = temperature;
  humReadings[readIndex] = humidity;

  readIndex = (readIndex + 1) % numReadings;
  if (totalReadings < numReadings) totalReadings++;

  // Calculăm media doar dacă avem suficiente citiri
  if (totalReadings == numReadings) {
    float sumTemp = 0, sumHum = 0;
    for (int i = 0; i < numReadings; i++) {
      sumTemp += tempReadings[i];
      sumHum += humReadings[i];
    }

    float avgTemp = sumTemp / numReadings; // avregeTemp
    float avgHum = sumHum / numReadings; //avreage hum

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 6500)) {
      sendDataPrevMillis = millis();

      if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", avgTemp)) {
        Serial.print("Temperatura medie trimisă: ");
        Serial.println(avgTemp);
      } else {
        Serial.println("Eroare trimitere temperatură");
        Serial.println("Motiv: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", avgHum)) {
        Serial.print("Umiditate medie trimisă: ");
        Serial.println(avgHum);
      } else {
        Serial.println("Eroare trimitere umiditate");
        Serial.println("Motiv: " + fbdo.errorReason());
      }
    }
  }

  delay(2000);  // mai mult timp între citiri pentru a nu aglomera bufferul
}
