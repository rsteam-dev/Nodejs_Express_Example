/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

//와이파이 접속 아이디와 비밀번호 설정
#define WIFI_SSID "와이파이 SSID"
#define WIFI_PASS "와이파이 PASSWORD"
#define REQUEST_ADDRESS "http://192.168.0.100:3000/arduino/LEDToggle"

//GND와 LED를 연결한 핀 설정  (기본 0 번 핀)
unsigned int ledPin = 0;

void setup() {

  // ledPin 핀을 출력핀으로 설정
  pinMode(ledPin, OUTPUT);

  //Debug용시리얼포트 통신 Baudrate 설정
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  //Debug용 시리얼통신 셋업
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  //와이파이 설정 및 연결 시도
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

}

void loop() {
  // 와이파이 연결 대기...
  // 연결이 완료되면 http 클라이언트를 생성하고 request 전송
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    //http 클라이언트를 생성
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");

    //http 통신 설정 request 메세지를 보낼 주소 입력
    // 각자 생성하고 만든 서버 주소를 입력
    if (http.begin(client, REQUEST_ADDRESS)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      // GET 방식으로 서버에 request 메세지를 전송
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        // 서버에 보낸 메세지가 정상적으로 도착하고 Response 메세지를 보냈을때....
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

          //서버가 받은 메세지를 String 객체로 메모리에 저장한다.
          String payload = http.getString();
          Serial.println(payload);

          // 서버가 보낸 메세지가 "true"일 경우 0번핀에 5V 전압을 인가한다.
          // * 서버가 보낸 payload 메세지는 Boolean 타입이아닌 String 객체이기 때문에
          // char* 타입으로 변환 후 char* 타입인 "true"와 비교한다.
          if(strcmp(payload.c_str(), "true") == 0){
            digitalWrite(ledPin, HIGH);
          }else{
            digitalWrite(ledPin, LOW);
          }
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  // 전송 로직이 끝난 후 1초간 동작 정지.
  // 인터럽트를 제외한 모든 입출력과 동작이 멈춘다.
  delay(1000);
}
