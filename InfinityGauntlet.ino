#include <SPI.h>

#include <ArduinoJson.h>
StaticJsonDocument<1024> routine;
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

const long pausedSeries = 20000;
int id;

const int buzzerPin=22;
int duracion=250; //Duración del sonido
int fMin=2000; //Frecuencia más baja
int fMax=4000; //Frecuencia más alta
int i=0;

const int led1 = 8; //Correcto
const int led2 = 9; //
const int buzzer = 10;
const char* fecha;
/*
 The circuit:
 * RX is digital pin 2 (connect to TX of other device)
 * TX is digital pin 3 (connect to RX of other device)
*/

void setup(){
  pinMode(buzzer,OUTPUT);
  
  Serial.begin(115200);
  Serial1.begin(115200);
  setupWeight();
  setupAcelerometro();
  setupBPM();
  antTimeMetric = actTimeMetric = millis();
  antPausedSeries = actPausedSeries = millis();
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
}

void loop() {
  readSerial();
  if(!isPaused && !isPausedSeries){
    digitalWrite(led1,LOW);
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
  else{
    digitalWrite(led1,HIGH);
  }
  
  actPausedSeries = millis();
  if(isPausedSeries && (actPausedSeries - antPausedSeries) > pausedSeries){
    antPausedSeries = actPausedSeries;
    isPausedSeries = false;
    digitalWrite(led2,LOW);
  }
  
  actTimeMetric = millis();
  if(actTimeMetric - antTimeMetric > 1000){
    sendMetrics(); 
    antTimeMetric = actTimeMetric;
  }
}

//SOCKETS
void readSerial(){
  if(Serial1.available() > 0){
    char data = Serial1.read();
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
    digitalWrite(led2,HIGH);
    if(actualSerie > series){
      actualSerie = 0;
      Serial.println("####Waiting####");
      nextExercise();
    }
  }
}

void readRoutine(){  
  while(!Serial1.available());
  delay(1);
  String rutinaData = "";
  while(true){
    if(Serial1.available() > 0){
      char data = Serial1.read();
      if(data == '#')
        break;
      rutinaData += data;
    }
  }
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
    fecha = actualEx["date"];
    actualRep = 1;
    actualSerie = 1;
    errores = 0;
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
    if(X_out > -80 && Y_out > 180 && Z_out > -120){
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
    if(X_out < -200 && Y_out < 80 && Z_out < -60){
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
    if(X_out > 200 && Y_out > -60 && Z_out > -80){
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
    if(X_out < -180 && Y_out < -30 && Z_out < -120){
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
    if(X_out > -90 && Y_out > 120 && Z_out > 150){
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
    if(X_out < -200 && Y_out < -5 && Z_out < -30){
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
  Serial1.println(cadena);
}

void rightRepetition(){
  rightBuzzer();  
  sendRepeticion(true); 
  actualRep++;
}

void wrongRepetition(){
  wrongBuzzer();
  sendRepeticion(false);
  errores++;
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
  
  cadena += ", \"id_rutina\": "; 
  cadena += id;
  
  cadena += ", \"serie\": "; 
  cadena += actualSerie;
  
  cadena += ", \"numero_repeticion\" : "; 
  cadena += actualRep;
  
  cadena += ", \"BPM\" : "; 
  cadena += getBPM();

  cadena += ", \"peso\" : "; 
  cadena += getWeight();

  cadena += ", \"fecha\" : \""; 
  cadena += fecha;
  cadena += "\"";
  if(estado) 
    cadena += ", \"completado\" : true}";
  else
    cadena += ", \"completado\" : false}";
  Serial.println(cadena);
  Serial1.println(cadena);
}

void rightBuzzer(){
  

  for(int j = 0; j <= 250; j++){
    digitalWrite(buzzer,HIGH);
    delayMicroseconds(1500);

    digitalWrite(buzzer,LOW);
    delayMicroseconds(1500);
  }
}

void wrongBuzzer(){

  for(int j = 0; j <= 250; j++){
    digitalWrite(buzzer,HIGH);
    delayMicroseconds(1000);

    digitalWrite(buzzer,LOW);
    delayMicroseconds(1000);
  }

}

void clearSerial(){
  while(Serial1.available()){
    char x = Serial1.read();
    Serial.print(x);
  }
  Serial.println();
}
