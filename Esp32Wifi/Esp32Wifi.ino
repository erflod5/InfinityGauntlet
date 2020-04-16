#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

const char* ssid = "CLARO_92c9b0";
const char* password =  "16DF6EFCDE";
const char* id;
const char* date;
WebSocketClient webSocketClient;
WiFiClient client;
boolean handshakeFailed=0;
StaticJsonDocument<1024> routine;
JsonObject actualEx;
int contador = 0;

void setup() {
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
//    Serial.println("Connecting top WiFi..");
  }
//  Serial.println("Connected to the WiFi network");
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
      nextExercise();
      clearSerial();
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
    String data = Serial.readStringUntil('\n');
    data += id;
    data += "}";
    HTTPClient http;
    
    http.begin("http://13.59.203.226/");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
   
    int httpCode = http.POST(data);   //Send the request
    String payload = http.getString();
   
    Serial.println(httpCode);
    Serial.println(payload);
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
          DeserializationError err = deserializeJson(routine, data);
          if (err) {
//            Serial.print(F("deserializeJson() failed with code "));
//            Serial.println(err.c_str());
          }
          else{
//            Serial.println("Json correcto");
            contador = 0;
            Serial.println("3");
            nextExercise();
          }
      }
      else{
        Serial.println(data);
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

void nextExercise(){
  
//  Serial.print("Num: ");
//  Serial.println(contador);
  
  actualEx = routine[contador++];
  if(actualEx.isNull()){
    
   // Serial.println("Ultimo");
   Serial.println("5,0,0");
  }
  else{
   int reps = actualEx["reps"];
    int series = actualEx["series"];
    int exercise = actualEx["exercise"];
//    id = actualEx["id"];
//    date = actualEx["date"];
    String data ="";
    data+= exercise;
    data+=",";
    data += series;
    data+=",";
    data += reps;
    Serial.println(data);
//    Serial.print("Ejercicio: ");
//    Serial.println(exercise);
//    Serial.print("Series: ");
//    Serial.println(series);
//    Serial.print("Repeticiones: ");
//    Serial.println(reps);
//    Serial.print("Id: ");
//    Serial.println(id);
//    Serial.print("date: ");
//    Serial.println(date );
  }
}

void clearSerial(){
  while(Serial.available()){
    Serial.read();
  }
}
