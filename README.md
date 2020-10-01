# M5atom_CO2_status
CO2 status monitor ("CO2-Ampel") based on M5stack Atom (ESP32) and Figaro CDM7160 optical CO2 sensor

Program reads out CO2 values via serial interface and sets the M5 Atom's matrix color according to current ppm value:
* case 0 ... 700: green
* case 701 ... 956: shift from green to yellow
* case 957 ... 1211: shift from yellow to red
* case 1212 ... 1500: red
* default: violet

If M5 Atom is moved (inertial sensor), the matrix turns white until movement stops or ~1.5s have passed.
It then scrolls the current ppm value as text on the matrix display.
The orientation for the scrolling text is taken from the inertial sensor.
A push on the main button (the LED matrix) adds 90° to the orientation and scrolls the added quadrant value (0 .. 3)

# Components
* [M5Stack Atom Matrix with ESP32](https://docs.m5stack.com/#/en/core/atom_matrix)
* [Figaro CDM7160 optical CO2 sensor](https://www.figaro.co.jp/en/product/feature/cdm7160.html)
* USB-C power supply

# HowTo
* Platform.io project
* Connect CDM7160 to +5V, GND, RX = Pin 23, TX = Pin 19 on Atom's backside



