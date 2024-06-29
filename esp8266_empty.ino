/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

#include <ArduinoJson.h>

#include <config.h>


bool LAMBA_STATUS = false;
bool RGB_STATUS = false;
bool SERVER_STATUS = false;
bool COLLER_FAN_STATUS = false;
int LAMBA_PIN = D1;
int RGB_PIN = D2;

const int sensor_hand_pin = D5; 
const int sensor_hand_trigger_led = D6;

// defines variables
long duration;
int distance;

const String ESP_ID = "dcde7e17-791d-48c7-bbd9-bdfb6379f319";
const String TOKEN = "dcde7e17-791d-48c7-bbd9-bdfb6379f319";

const char* API_HOST = "smartapp.farukseker.com.tr";
const int PORT = 443;
//const int PORT = 8000;
const char* PATH = "/ws/communication/esp/dcde7e17-791d-48c7-bbd9-bdfb6379f319";


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;




int led_scoket_ping_pong = D8;
int har_sensor = D7;

int coller_fan_pin = D3; 
const int server_pin = D4; 


#define USE_SERIAL Serial
void pargosden(){
  USE_SERIAL.printf("lopen pars\n");
  delay(500);
}

void key_status(String key,bool status){
  if (key == "LAMBA_PIN"){
    USE_SERIAL.printf("lopen pars\n");
    LAMBA_STATUS = status;
    status_apply();
  }
  if (key == "RGB_PIN"){
    RGB_STATUS = status;
    status_apply(); 
  }
  if (key == "SERVER_PIN"){
    SERVER_STATUS = status;
    status_apply(); 
  }
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
      digitalWrite(led_scoket_ping_pong,HIGH);

			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
      digitalWrite(led_scoket_ping_pong,LOW);
			// send message to server when Connected
		}
//			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			break;
		case WStype_TEXT:{
      USE_SERIAL.println("hased message ");
      StaticJsonDocument<200> doc;

      DeserializationError error = deserializeJson(doc, (const char*)payload, length);
      if (error) {
        USE_SERIAL.print(F("[WSc] JSON parse failed: "));
        USE_SERIAL.println(error.c_str());
        return;
      }
      //const char* __type = doc["type"];
      const String __type = doc["type"];
      USE_SERIAL.print("tpye");
      USE_SERIAL.print(__type);

      if (__type == "key_update"){
        String key = doc["pin"];
        bool status = doc["status"];
        key_status(key,status);
      }
      if (__type == "key_sync"){
        String key = doc["pin"];
        bool status = doc["status"];
        key_status(key,status);
      }
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      break;
    }

		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
        case WStype_PING:
            // pong will be send automatically
            //USE_SERIAL.printf("[WSc] get ping\n");
            digitalWrite(led_scoket_ping_pong,HIGH);
            delay(100);
            digitalWrite(led_scoket_ping_pong,LOW);

            break;
        case WStype_PONG:
            // answer to a ping we send
            //USE_SERIAL.printf("[WSc] get pong\n");
            digitalWrite(led_scoket_ping_pong,HIGH);
            delay(100);
            digitalWrite(led_scoket_ping_pong,LOW);
            break;
    }

}

bool valo_message(String message){
        DynamicJsonDocument doc(1024);
      deserializeJson(doc, message);
      const char* value = doc["pin"];
			USE_SERIAL.printf(value);
      return 1;
      
}
void setup() {
  pinMode(LAMBA_PIN,INPUT);
  pinMode(RGB_PIN,INPUT);
  // master sensor pins
  pinMode(sensor_hand_pin, INPUT);
  pinMode(sensor_hand_trigger_led, OUTPUT);
//test*
  pinMode(server_pin, INPUT);

//
  pinMode(led_scoket_ping_pong,OUTPUT);
  digitalWrite(led_scoket_ping_pong,HIGH);

  pinMode(har_sensor, INPUT);
  pinMode(A0, INPUT);
    // JSON verisi burada yer alacak
  char json[] = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";
  
  // JSON nesnesi oluşturuluyor
  DynamicJsonDocument doc(1024);
  
  
  // JSON verisi ayrıştırılıyor
  DeserializationError _error = deserializeJson(doc, json);
  if (_error) {
    Serial.println("Ayrıştırma hatası!");
    return;
  }
  
  // JSON nesnesinin içeriği okunuyor
  const char* name = doc["name"];
  int age = doc["age"];
  const char* city = doc["city"];
  



  //pinMode(send_msg_btn,INPUT);
  //pinMode(send_msg_btn,OUTPUT);
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(9600);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

  // okunan veriler seri porta yazdırılıyor
  Serial.print("name: ");
  Serial.println(name);
	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}
  WiFiMulti.addAP("pars", "1afsbns1");
	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	//webSocket.begin( "192.168.0.111", 8000 , "/ws/communication/esp/id");
  webSocket.beginSSL(API_HOST, PORT , PATH);

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user-esp", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(10000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 10);

}
unsigned long previousMillis = 0; 
const long interval = 10000;  

