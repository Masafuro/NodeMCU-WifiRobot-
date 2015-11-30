
#ifndef jumpwire_io_ESP8266_Arduino_IDE_h
#define jumpwire_io_ESP8266_Arduino_IDE_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

void Catch(char key, float value);

class jumpwireIo
{
  public:
    jumpwireIo(const char* ssid, const char* password,const char* token,const char* project) ;
    void setup() ;
    void loop() ;
    bool Throw(char key, float value);
  private:
    WiFiClient  client;
    const bool  debug=true;
    const char* ssid_private;
    const char* password_private;
    const char* token_private;
    const char* project_private;
    const char* host = "socket.jumpwire.io"; //104.197.63.30
    const int   port = 80;
    const char* path = "/socket.io/?transport=websocket"; 
    const char* node = "__EA0.0.2";
    int         ws_status = 0;
    unsigned long pingtimer;
    unsigned long pingsent;
    void        ws_connect();
    void        httpConnect();
    void        parseWebsocket();
    void        parseSocketIo(String frame);
    void        parseJumpwireIo(String frame);
    bool        WebSocketSendText(String frame);
    void        Ping();
};

#endif
