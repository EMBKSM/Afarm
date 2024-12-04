#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <math.h>
#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
const char* ssid = "kjm";     // 여기에 Wi-Fi 네트워크 이름 입력
const char* password="00000000"; // 여기에 Wi-Fi 암호 입력
const char* serverAddress = "http://192.168.43.220:8080/plant_m/connect"; // 원격 서버 주소 및 포트
void setup() {
  dht.begin();
  pinMode(A0,INPUT);
  Serial.begin(115200);
  delay(10);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}
unsigned long now=0;
unsigned long before_2sec=0;
int sensor=0;
void loop() {
  now=millis();
  int data=abs((analogRead(A0)-760)/4);
  float Temp=dht.readTemperature();
  float Humi=dht.readHumidity();
  if(now-before_2sec>=2000){
    before_2sec=now;
    Serial.println(data);
    Serial.println(analogRead(A0));
    Serial.println(Temp);
    Serial.println(Humi);
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      // 원격 서버로 POST 요청 보내기
      WiFiClient client;
      http.begin(client, serverAddress); // 수정된 부분
      http.addHeader("Content-Type", "application/json");
      // JSON 형식으로 데이터 전송 (예: 센서 데이터를 넣으세요)
      String jsonData = "{\"date\":"+String(data)+",\"temp\":"+String(Temp)+",\"humi\":"+String(Humi)+"}"; // 데이터 내용을 원하는 대로 수정
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response Code: " + String(httpResponseCode));
        Serial.println(response);
      } else {
        Serial.println("Error in HTTP  request");
      }

      http.end();
    }
  }
}