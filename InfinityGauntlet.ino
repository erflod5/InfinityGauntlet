#include <SoftwareSerial.h>
#include <ArduinoJson.h>
StaticJsonDocument<512> routine;
JsonObject actualEx;
int contador = 0;
float X_out, Y_out, Z_out;

int ejercicio =-1;
int series=-1;
int repeticiones=-1;
int errores=-1;

int actualSerie=0;
int actualRep=0;
boolean isPaused;

long antTimeMetric;
long actTimeMetric;

long antPausedSeries;
long actPausedSeries;
boolean isPausedSeries = false;

const long pausedSeries = 10000;
int id;

const int buzzerPin=22;
int duracion=250; //Duración del sonido
int fMin=2000; //Frecuencia más baja
int fMax=4000; //Frecuencia más alta
int i=0;

/*
 The circuit:
 * RX is digital pin 2 (connect to TX of other device)
 * TX is digital pin 3 (connect to RX of other device)
*/
SoftwareSerial wifiSerial(2,3); 

void setup(){
  Serial.begin(9600);
  wifiSerial.begin(9600);
  setupWeight();
  setupAcelerometro();
  setupBPM();
  antTimeMetric = actTimeMetric = millis();
  antPausedSeries = actPausedSeries = millis();
}

void loop() {
  readSerial();
  if(!isPaused && !isPausedSeries){
    switch(ejercicio){
      case 1:
        exercise1();
        break;
      case 2:
        exercise2();
        break;
      case 3:
        exercise3();
        break;
    }
  }
  
  actPausedSeries = millis();
  if(isPausedSeries && (actPausedSeries - antPausedSeries) > pausedSeries){
    antPausedSeries = actPausedSeries;
    isPausedSeries = false;
  }
  
  actTimeMetric = millis();
  if(actTimeMetric - antTimeMetric > 1000){
    sendMetrics(); 
    antTimeMetric = actTimeMetric;
  }
}

//SOCKETS
void readSerial(){
  if(wifiSerial.available() > 0){
    char data = wifiSerial.read();
    Serial.println(data);
    switch(data){
      case '0': //Pause
        Serial.println("#####Pausa#####\n\n");
        isPaused = true;
        break;
      case '1': //Continue
        Serial.println("####Continuar####\n\n");
        isPaused = false;
        break;
      case '2': //Finish
        Serial.println("####Terminar####\n\n");
        isPaused = true;
        ejercicio = series = repeticiones = -1;
        break;
      case '3': //Starting routine
        Serial.println("####Empezando rutina...####\n\n");
        readRoutine();
        isPaused = isPausedSeries = false;
        break;
      default:
        String cadena = Serial.readString();
        Serial.println(cadena);
        break;
    }
    clearSerial();
  }
}

void next(){
  if(actualRep > repeticiones){
    actualRep = 0;
    actualSerie++;
    isPausedSeries = true;
    antPausedSeries = millis();
    if(actualSerie > series){
      actualSerie = 0;
      Serial.println("####Waiting####");
      nextExercise();
    }
  }
}

void readRoutine(){  
  while(!wifiSerial.available());
  delay(1);
  String rutinaData = Serial.readStringUntil('\n');
  Serial.println(rutinaData);
  DeserializationError err = deserializeJson(routine, rutinaData);
  if (err) {
    Serial.println("JSON error");
  }
  else{
    Serial.println("JSON correcto");
    contador = 0;
    nextExercise();
  }
}

void nextExercise(){
  actualEx = routine[contador++]; 
  if(actualEx.isNull()){
    Serial.println("Rutina terminada");
    ejercicio = -1;
  }
  else{
    repeticiones = actualEx["reps"];
    series = actualEx["series"];
    ejercicio = actualEx["exercise"];
    id = actualEx["routine"];
    actualRep = 1;
    actualSerie = 1;
    Serial.println(ejercicio);
    Serial.println(series);
    Serial.println(repeticiones);
    Serial.println(id);
  }
}