unsigned long previousFanMillis = 0; 
const long fan_interval = 1000 * 60 * 60;  // 60 dakika (60 * 60 * 1000 ms)
const long fan_duration = 1000 * 60 * 5;  // 5 dakika (5 * 60 * 1000 ms)

void loop() {


 
	webSocket.loop();

  unsigned long currentMillis = millis();

  int har_sensor_value = digitalRead(har_sensor); 
 
  if (har_sensor_value == 1){

    if (currentMillis - previousMillis >= interval) {
      digitalWrite(sensor_hand_trigger_led,HIGH);
      //delay(1000);
      digitalWrite(sensor_hand_trigger_led,LOW);
      previousMillis = currentMillis;
      send_key_status(2, true);
    }
  }

  int sensor_hand_value = digitalRead(sensor_hand_pin); 

  if ( sensor_hand_value == 0){
  //	USE_SERIAL.println("hand triger" );
    change_master_key_status();
    delay(500);
    previousMillis = currentMillis;

  }


  // Belirli bir süre geçti mi kontrol edin
  if (currentMillis - previousFanMillis >= fan_interval) {
    // Fanı belirli bir süre güçlendirin
    if (currentMillis - previousFanMillis < fan_interval + fan_duration) {
      // Fan güç veriliyor
      Serial.println("Fan güçlendiriliyor");
      COLLER_FAN_STATUS = true;
    } else {
      // Fan güç kesiliyor
      Serial.println("Fan güç kesiliyor");
      COLLER_FAN_STATUS = false;
      previousFanMillis = currentMillis;  // Sonraki güç verme için zamanı güncelle
    }
    status_apply();
  }

}


void change_master_key_status(){
    
  LAMBA_STATUS = !LAMBA_STATUS;
  status_apply();
  
  send_key_status(1, LAMBA_STATUS);
  
} 

void status_apply(){
  if (LAMBA_STATUS){
    pinMode(LAMBA_PIN,OUTPUT);
    digitalWrite(LAMBA_PIN,LOW);
    
  }else{
    pinMode(LAMBA_PIN,INPUT);
    digitalWrite(LAMBA_PIN,HIGH);
  }
  if (RGB_STATUS){
    pinMode(RGB_PIN,OUTPUT);
    digitalWrite(RGB_PIN,LOW);

  }else{
    pinMode(RGB_PIN,INPUT);
    digitalWrite(RGB_PIN,HIGH);
  
  }
  if (COLLER_FAN_STATUS){
    pinMode(coller_fan_pin,OUTPUT);
    digitalWrite(coller_fan_pin,LOW);

  }else{
    pinMode(coller_fan_pin,INPUT);
    digitalWrite(coller_fan_pin,HIGH);
  
  }
  if (SERVER_STATUS){
      open_server();
  }else{
      USE_SERIAL.print("sercer c");

    pinMode(server_pin,INPUT);
    digitalWrite(server_pin,HIGH);
  }
  
}

void open_server(){
      USE_SERIAL.print("SERVER S");

    pinMode(server_pin,OUTPUT);
    digitalWrite(server_pin,LOW);
    delay(100);
    pinMode(server_pin,INPUT);
    digitalWrite(server_pin,HIGH);
      USE_SERIAL.print("SERVER s e");

}


void send_key_status(int pin_id, bool status){
  if(webSocket.isConnected()){
    String json = "{\"type\":\"set_master_key\",\"status\":\"" + String(status) + "\",\"id\":\"" + String(pin_id) + "\"}";
    webSocket.sendTXT(json);
  }

}

