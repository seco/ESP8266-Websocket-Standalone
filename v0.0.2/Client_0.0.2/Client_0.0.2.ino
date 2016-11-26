#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>

// define debug as true to debug the module using the UART TX and RX pins
#define debug

const char* apssid     = "ESP8266-Wifi-Server-Prototype";
const char* appassword = "xxxxxxxxxx";

const char* serverip   = "192.168.0.1";
const int port         = 81;

WebSocketsClient client;

void doSomething(WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[WSc] Disconnected!");
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n",  payload);
            client.sendTXT("Connected");
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary lenght: %u\n", lenght);
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
  Serial.println("| CLIENT                                         |");
  Serial.println("| ESP8266 Modul for standalone wifi-comunication |");
  Serial.println("| between several low-level-wifi-devices         |");
  Serial.println("|                                                |");
  Serial.println("| Version 0.0.2 ( 2016/11/25 - 22:15 )           |");
  Serial.println("| Christophger Koch ( chris@koch.codes )         |");
  Serial.println("| https://www.koch.codes                         |");
  Serial.println("+------------------------------------------------+");
  Serial.println();
  Serial.println();
  #endif
  
  // connect to wifi
  #ifdef debug
  Serial.println("Connecting to WiFi");
  Serial.print  ("SSID:     ");
  Serial.println( apssid );
  Serial.print  ("Password: ");
  Serial.println( appassword );
  Serial.println();
  #endif
  WiFi.begin(apssid, appassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef debug
    Serial.print(".");
    #endif
  }
  #ifdef debug
  Serial.println();
  #endif
  
  #ifdef debug
  Serial.print  ("IP:       ");
  Serial.println(WiFi.localIP());
  Serial.println("connected");
  Serial.println();
  #endif

  #ifdef debug
  Serial.println("Connecting to Websocket-Server");
  Serial.print  ("IP:       ");
  Serial.println( serverip );
  Serial.print  ("Port:     ");
  Serial.println( port );
  #endif
  client.begin(serverip, port);
  client.onEvent(doSomething);
  #ifdef debug
  Serial.println("connected");
  Serial.println();
  #endif
}

void loop(void){
  client.loop();;
}