void exercise1(){
  loopAcelerometro();
  boolean goingUp = true;
  float x = X_out, y = Y_out, z = Z_out;
  long antTiempoRep = millis(), actTiempoRep = millis();
    
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 6000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out > 200 && Y_out > 0 && Z_out > -75){
      goingUp = false;
      Serial.println("### Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out < -90 && Y_out < -20 && Z_out < -200){
      Serial.println("### Bajada correcta");
      rightRepetition();
      next();
      delay(100);
      break;
    } 
  }
}

void exercise2(){
  loopAcelerometro();
  boolean goingUp = true;
  float x = X_out, y = Y_out, z = Z_out;
  long antTiempoRep = millis(), actTiempoRep = millis();
    
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 6000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out > -50 && Y_out > 170 && Z_out > -130){
      goingUp = false;
      Serial.println("### Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out < -180 && Y_out < 100 && Z_out < -95){
      Serial.println("### Bajada correcta");
      rightRepetition();
      next();
      delay(100);
      break;
    } 
  }
}

void exercise3(){
loopAcelerometro();
  boolean goingUp = true;
  float x = X_out, y = Y_out, z = Z_out;
  long antTiempoRep = millis(), actTiempoRep = millis();
    
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 6000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out > -90 && Y_out > 130 && Z_out > 110){
      goingUp = false;
      Serial.println("### Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if(X_out < -200 && Y_out < -10 && Z_out < -10){
      Serial.println("### Bajada correcta");
      rightRepetition();
      next();
      delay(100);
      break;
    } 
  }
}

void sendMetrics(){
  int bpm = getBPM();
  float weight = getWeight();
  String cadena = "3#{\"data\":\"";
  cadena += bpm ;
  cadena +=",";
  cadena += weight ;
  cadena +=",";
  cadena += ejercicio ;
  cadena +=",";
  cadena += actualSerie ;
  cadena +=",";
  cadena += actualRep ;
  cadena +=",";
  cadena += series ;
  cadena +=",";
  cadena += repeticiones;
  cadena += "\"}";
  Serial.println(cadena);
  wifiSerial.println(cadena);
}

void rightRepetition(){
  rightBuzzer();  
 // sendRepeticion(true); 
  actualRep++;
}

void wrongRepetition(){
  wrongBuzzer();
 // sendRepeticion(false);
  //errores++;
  if(errores > 2){
    antPausedSeries = millis();
    isPausedSeries = true;
    errores = 0;
    Serial.println("####Pausa Obligatoria");
  }
}

/*{
  "id_rutina":53,
  "id_ejercicio":2,
  "serie":2,
  "numero_repeticion":8,
  "completado":true,
  "BPM":85,
  "peso":10.2
}*/

void sendRepeticion(boolean estado){
  String cadena = "1#";
  cadena += "{ \"id_ejercicio\" : "; 
  cadena+= ejercicio;
  
  cadena += ", \"serie\": "; 
  cadena += actualSerie;
  
  cadena += ", \"numero_repeticion\" : "; 
  cadena += actualRep;
  
  cadena += ", \"BPM\" : "; 
  cadena += getBPM();

  cadena += ", \"peso\" : "; 
  cadena += getWeight();
  
  if(estado) 
    cadena += ", \"estado\" : true,";
  else
    cadena += ", \"estado\" : false, \"id\":";
  wifiSerial.println(cadena);
}

void rightBuzzer(){
  //tone(22,2000,500);
  //noTone(22);
  Serial.println("right buzzer...");
}

void wrongBuzzer(){
  //tone(22,1000,500);
  //noTone(22);
  Serial.println("wrong buzzer...");
}

void clearSerial(){
  while(wifiSerial.available()){
    char x = wifiSerial.read();
    Serial.print(x);
  }
  Serial.println();
}
