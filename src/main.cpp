#include <Arduino.h>
#include "M5Atom.h"
#include <digit_display.h>
#include <cdm7160.h>

float accX = 0, accY = 0, accZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float temp = 0;
bool IMU6886Flag = false;

unsigned char digits_buf[5*75]; // empty + 4 digit ppm à 5x5 pixel in RGB
unsigned char disp_buf[sizeof(digits_buf)+2]; // + initial bytes for "width" & "height"

int CO2val = 400;
int CO2old = 400;
int rot_mod = 0;

void scroll_ppm(int ppm, byte rot_count) {
    Serial.print("Direction: "); Serial.println(rot_count);
    int buf_width;
    int buf_height;

    if (ppm >= 10000) ppm = 9999;
    byte thous = (int)floor(ppm/1000);
      Serial.print(thous);
    ppm = ppm - thous * 1000;
    byte huns = (int) floor(ppm/100);
      Serial.print(huns);
    ppm = ppm - huns * 100;
    byte tens = (int) floor(ppm/10);
      Serial.print(tens);
    ppm = ppm - tens * 10;
    byte ones = (int) floor(ppm);
      Serial.print(ones);
      Serial.println(" ppm");

    memcpy(digits_buf, empty, sizeof(empty));
    buf_width = DIGIT_WIDTH;
    buf_height = DIGIT_HEIGHT;
    if (thous == 0)
      digit_to_buf(99, rot_count); // -> empty
    else
      digit_to_buf(thous, rot_count);
    merge_buffers(digits_buf, &buf_width, &buf_height, rot_count);
    digit_to_buf(huns, rot_count);
    merge_buffers(digits_buf, &buf_width, &buf_height, rot_count);
    digit_to_buf(tens, rot_count);
    merge_buffers(digits_buf, &buf_width, &buf_height, rot_count);
    digit_to_buf(ones, rot_count);
    merge_buffers(digits_buf, &buf_width, &buf_height, rot_count);

    disp_buf[0] = buf_width;
    disp_buf[1] = buf_height;
    memcpy(disp_buf + 2, digits_buf, sizeof(digits_buf));

    int start = 0;
    int ende = 0;
    int incr = 0;
    if ((rot_count == SCROLL_LEFT_MERGE_TO_RIGHT) || (rot_count == SCROLL_TOP_MERGE_TO_BOTTOM)) {
      start = (thous == 0) ? 20 : 25;
      ende = -1;
      incr = -1;
    } else {
      start = (thous == 0) ? 10 : 5;
      ende = 31;
      incr = 1;
    }
    switch (rot_count % 2) {
      case 0: {
        for (int i = start; i != ende; i = i + incr) {
          M5.dis.displaybuff((uint8_t *)disp_buf, i,0);
          delay(100);
        }
        break;
      }
      default: {
        for (int i = start; i != ende; i = i + incr) {
          M5.dis.displaybuff((uint8_t *)disp_buf, 0,i);
          delay(100);
        }
        break;
      }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("M5atom_CO2_status starting...");

    M5.begin(true, false, true);
    delay(50);
    setup_cdm7160();
    matrix_fill(0xFF0000);
    delay(300);

    if (M5.IMU.Init() != 0) {
      IMU6886Flag = false;
      Serial.println("MPU NOK!");
    }
    else
    {
      IMU6886Flag = true;
      Serial.println("MPU ok...");
    }
}

void loop()
{
    if (M5.Btn.wasPressed()) {
      rot_mod = (rot_mod+1)%4;
      matrix_fill(0xFF00FF);
      delay(100);
      matrix_fill(0xFF0000);
      delay(100);
      scroll_ppm(rot_mod, rot_mod);
      delay(100);

    }

    if (IMU6886Flag == true)
    {
        M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
        if ((gyroX*gyroX + gyroY*gyroY + gyroZ*gyroZ) > 60) {
          matrix_fill(0xFFFFFF);
          int timer = 15; // ~1500 ms
          while (((gyroX*gyroX + gyroY*gyroY + gyroZ*gyroZ) > 40) && timer > 0) {
            M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
            delay(100);
            timer--;
          }
          M5.IMU.getAccelData(&accX, &accY, &accZ);
          M5.IMU.getTempData(&temp);
          byte direction = 0;
          if (accY*1000 > 700) direction = SCROLL_LEFT_MERGE_TO_RIGHT;
          if (accX*1000 > 700) direction = SCROLL_BOTTOM_MERGE_TO_TOP;
          if (accY*1000 < -700) direction = SCROLL_RIGHT_MERGE_TO_LEFT;
          if (accX*1000 < -700) direction = SCROLL_TOP_MERGE_TO_BOTTOM;
          direction = (direction + rot_mod) % 4;
          Serial.printf("Gyroscope X,Y,Z: %.2f,%.2f,%.2f o/s \r\n", gyroX, gyroY, gyroZ);
          Serial.printf("Accelerometer X,Y,Z: %.2f,%.2f,%.2f mg\r\n", accX * 1000, accY * 1000, accZ * 1000);
          Serial.printf("Temperature : %.2f C \r\n", temp);
          scroll_ppm(CO2val, direction);
          delay(100);
          matrix_fill(get_CO2color(CO2val));
        }
    }

    CO2val = read_CO2ppm();
    if (CO2val != CO2old) {
      Serial.print(millis()); Serial.print(": "); Serial.print(CO2val); Serial.println(" ppm");
      matrix_fill(get_CO2color(CO2val));
      CO2old = CO2val;
    }

    delay(100);
    M5.update();
}
