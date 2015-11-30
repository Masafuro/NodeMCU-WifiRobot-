#include "JWIO_ESP8266_ArduinoIDE.h"

//
// Constructor
//

jumpwireIo::jumpwireIo(const char* ssid, const char* password,const char* token,const char* project)
{
  ssid_private = ssid;
  password_private = password;
  token_private = token;
  project_private = project;
}

//
// API
//

void jumpwireIo::setup()
{
  if(debug){
    //ESP.restart();
    Serial.begin(115200);
    delay(10);
    Serial.setDebugOutput(true);
    Serial.println(ESP.getFreeHeap());
    Serial.println();
    Serial.println("START jumpwireIo::setup()");

  }
  ws_connect();
  pingtimer = millis();
  if(debug){
    Serial.println("END   jumpwireIo::setup()");
    Serial.println();
  }
}

void jumpwireIo::loop()
{ 
  if(ws_status == 4){  //Wifi error
    ws_connect();
  }else if(ws_status == 3){ //Websocket error
    httpConnect();
  }else if(ws_status == 2){ //Waiting for an incomming message
    parseWebsocket();
  }
  if(millis() - pingtimer > 55000){ //ping every 55 sec.
    pingtimer = millis();
    Ping();
  }
}

bool jumpwireIo::Throw(char key, float value)
{
  if(debug){

    Serial.print("Throw key: ");
    Serial.print(key);
    Serial.print(" value: ");
    Serial.println(value);
  }
  String frame;
  //frame.reserve(50); //to avoid fragmentation
  frame ="42[\"f\",[\"";
  frame +=key;
  frame +="\",";
  frame +=value;
  frame +="]]";
  return WebSocketSendText(frame);
}

//
// Connecting
//

void jumpwireIo::ws_connect()
{
  if(debug){
    Serial.println();
    Serial.println("START jumpwireIo::ws_connect()");
    Serial.print("Connecting Wifi : ");
    Serial.println(ssid_private);
  }
  WiFi.begin(ssid_private, password_private);
  // TODO: timeout
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debug){
      Serial.print(".");
    }
  }
  ws_status = 1;
  WiFi.printDiag(Serial);
  if(debug){
    Serial.println("OK");
    Serial.print("Local IP : ");
    Serial.println(WiFi.localIP());
  }
  httpConnect();
  if(debug){
    Serial.println("END   jumpwireIo::ws_connect()");
    Serial.println();
  }
}


void jumpwireIo::httpConnect(){

  if(debug){
    Serial.println();
    Serial.print("Connecting Websocket : ws://");
    Serial.print(host);
    Serial.print(":");
    Serial.print(port);
    Serial.println(path);
    Serial.print("Token : ");
    Serial.println(token_private);
    Serial.print("Project : ");
    Serial.println(project_private);
  }
  // TODO: timeout
  while (!client.connect(host, port)) {
    if(debug){
      Serial.print(".");
    }
  }
  client.print(
    String("GET ") + path + " HTTP/1.1\r\n" +
    "Host: " + host + ":"+ port + "\r\n" +
    "Upgrade: websocket\r\n"+
    "Connection: Upgrade\r\n"+
    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"+ //TODO: randamize
    "Sec-WebSocket-Version: 13\r\n"+
    "Cookie: v=1; t="+token_private+";p="+project_private+";n="+node+";\r\n"
    "\r\n"
  );
  delay(10);
  if(debug){
    Serial.println("HTTTP Response Header : ");
  }
  String httpHead;
  //httpHead.reserve(150); //to avoid fragmentation
  while(client.available()){
    char a = client.read();
    httpHead.concat(a);
    if(httpHead.endsWith("\r\n\r\n")){
      ws_status = 2;
      break;
    }
  }
  if(debug){
    Serial.print(httpHead);
  }
}


//
// Receiving
//

void jumpwireIo::parseWebsocket(){
  if(client.available()){
    if(client.read()==B10000001){//first char for Websocket txt must be B10000001
      //find out length of Websocket frame
      byte len = client.read();
      len &= B01111111; //drop first bit
      if(len>125){ //ignore long frame
        return;
      } 
      //read Websocket frame
      byte i;
      char a;
      String frame;
      frame.reserve(len); //to avoid fragmentation
      // TODO: timeout
      for (i=1;i<=len;i++){
        a = client.read();
        frame+=a;
      }
      parseSocketIo(frame);
    }
  }
}

void jumpwireIo::parseSocketIo(String frame){
  if(frame.charAt(0)=='3'){
    if(debug){

      Serial.print("  ok: received pong (round trip time = ");
      Serial.print(millis() - pingsent);
      Serial.println("ms)");
      Serial.println();
    }
  }else if((frame.charAt(0)=='4')&&(frame.charAt(1)=='2')){
    //pass TXT frame to the next.
    frame.remove(0,2);
    parseJumpwireIo(frame);
  }else{
    if(debug){
      Serial.print("Unknown socket.io frame ");
      Serial.println(frame);
    }
  }
}
void jumpwireIo::parseJumpwireIo(String frame){
  if(frame.charAt(2)=='f'){ //is f (float) frame?
    if(debug){
      Serial.print("Catch key: ");
      Serial.print(frame.charAt(7));
      Serial.print(" value: ");
      Serial.println(frame.substring(10,frame.length()-2).toFloat());
    }
    //call Catch function
    Catch(frame.charAt(7),
          frame.substring(10,frame.length()-2).toFloat());
  }else{
    if(debug){
      Serial.print("Unknown frame ");
      Serial.println(frame);
    }
  }
}

//
// Sending
//

void jumpwireIo::Ping(){
  if(debug){
      Serial.println();
      Serial.print("WiFi status : ");
      Serial.println(WiFi.status());
      Serial.print("Websocket status : ");
      Serial.println(client.connected());
      Serial.print("Free Heap : ");
      Serial.println(ESP.getFreeHeap());
      Serial.println("Sending ping...");
    }
  pingsent = millis();
  WebSocketSendText(String('2'));
}

bool jumpwireIo::WebSocketSendText(String frame)
{
  if(WiFi.status() != WL_CONNECTED){ //Wifi Check but
                                     //Usually ESP automatically reboots when wifi disconnect
    if(debug){
      Serial.println("ERR: WiFi disconnected");
    }
    ws_status = 4;
    return false;
  }
  if(!client.connected()){
    if(debug){
      Serial.println("ERR: Websocket disconnected");
    }
    ws_status = 3;
    return false;
  }
  char WebSocketFrame[6 + frame.length() + 1]; //head + paylod + null char
  WebSocketFrame[0] = B10000001; //FIN(1),RSV(000),opcode(0001:TXT)
  WebSocketFrame[1] = B10000000; //MASK(1)
  WebSocketFrame[1] = WebSocketFrame[1] + frame.length(); //set payload length
  WebSocketFrame[2] = random(-128, 128); //MASK
  WebSocketFrame[3] = random(-128, 128); //MASK
  WebSocketFrame[4] = random(-128, 128); //MASK
  WebSocketFrame[5] = random(-128, 128); //MASK
  byte i = 0;
  for (i = 0; i < frame.length(); i++) {
    WebSocketFrame[6 + i] = frame[i] ^ WebSocketFrame[i % 4 + 2];
  }
  WebSocketFrame[6 + frame.length()] = B00000000; //null char
  client.print(WebSocketFrame);
  return true;
}




