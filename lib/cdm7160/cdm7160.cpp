// Code related to simple serial communication with Figaro CDM7160 CO2 sensor
// Only readout of current CO2 ppm value supported

#include <Arduino.h>
#include "cdm7160.h"

HardwareSerial Ser1_CO2(1);
int ser_rx = 23;
int ser_tx = 19;
bool cdm7160_available = false;

byte cmdCO2[7] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25};
int CO2 = 400;


void setup_cdm7160(int rx, int tx) {
    if (rx != ser_rx) ser_rx = rx;
    if (tx != ser_tx) ser_tx = tx;
    Ser1_CO2.begin(9600, SERIAL_8N1, rx, tx);
    Ser1_CO2.setTimeout(3000);
    cdm7160_available = true;
    Serial.print("CDM7160 port initalized on RX: Pin "+String(rx));
    Serial.println(" / TX: Pin "+String(tx));
}

void setup_cdm7160() {
    setup_cdm7160(ser_rx, ser_tx);
}

bool is_cdm7160_available() {
    return cdm7160_available;
}

int read_CO2ppm() {
    // flush incoming buffer
    while (Ser1_CO2.available())
      Ser1_CO2.read();

    delay(100);
    Ser1_CO2.write(cmdCO2, 7);

    delay(200);
    byte buf[8];
    Ser1_CO2.readBytes(buf, 7);
    CO2 = ((int)buf[3])<<8 | buf[4];
    return CO2;
}

long get_CO2color(int CO2ppm) {
    switch (CO2ppm) {
      case 0 ... 700:
        // 0xGGRRBB
        return 0xFF0000;
      case 701 ... 956:
        return (0xFF0000+((CO2-701)<<8));
      case 957 ... 1211:
        return ((0xFFFF00-((CO2-957)<<16)));
      case 1212 ... 1500:
        return (0x00FF00);
      default:
        return (0x00FFFF);
    }
}

long get_CO2color() {
    return get_CO2color(CO2);
}



