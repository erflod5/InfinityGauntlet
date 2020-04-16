#include "HX711.h"
#define pinP1 (25)
#define DOUT 6
#define CLK 5
HX711 scale(DOUT, CLK);

float units;

void setupWeight() {
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
}

float getWeight() {
  scale.set_scale(30000);
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  return units;
}
