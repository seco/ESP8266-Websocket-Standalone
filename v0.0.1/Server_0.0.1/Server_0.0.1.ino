#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// define debug as true to debug the module using the UART TX and RX pins
#define debug

// use this only of real internet connection is needed
// use to enable control over internet (IoT)
// TODO: test this function
// const char* stassid = "FRITZ!Box 6360 Cable";
// const char* stapassword = "9331822416340669";

// internal AP (only my modules are in this network)
// enables communication betweet all modules without
// internet and additional hardware
IPAddress ip( 192, 168, 0, 1 );
IPAddress gateway( 192, 168, 1, 1);
IPAddress subnet( 255, 255, 255, 0 );
const char* apssid = "ESP8266-Wifi-Server-Prototype";
const char* appassword = "9331822416340669";

// start webserver
// this is accessable for all modules and clients in real
// network and whole internet (only if router is used)
ESP8266WebServer server(80);

// not found handle-function fot the HTTP-server
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  #ifdef debug
  Serial.println( message );
  #endif
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
  Serial.println();
  Serial.println("+------------------------------------------------+");
  Serial.println("| DEBUGGING-MODE                                 |");
  Serial.println("|                                                |");
  Serial.println("| ESP8266 Modul for standalone wifi-comunication |");
  Serial.println("| between several low-level-wifi-devices         |");
  Serial.println("|                                                |");
  Serial.println("| Version 0.0.1 ( 2016/11/25 - 22:15 )           |");
  Serial.println("| Christophger Koch ( chris@koch.codes )         |");
  Serial.println("| https://www.koch.codes                         |");
  Serial.println("+------------------------------------------------+");
  Serial.println();
  Serial.println();
  #endif
  
  // Start AP
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(apssid,appassword);
  #ifdef debug
  Serial.println("AP started");
  #endif
 
  IPAddress myIP = WiFi.softAPIP();
  #ifdef debug
  Serial.print("AP IP address: ");
  #endif
  Serial.println(myIP);
  
  server.begin();
  #ifdef debug
  Serial.println("HTTP server started");
  #endif

  server.onNotFound( handleNotFound );
}

void loop(void){
  server.handleClient();
}
