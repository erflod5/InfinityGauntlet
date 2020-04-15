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
const int min3[] = {-260,40,-20,-260,5,-30};
const int max3[] = {-230,80,0,-230,20,10};
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
  if(isPausedSeries && (actPausedSeries - antPausedSeries) > pausedSeries){
    Serial.println("##################");
    Serial.println(actPausedSeries);
    Serial.println(antPausedSeries);
    Serial.println("##################");
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
        Serial.println("9#Terminar");
        isPaused = true;
        ejercicio = series = repeticiones = -1;
        break;
      case '3': //Starting routine
        Serial.println("9#Empezando rutina...");
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
  String data = Serial.readStringUntil(',');
  ejercicio = data.toInt();
  if(ejercicio == -1){
    Serial.println("#######Serie finalizada######");
    clearSerial();
    return;
  }
  data = Serial.readStringUntil(',');
  series = data.toInt();
  data = Serial.readString();
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
      Serial.println("9#Subida Correcta");
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
      Serial.println("9# Bajada correcta");
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
      Serial.println("9#Subida Correcta");
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
      Serial.println("9# Bajada correcta");
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
      Serial.println("9#Subida Correcta");
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
      Serial.println("9# Bajada correcta");
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
  //errores++;
  if(errores > 2){
    antPausedSeries = millis();
    isPausedSeries = true;
    errores = 0;
    Serial.println("9#Pausa Obligatoria");
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
