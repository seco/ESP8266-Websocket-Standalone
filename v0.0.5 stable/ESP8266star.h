#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Udp.h>
#include <WiFiUDP.h>
#include <ArduinoJson.h>
#include <Time.h>

#define TAG_DISCONNECT  0
#define TAG_CONNECT     1
#define TAG_MESSAGE     2

extern "C"{
  #include "user_interface.h"
}

// define debug as true to debug the module using the UART TX and RX pins
#define debug
#define bufferSize 500

class ESP8266star{
    int local_time;
    static ESP8266star* instance();
    bool requestTime;
    
    
    os_timer_t t;
    
    char* wifi_ssid;
    char* wifi_password;
    char* ap_ssid;
    char* ap_password;
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnetmask;
    int port;
    WebSocketsServer *server;
    WiFiUDP udp;
    DynamicJsonBuffer JSONBuffer;
    
protected:
    ESP8266star();
    
public:

    void startWebsocketsServer( );
    void connectToWifi();
    void startAccessPoint();
    void sendMessageToClient( JsonObject&, int );
    void logOnSockets( String );
    
    static ESP8266star& Instance();
    void setWifiSSID( char* );
    void setWifiPassword( char* );
    void setAPSSID( char* );
    void setAPPassword( char* );
    void setAPIP( int, int, int, int );
    void setAPGateway( int, int, int, int );
    void setAPSubnetmask( int, int, int, int );
    void setWebsocketsPort( int );
    
    void timerEvent();
    
    void replyTime( int );
    void cycle();
    int getTime();
    int getTimeFromServer( UDP& );
    JsonObject& jsonToObject( char* );
    char* objectToJson( JsonObject& );
    void timerCycle();
    void timerCallback( void );
    void messageHandler( uint8_t, WStype_t, uint8_t*, size_t );
    void setup();
    void loop();
};