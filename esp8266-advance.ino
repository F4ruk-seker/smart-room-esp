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


class DigitalRole {
  public:
    bool status;
    const pin;
}

