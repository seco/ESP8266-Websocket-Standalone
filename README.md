# ESP8266-Websocket-Standalone
ESP8266 example to easey connect two or more ESP8266 modules without external router or other additional hardware
```
#include <ESP8266star.h>

ESP8266star* esp;

// this function will be called if a message in json-fomat 
// will be received by your websocket-server
void esp8266star_json_message_event_callback( JsonObject& m ){
  
  Serial.printf( "Message from client <%d> with type <%d>\n", int(m["id"]), int(m["type"]) );

  esp->logOnSockets( "Received data" );
  
  //JsonObject& response = JSONBuffer.createObject();
  //response["id"] = -1;
  //response["type"] = TAG_RESPONSE;
  
  switch( int(m["type"]) ){
    case TAG_CONNECT:
      //esp->sendMessageToClient( response, m["id"] );
      break;
    case TAG_DISCONNECT:
      break;
    case TAG_MESSAGE:
      String message = m["text"];
      //response["text"] = message;
      if( message == "true" )
        digitalWrite( 2, HIGH );
      else
        digitalWrite( 2, LOW );
      Serial.printf( "Content was <%s>\n", message.c_str() );
      esp->sendMessageToClient( m, m["id"] );
      break;
  }
}

void setup(){
  Serial.begin( 9600 );

  pinMode( 2, OUTPUT );
  
  esp = &ESP8266star::Instance();

  // Wifi Settings
  // use this if you want to connect to a existing wifi
  // or you want to get some data from the web
  esp->setWifiSSID( "Your Wifi your server should connect" );
  esp->setWifiPassword( "your password" );
  esp->connectToWifi();

  // AccessPoint Settings
  // use this to open a new wifi on which your other devices
  // can connect
  esp->setAPSSID( "TestWfi 0.0.5 stable" );
  esp->setAPPassword( "123456" );
  esp->setAPIP( 192, 168, 1, 1 );
  esp->setAPGateway( 192, 168, 178, 1 );
  esp->setAPSubnetmask( 255, 255, 255, 0 );
  esp->startAccessPoint();

  // WebsocketsServer Settings
  esp->setWebsocketsPort( 81 );
  esp->startWebsocketsServer();

  Serial.println( "Server is running!" );
}
void loop(){
  // esp->loop() is needed to receiving messages
  esp->loop();
}
```
