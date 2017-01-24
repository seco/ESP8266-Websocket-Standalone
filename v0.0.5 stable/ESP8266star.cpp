#include "ESP8266star.h"

void ESP8266star_timerCallback( void *pArg ){
    ESP8266star::Instance().timerEvent( );
}

void ESP8266star_messageHandler( uint8_t num, WStype_t type, uint8_t * payload, size_t lenght ){
    ESP8266star::Instance().messageHandler( num, type, payload, lenght );
}

__attribute__ ((weak))
void esp8266star_json_message_event_callback( JsonObject& m ){

}

ESP8266star::ESP8266star(){
    local_time = 0;
    requestTime = true;
}

ESP8266star& ESP8266star::Instance(){
    static ESP8266star instance;
    return instance;
}

void ESP8266star::setWifiSSID( char* ssid ){
    this->wifi_ssid = ssid;
}

void ESP8266star::setWifiPassword( char* password){
    this->wifi_password = password;
}

void ESP8266star::setAPSSID( char* ssid){
    this->ap_ssid = ssid;
}

void ESP8266star::setAPPassword( char* password){
    this->ap_password = password;
}

void ESP8266star::setAPIP( int a, int b, int c, int d ){
    this->ip = IPAddress( a, b, c, d); 
}

void ESP8266star::setAPGateway( int a, int b, int c, int d ){
    this->gateway = IPAddress( a, b, c, d ); 
}

void ESP8266star::setAPSubnetmask( int a, int b, int c, int d ){
    this->subnetmask = IPAddress( a, b, c, d ); 
}

void ESP8266star::setWebsocketsPort( int port ){
    this->port = port;
}

JsonObject& ESP8266star::jsonToObject( char* s ){
     return JSONBuffer.parseObject( s );
}

char* ESP8266star::objectToJson( JsonObject& o ){
    char s[bufferSize];
    memset(s, '\0', bufferSize);
    o.printTo( s, sizeof(s) );
    return s;
}

void ESP8266star::messageHandler( uint8_t num, WStype_t type, uint8_t * payload, size_t lenght ){
    char message[lenght];
    for( size_t i = 0; i < lenght; ++i ){
        message[i] = (char)payload[i];
    }

    switch(type) {
        case WStype_DISCONNECTED:
          {
            JsonObject& data = JSONBuffer.createObject();
            data["type"] = TAG_DISCONNECT;
            data["id"] = num;
            esp8266star_json_message_event_callback( data );
          }
            break;
        case WStype_CONNECTED:
          {
            JsonObject& data = JSONBuffer.createObject();
            data["type"] = TAG_CONNECT;     // Force to sign in
            data["id"] = num;               // the id of the new module
            esp8266star_json_message_event_callback( data );
            server->sendTXT(num, objectToJson( data ));
          }
            break;
        case WStype_TEXT:
          {
            JsonObject& data = JSONBuffer.parseObject( message );
            data["type"] = TAG_MESSAGE;     // Force to sign in
            data["id"] = num;               // the id of the new module
            data["text"] = data["text"];
            esp8266star_json_message_event_callback( data );
          }
            break;
    }
}

void ESP8266star::logOnSockets( String s ){
    s = "{\"log\":\"" + s + "\",\"t\":" + millis() + "}";
    server->broadcastTXT( s );
}

void ESP8266star::sendMessageToClient( JsonObject& o, int id ){
    server->sendTXT(id, objectToJson( o ));
}

void ESP8266star::startWebsocketsServer( ){
    server = new WebSocketsServer( port );
    server->begin();
    server->onEvent( ESP8266star_messageHandler );
    #ifdef debug
    Serial.println("WebSockets-server started");
    Serial.print  ("IP:       ");
    Serial.println( WiFi.softAPIP() );
    Serial.print  ("Port:     ");
    Serial.println( port );
    #endif
}
void ESP8266star::connectToWifi(){
    WiFi.begin( wifi_ssid, wifi_password );
    #ifdef debug
    Serial.println("Connecting to Wifi" );
    Serial.print  ("SSID:     ");
    Serial.println( wifi_ssid );
    Serial.print  ("Password: ");
    Serial.println( wifi_password );
    Serial.println();
    #endif
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        #ifdef debug
        Serial.print(".");
        #endif
    }
    Serial.println();
    Serial.print  ("IP: ");
    Serial.println( WiFi.localIP() );
};
void ESP8266star::startAccessPoint(){
    WiFi.softAPConfig(ip, gateway, subnetmask);
    WiFi.softAP(ap_ssid,ap_password );
    #ifdef debug
    Serial.println("WiFi AP running");
    Serial.print  ("SSID:     ");
    Serial.println( ap_ssid );
    Serial.print  ("Password: ");
    Serial.println( ap_password );
    Serial.println();
    #endif
};


void ESP8266star::loop( ){
    server->loop();
}