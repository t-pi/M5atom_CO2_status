// Code related to simple serial communication with Figaro CDM7160 CO2 sensor
// Only readout of current CO2 ppm value supported

#ifndef _CDM7160_H_
#define _CDM7160_H_

void setup_cdm7160(int rx, int tx);
void setup_cdm7160();
bool is_cdm7160_available();
int read_CO2ppm();

long get_CO2color(int CO2ppm);
long get_CO2color();

#endif // _CDM7160_H_