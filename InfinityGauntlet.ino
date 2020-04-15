#include <SoftwareSerial.h>
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
String id;

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
  wifiSerial.begin(115200);
  setupWeight();
  setupAcelerometro();
  setupBPM();
  antTimeMetric = actTimeMetric = millis();
  antPausedSeries = actPausedSeries = millis();
  id = "";
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
    String data = wifiSerial.readStringUntil('\n');
    switch(data.charAt(0)){
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
      wifiSerial.print("2#");
      readRoutine();
    }
  }
}

void readRoutine(){  
  while(!wifiSerial.available());
  String data = wifiSerial.readStringUntil(',');
  ejercicio = data.toInt();
  if(ejercicio == -1){
    Serial.println("#######Serie finalizada######");
    clearSerial();
    return;
  }
  data = wifiSerial.readStringUntil(',');
  series = data.toInt();
  data = wifiSerial.readString();
  repeticiones = data.toInt();
  Serial.println(ejercicio);
  Serial.println(series);
  Serial.println(repeticiones);
  delay(1000);
  actualRep = 1;
  actualSerie = 1;
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
    if(X_out > -90 && Y_out > 40 && Z_out > -170){
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
    if(X_out < -170 && Y_out < 20 && Z_out < -185){
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
    if(X_out > 200 && Y_out > -50 && Z_out > -100){
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
    if(X_out < -90 && Y_out < -90 && Z_out < -200){
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
  String cadena = "3#";
  cadena += id ;
  cadena +=",";
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
  cadena += repeticiones ;
  wifiSerial.println(cadena);
}

void rightRepetition(){
  rightBuzzer();  
  sendRepeticion(true); 
  actualRep++;
}

void wrongRepetition(){
  wrongBuzzer();
  sendRepeticion(false);
  //errores++;
  if(errores > 2){
    antPausedSeries = millis();
    isPausedSeries = true;
    errores = 0;
    Serial.println("####Pausa Obligatoria");
  }
}

void sendRepeticion(boolean estado){
  String cadena = "1#";
  cadena += "{ \"ejercicio\" : "; 
  cadena+= ejercicio;
  cadena += ", \"actualSerie\": "; 
  cadena += actualSerie;
  cadena += ", \"actualRep\" : "; 
  cadena += actualRep;
  cadena += ", \"actualSerie\": ";
  cadena = actualSerie;
  cadena += ", \"actualRep\" : "; 
  cadena += actualRep;
  
  if(estado) 
    cadena += ", \"estado\" : true }";
  else
    cadena += ", \"estado\" : false }";
  wifiSerial.println(cadena);
}
void rightBuzzer(){
  for (i=fMin;i<=fMax; i++)
     tone(22, i, duracion);
  noTone(22);
}

void wrongBuzzer(){
  for (i=fMax;i>=fMin; i--)
    tone(22, i, duracion);
  noTone(22);
}

void clearSerial(){
  while(wifiSerial.available())
    wifiSerial.read();
}
