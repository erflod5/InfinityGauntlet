/*
  VALORES MINIMOS Y MAXIMOS
*/
float X_out, Y_out, Z_out;

const int min1[] = {-245,65,-30,-255,30,-20};
const int max1[] = {-230,80,-5,-240,50,-5};

const int min2[] = {230,-90,-70,-185,-60,-200};
const int max2[] = {255,-70,-40,-160,-40,-170};

/*
  ARRIBA: (-80,120,190),(-50,140,210)
  ABAJO: (-260,25,-50),(-240,55,-25) 
*/
const int min3[] = {-80,120,190,-260,25,-50};
const int max3[] = {-50,140,210,-240,55,-25};

int ejercicio;
int series;
int repeticiones;
int errores;

int actualSerie;
int actualRep;
boolean isPaused;

long antTimeMetric;
long actTimeMetric;

long antPausedSeries;
long actPausedSeries;
boolean isPausedSeries = false;

const int pausedSeries = 60000;
String id;
void setup(){
  Serial.begin(115200);
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
  if(isPausedSeries && (actPausedSeries - antPausedSeries > pausedSeries)){
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
  if(Serial.available() > 0){
    String data = Serial.readStringUntil('\n');
    switch(data.charAt(0)){
      case '0': //Pause
        Serial.println("9#Pausa");
        isPaused = true;
        break;
      case '1': //Continue
        Serial.println("9#Continuar");
        isPaused = false;
        break;
      case '2': //Finish
        Serial.println("9#Continuar");
        isPaused = true;
        ejercicio = series = repeticiones = -1;
        break;
      case '3': //Starting routine
        Serial.prinlnt("9#Empezando rutina...");
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
      Serial.println("9#Waiting");
      Serial.print("2#");
      readRoutine();
    }
  }
}

void readRoutine(){
  while(!Serial.available());
  String newEx = Serial.readString();
  Serial.println(newEx);
  ejercicio = -1;
}

void exercise1(){
  long antTiempoRep = millis();
  long actTiempoRep = millis();
  loopAcelerometro();
  boolean goingUp = true;
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min1[0] && X_out <= max1[0]) && (Y_out >= min1[1] && Y_out <= max1[1]) && (Z_out >= min1[2] && Z_out <= max1[2])){
      goingUp = false;
      Serial.println("9#Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  delay(500);
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 2000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min1[3] && X_out <= max1[3]) && (Y_out >= min1[4] && Y_out <= max1[4]) && (Z_out >= min1[5] && Z_out <= max1[5])){
      Serial.println("9#Bajada correcta");
      rightRepetition();
      next();
    } 
  }
}

void exercise2(){
  long antTiempoRep = millis();
  long actTiempoRep = millis();
  loopAcelerometro();
  boolean goingUp = true;
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min2[0] && X_out <= max2[0]) && (Y_out >= min2[1] && Y_out <= max2[1]) && (Z_out >= min2[2] && Z_out <= max2[2])){
      goingUp = false;
      Serial.println("9#Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 2000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min2[3] && X_out <= max2[3]) && (Y_out >= min2[4] && Y_out <= max2[4]) && (Z_out >= min2[5] && Z_out <= max2[5])){
      Serial.println("9#Bajada correcta");
      rightRepetition();
      next();
    } 
  }
}

void exercise3(){
  long antTiempoRep = millis();
  long actTiempoRep = millis();
  loopAcelerometro();
  boolean goingUp = true;
  while(goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 4000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min3[0] && X_out <= max3[0]) && (Y_out >= min3[1] && Y_out <= max3[1]) && (Z_out >= min3[2] && Z_out <= max3[2])){
      goingUp = false;
      Serial.println("9#Subida Correcta");
    }
  }
  antTiempoRep = actTiempoRep = millis();
  while(!goingUp){
    actTiempoRep = millis();
    if(actTiempoRep - antTiempoRep > 2000){
      wrongRepetition();
      return;
    }
    loopAcelerometro();
    if((X_out >= min3[3] && X_out <= max3[3]) && (Y_out >= min3[4] && Y_out <= max3[4]) && (Z_out >= min3[5] && Z_out <= max3[5])){
      Serial.println("9# Bajada correcta");
      rightRepetition();
      next();
    } 
  }
}

void sendMetrics(){
  int bpm = getBPM();
  int weight = getWeight();
  String cadena = "3#";
  cadena += id + ",";
  cadena += bpm + ",";
  cadena += weight + ",";
  cadena += ejercicio + ",";
  cadena += actualSerie + ",";
  cadena += actualRep + ",";
  cadena += series + ",";
  cadena += repeticiones + ",";
  Serial.println(cadena);
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
    isPausedSeries = true;
    errores = 0;
    Serial.println("9#Pausa Obligatoria");
  }
}

void sendRepeticion(boolean estado){
  String cadena = "1#";
  cadena += "{ \"ejercicio\" : " + ejercicio;
  cadena += ", \"actualSerie\": " + actualSerie;
  cadena += ", \"actualRep\" : " + actualRep;
  cadena += ", \"actualSerie\": " + actualSerie;
  cadena += ", \"actualRep\" : " + actualRep;
  
  if(estado) 
    cadena += ", \"estado\" : true }";
  else
    cadena += ", \"estado\" : false }";
  Serial.println(cadena);
}

void rightBuzzer(){
  Serial.println("9#Ejercicio correcto");
}

void wrongBuzzer(){
  Serial.println("9#Ejercicio incorrecto");
}

void clearSerial(){
  while(Serial.available())
    Serial.read();
}
