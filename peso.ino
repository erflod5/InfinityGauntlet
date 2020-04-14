#include "HX711.h"
#define pinP1 (25)
#define DOUT 6
#define CLK 5
HX711 scale(DOUT, CLK);

float calibration_factor = 30000;
float units;
float ounces;

void setupWeight() {
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
}

int getWeight() {
  scale.set_scale(calibration_factor);
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  return units;
}
