#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

const char* ssid = "CLARO_92c9b0";
const char* password =  "16DF6EFCDE";
WebSocketClient webSocketClient;
WiFiClient client;
boolean handshakeFailed=0;

void setup() {
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
  }
  wsconnect();
}

void loop() {
  if(Serial.available() > 0){
    String estado = Serial.readStringUntil('#');
    if(estado.charAt(0) == '1'){
      sendData();
      
      clearSerial();
    }
    else if(estado.charAt(0) == '2'){
      //Fin ejerccios
    }
    else if(estado.charAt(0) == '3'){
      String message = Serial.readStringUntil('\n');
      webSocketClient.sendData(message);
    }else{
      clearSerial();
    }
  }
  handleSocket();
}

void sendData(){
  
  if (WiFi.status() == WL_CONNECTED) {
     StaticJsonDocument<256> doc;
    String JSONmessageBuffer = Serial.readStringUntil('\n');
    JsonObject root = doc.to<JsonObject>();
    //serializeJsonPretty(root, JSONmessageBuffer);
    HTTPClient http;    
    http.begin("http://13.59.203.226:3000/rep");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type heade   
    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();
    http.end();  //Close connection*/
  } 
  else {
    Serial.println("Error in WiFi connection");
  }
  clearSerial();
}

void handleSocket(){
  if (client.connected()) {
    String data;
    webSocketClient.getData(data);    
    if (data.length() > 0) {
      if(data.charAt(0) == '['){
          Serial.print('3');
          Serial.print(data);
          Serial.print('#'); 
      }
      else{
        switch(data.charAt(0)){
          case '0': //Pause
            Serial.print('0');
            break;
          case '1': //Continue
            Serial.print('1');
            break;
          case '2': //Finish
            Serial.print('2');
            break;
        }
      }
    }
  }
}

void wsconnect(){
  if (client.connect("13.59.203.226", 3000)) {
//    Serial.println("Connected");
  } 
  else {
//    Serial.println("Connection failed.");
    delay(1000);
    if(handshakeFailed){
      handshakeFailed=0;
      ESP.restart();
    }
    handshakeFailed=1;
  }
  
  // Handshake with the server
  webSocketClient.path = "/";
  webSocketClient.host = "http://13.59.203.226";
  if (webSocketClient.handshake(client)) {
//    Serial.println("Handshake successful");
  } 
  else { 
//    Serial.println("Handshake failed.");
    delay(4000);  
    if(handshakeFailed){
      handshakeFailed=0;
      ESP.restart();
    }
    handshakeFailed=1;
  }
}

void clearSerial(){
  while(Serial.available()){
    Serial.read();
  }
}
