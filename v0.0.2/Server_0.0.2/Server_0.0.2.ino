#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>

// define debug as true to debug the module using the UART TX and RX pins
#define debug

// use this only of real internet connection is needed
// use to enable control over internet (IoT)
// TODO: test this function
// const char* stassid = "xxxxxxxxxx";
// const char* stapassword = "xxxxxxxxxx";

// internal AP (only my modules are in this network)
// enables communication betweet all modules without
// internet and additional hardware
IPAddress ip( 192, 168, 0, 1 );
IPAddress gateway( 192, 168, 1, 1);
IPAddress subnet( 255, 255, 255, 0 );
const String apssid = "ESP8266-Wifi-Server-Prototype";
const String appassword = "xxxxxxxxxx";

// start websockets-server on port
// TODO: check for free ports
const int port = 81;
WebSocketsServer server = WebSocketsServer( port );

void doSomething(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            Serial.printf("[%u] Connected from url: %s\n", num, payload);
            server.sendTXT(num, "Connected");
            
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);
            break;
    }
}

void setup(void){
  delay(1000);
  WiFi.disconnect();
  delay(100);

  // begin serial communication
  Serial.begin(9600);
  delay( 500 );

  #ifdef debug
  Serial.println();
  Serial.println("+------------------------------------------------+");
  Serial.println("| DEBUGGING-MODE                                 |");
  Serial.println("|                                                |");
  Serial.println("| SERVER                                         |");
  Serial.println("| ESP8266 Modul for standalone wifi-comunication |");
  Serial.println("| between several low-level-wifi-devices         |");
  Serial.println("|                                                |");
  Serial.println("| Version 0.0.2 ( 2016/11/25 - 22:15 )           |");
  Serial.println("| Christophger Koch ( chris@koch.codes )         |");
  Serial.println("| https://www.koch.codes                         |");
  Serial.println("+------------------------------------------------+");
  Serial.println();
  #endif
  
  // Start AP
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(apssid.c_str(),appassword.c_str());
  #ifdef debug
  Serial.println("WiFi AP running");
  Serial.print  ("SSID:     ");
  Serial.println( apssid );
  Serial.print  ("Password: ");
  Serial.println( appassword );
  Serial.println();
  #endif

  // start websocket-server
  server.begin();
  server.onEvent( doSomething );
  #ifdef debug
  Serial.println("WebSockets-server started");
  Serial.print  ("IP:       ");
  Serial.println( WiFi.softAPIP() );
  Serial.print  ("Port:     ");
  Serial.println( port );
  #endif
}

void loop(void){
  server.loop();
  delay( 5000 );
  server.broadcastTXT("HALLO BITTE MELDEN");
}
