#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Udp.h>
#include <WiFiUDP.h>
#include <ArduinoJson.h>
#include <Time.h>

// define debug as true to debug the module using the UART TX and RX pins
#define debug
#define internet

// use this only of real internet connection is needed
// use to enable control over internet (IoT)
// TODO: test this function
const char* stassid = "FRITZ!Box 6360 Cable";
const char* stapassword = "9331822416340669";

// internal AP (only my modules are in this network)
// enables communication betweet all modules without
// internet and additional hardware
IPAddress ip( 192, 168, 0, 1 );
IPAddress gateway( 192, 168, 1, 1);
IPAddress subnet( 255, 255, 255, 0 );
const String apssid = "ESP8266-Wifi-Server-Prototype";
const String appassword = "9331822416340669";

// start websockets-server on port
// TODO: check for free ports
const int port = 81;
WebSocketsServer server = WebSocketsServer( port );

// UDP for RTC from NTP server
#ifdef internet
WiFiUDP udp;
#endif

#define bufferSize 500
DynamicJsonBuffer JSONBuffer;

// Variable to hold the current time
extern "C"{
  #include "user_interface.h"
}
os_timer_t t;
int local_time = 0;
#ifdef internet
bool requestTime = true;
#endif
void timerCallback( void *pArg ){
  local_time++;
  #ifdef internet
  if( local_time % 3600 == 0 )
    requestTime = true;
  #endif
}

#ifdef internet
int ntpUnixTime (UDP &udp){
  static int udpInited = udp.begin(123);
  const char timeServer[] = "pool.ntp.org"; 
  const long ntpFirstFourBytes = 0xEC0600E3;

  if (! udpInited)
    return 0;
  udp.flush();

  if (! (udp.beginPacket(timeServer, 123)
   && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
   && udp.endPacket()))
    return 0;

  const int pollIntv = 150;
  const byte maxPoll = 15;
  int pktLen;
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  int time = udp.read();
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  time += (udp.read() > 115 - pollIntv/8);

  udp.flush();

  return time - 2208988800ul;
}
#endif

int getTime( ){
  return local_time;
}

JsonObject& jsonToObject( char* s ){
  return JSONBuffer.parseObject( s );
}

char * objectToJson( JsonObject& o ){
  char s[bufferSize];
  memset(s, 0x00, bufferSize);   // this is necessary to set the char* with an ending zero, otherwise there will be a stack overflow
  o.printTo( s, sizeof(s) );
  return s;
}

void doSomething(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    char message[lenght];
    for( size_t i = 0; i < lenght; ++i ){
        message[i] = (char)payload[i];
    }

    switch(type) {
        case WStype_DISCONNECTED:
          {
            Serial.printf("[%u] Disconnected!\n", num);
          }
            break;
        case WStype_CONNECTED:
          {
            JsonObject& data = JSONBuffer.createObject();
            data["type"] = "text";
            data["id"] = num;
            data["text"] = "connected";
            data["timestamp"] = getTime();
            server.sendTXT(num, objectToJson( data ));
          }
            break;
        case WStype_TEXT:
          {
            JsonObject& data = jsonToObject( message );
            #ifdef debug
            String type = data["type"];
            Serial.print( "type: " );
            Serial.println( type );
            bool text = data["text"];
            Serial.print( "text: ");
            Serial.println( text );
            int id = data["id"];
            Serial.print( "id: ");
            Serial.println( id );
            unsigned long int timestamp = data["timestamp"];
            Serial.print( "timestamp: ");
            Serial.println( timestamp );
            #endif
          }
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
  Serial.println("| Version 0.0.3 ( 2016/11/25 - 22:15 )           |");
  Serial.println("| Christophger Koch ( chris@koch.codes )         |");
  Serial.println("| https://www.koch.codes                         |");
  Serial.println("+------------------------------------------------+");
  Serial.println();
  #endif

  #ifdef internet
  // Connect to Router
  WiFi.begin( stassid, stapassword );
  #ifdef debug
  Serial.println("Connecting to Wifi" );
  Serial.print  ("SSID:     ");
  Serial.println( stassid );
  Serial.print  ("Password: ");
  Serial.println( stapassword );
  Serial.println();
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef debug
    Serial.print(".");
    #endif
  }
  Serial.println();
  Serial.print( "Signalstrength: " );
  Serial.println( WiFi.RSSI() );
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

  os_timer_setfn(&t, timerCallback, NULL );
  os_timer_arm(&t, 1000, true);

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
  #ifdef internet
  if( requestTime ){
    local_time = ntpUnixTime( udp );
    requestTime = false;
  }
  #endif
}
