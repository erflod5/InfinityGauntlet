//BLANCO -> VCC NEGRO -> GND AMARILLO -> SCL NARANJA -> SDA
//CAFE -> VCC   ROJO -> GND   NARANJA -> SCL AMARILLO -> SDA
#include <Wire.h>  // Wire library - used for I2C communication
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
int x;
int y;

void setupAcelerometro() {
  x=y=millis();
  Wire.begin(); // Initiate the Wire library
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
}

void loopAcelerometro() {
  String cadena = "";
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out;
   cadena = X_out;
  cadena += ",";
  cadena += Y_out;
  cadena += ",";
  cadena += Z_out;
  cadena += ",";
  x=millis();
//  if(x-y>500){
//    Serial.println(cadena);  
//    y=millis();
//  }
  
  
}

  
