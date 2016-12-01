#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define relayPin 2

// define debug as true to debug the module using the UART TX and RX pins
#define debug

int clientId = -1;

const char* apssid     = "ESP8266-Wifi-Server-Prototype";
const char* appassword = "9331822416340669";

const char* serverip   = "192.168.0.1";
const int port         = 81;

WebSocketsClient client;

#define bufferSize 500
DynamicJsonBuffer JSONBuffer;

JsonObject& jsonToObject( char* s ){
  return JSONBuffer.parseObject( s );
}

char * objectToJson( JsonObject& o ){
  char s[bufferSize];
  memset(s, 0x00, bufferSize);   // this is necessary to set the char* with an ending zero, otherwise there will be a stack overflow
  o.printTo( s, sizeof(s) );
  return s;
}

void signIn( JsonObject& data ){
  clientId = data["text"];
  data["type"] = 1; // sign in
  data["text"] = "relay";
  data["id"] = clientId;
  client.sendTXT(objectToJson( data ));
}

void setSwitch( JsonObject& data ){
  bool swStatus = data["text"];
  Serial.print( "Set switch to <" );
  Serial.print( swStatus );
  Serial.println( ">" );
  if( swStatus )
    digitalWrite( relayPin, HIGH );
  else
    digitalWrite( relayPin, LOW );
}

void doSomething(WStype_t type, uint8_t * payload, size_t lenght) {

  char message[lenght];
    for( size_t i = 0; i < lenght; ++i ){
        message[i] = (char)payload[i];
    }
  switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[WSc] Disconnected!");
            break;
        case WStype_CONNECTED:
            {
                JsonObject& data = JSONBuffer.createObject();
                data["type"] = 1;
                data["text"] = "relay";
                data["id"] = clientId;
                client.sendTXT(objectToJson( data ));
            }
            break;
        case WStype_TEXT:
            {
                JsonObject& data = jsonToObject( message );
                int type = data["type"];
                switch( type ){
                  case 0:   // force to sign in
                    signIn( data );
                    break;
                  case 2:   // switch for simple relay
                    setSwitch( data );
                    break;
                }
                #ifdef debug
                {
                  String type = data["type"];
                  Serial.print( "type: " );
                  Serial.println( type ); bool text = data["text"];
                  Serial.print( "text: ");
                  Serial.println( text );int id = data["id"];
                  Serial.print( "id: ");
                  Serial.println( id );unsigned long int timestamp = data["timestamp"];
                  Serial.print( "timestamp: ");
                  Serial.println( timestamp );
                }
                #endif
            }
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary lenght: %u\n", lenght);
            hexdump(payload, lenght);
            break;
    }

}

void setup(void){

  pinMode( relayPin, OUTPUT );
  
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
