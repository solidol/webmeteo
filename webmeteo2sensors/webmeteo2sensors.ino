/*
  Тестировалось на Arduino IDE 1.8.5
  Дата тестирования 03.10.2019г.
*/

#include <ESP8266WiFi.h>                                // Подключаем библиотеку ESP8266WiFi
#include <Wire.h>                                       // Подключаем библиотеку Wire
#include <Adafruit_BME280.h>                            // Подключаем библиотеку Adafruit_BME280
#include <Adafruit_Sensor.h>                            // Подключаем библиотеку Adafruit_Sensor
#include <ESP8266HTTPClient.h>
#define SEALEVELPRESSURE_HPA (1013.25)                  // Задаем высоту

Adafruit_BME280 bme77;                                    // Установка связи по интерфейсу I2C
Adafruit_BME280 bme76;                                    // Установка связи по интерфейсу I2C

const char* ssid = "magnitko";          // Название Вашей WiFi сети
const char* password = "10480144";     // Пароль от Вашей WiFi сети
const char *host = "192.168.43.128";
const int timeToSleep = 15 * 60;

String header;

void setup() {
  pinMode(16, OUTPUT);
  pinMode(0, OUTPUT);
  Serial.begin(9600);                                 // Скорость передачи 115200
  bool status;

  while (!bme76.begin(0x76)) {                               // Проверка инициализации датчика
    Serial.println("Could not find external BME280 sensor, check wiring!"); // Печать, об ошибки инициализации.
    delay(1000);// Зацикливаем
  }

  while (!bme77.begin(0x77)) {                               // Проверка инициализации датчика
    Serial.println("Could not find internal BME280 sensor, check wiring!"); // Печать, об ошибки инициализации.
    delay(1000);// Зацикливаем
  }

  Serial.print("Connecting to ");                       // Отправка в Serial port
  Serial.println(ssid);                                 // Отправка в Serial port
  WiFi.begin(ssid, password);                           // Подключение к WiFi Сети
  while (WiFi.status() != WL_CONNECTED) {               // Проверка подключения к WiFi сети]
    digitalWrite(16, LOW);   // turn the LED on (HIGH is the voltage level)
    delay(500);
    digitalWrite(16, HIGH);
    delay(500);                                         // Пауза
    Serial.print(".");                                  // Отправка в Serial port
  }
  digitalWrite(16, HIGH);
  Serial.println("");                                   // Отправка в Serial port
  Serial.println("WiFi connected.");                    // Отправка в Serial port
  Serial.println("IP address: ");                       // Отправка в Serial port
  Serial.println(WiFi.localIP());                       // Отправка в Serial port
}

void loop() {
  float sumT76, sumH76, sumP76;
  float sumT77, sumH77, sumP77;
  sumT76 = sumH76 = sumP76 = 0;
  sumT77 = sumH77 = sumP77 = 0;
  
  int ledSig = 0;
  for (int i = 0; i < 1000; i++) {
    sumT76 += bme76.readTemperature();
    sumH76 += bme76.readHumidity();
    sumP76 += bme76.readPressure();
    sumT77 += bme77.readTemperature();
    sumH77 += bme77.readHumidity();
    sumP77 += bme77.readPressure();
    if (i % 2 == 0) ledSig = !ledSig;
    digitalWrite(16, ledSig);
    delay(timeToSleep);
  }

  sumT76 /= 1000.0;
  sumH76 /= 1000.0;
  sumP76 /= 1000.0;
  sumT77 /= 1000.0;
  sumH77 /= 1000.0;
  sumP77 /= 1000.0;
  HTTPClient http;    //Declare object of class HTTPClient
  String postData;
  postData = "temperature=" + String(sumT76) +
             "&humidity=" + String(sumH76) +
             "&pressure=" + String(sumP76) +
             "&sensor_id=1";

  ;
  http.begin("http://192.168.88.245/api/wstamp");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
  postData = "temperature=" + String(sumT77) +
             "&humidity=" + String(sumH77) +
             "&pressure=" + String(sumP77) +
             "&sensor_id=2";

  ;
  http.begin("http://192.168.88.245/api/wstamp");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  httpCode = http.POST(postData);   //Send the request
  payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
  
  Serial.print("IP address: ");                       // Отправка в Serial port
  Serial.println(WiFi.localIP());                       // Отправка в Serial port
  
  digitalWrite(16, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);
  digitalWrite(16, HIGH);

}
