// Code related to display numbers via single digits on small 5x5 NeoPixel-Matrix

#include <Arduino.h>
#include "digit_display.h"
#include "M5Atom.h"

unsigned char rot_buf[75];
unsigned char digit_buf[75];

// rotate number by 90 degrees counterclockwise
unsigned char *rotate_buffer90(unsigned char *digit) {
  for (long i = 0; i < 25; i++) {
    rot_buf[i*3+0] = digit[(i*3+(i+1)*12+0)%75 - (i/5)*3];
    rot_buf[i*3+1] = digit[(i*3+(i+1)*12+1)%75 - (i/5)*3];
    rot_buf[i*3+2] = digit[(i*3+(i+1)*12+2)%75 - (i/5)*3];
  }
  return rot_buf;
}

// copy single digit in digit matrix buffer and rotate
void digit_to_buf(byte number, byte rot_count) {
  const unsigned char *digit_ptr;
  switch (number) {
    case 0:
      digit_ptr = (const unsigned char *)zero;
      break;
    case 1:
      digit_ptr = (const unsigned char *)one;
      break;
    case 2:
      digit_ptr = (const unsigned char *)two;
      break;
    case 3:
      digit_ptr = (const unsigned char *)three;
      break;
    case 4:
      digit_ptr = (const unsigned char *)four;
      break;
    case 5:
      digit_ptr = (const unsigned char *)five;
      break;
    case 6:
      digit_ptr = (const unsigned char *)six;
      break;
    case 7:
      digit_ptr = (const unsigned char *)seven;
      break;
    case 8:
      digit_ptr = (const unsigned char *)eight;
      break;
    case 9:
      digit_ptr = (const unsigned char *)nine;
      break;
    default:
      digit_ptr = (const unsigned char *)empty;
      break;
  }
  memcpy(digit_buf, digit_ptr, sizeof(empty));
  for (long i = 0; i < rot_count; i++)
    memcpy(digit_buf, rotate_buffer90(digit_buf), sizeof(empty));
}

// SCROLL_LEFT_MERGE_TO_RIGHT 0, SCROLL_TOP_MERGE_TO_BOTTOM 1, SCROLL_RIGHT_MERGE_TO_LEFT 2,  SCROLL_BOTTOM_MERGE_TO_TOP 3
void merge_buffers(unsigned char *target, int *target_width, int *target_height, byte merge_direction) {
    unsigned char *first, *second;
    int first_width, first_height, second_width, second_height;
    bool line_merge;
    merge_direction = merge_direction % 4;
    unsigned char target_buffer[(*target_width)*(*target_height)*3 + DIGIT_HEIGHT*DIGIT_WIDTH*3];

    if ((*target_width != DIGIT_WIDTH) && (*target_height != DIGIT_HEIGHT)) {
        Serial.println("! Error while buffer merge: Either width must be " + String(DIGIT_WIDTH) + " or height " + String(DIGIT_HEIGHT));
    }

    switch (merge_direction) {
        case SCROLL_LEFT_MERGE_TO_RIGHT:
            first = target;
            first_width = *target_width;
            first_height = *target_height;
            second = digit_buf;
            second_width = DIGIT_WIDTH;
            second_height = DIGIT_HEIGHT;
            line_merge = true;
            break;
        case SCROLL_TOP_MERGE_TO_BOTTOM:
            first = target;
            first_width = *target_width;
            first_height = *target_height;
            second = digit_buf;
            second_width = DIGIT_WIDTH;
            second_height = DIGIT_HEIGHT;
            line_merge = false;
            break;
        case SCROLL_RIGHT_MERGE_TO_LEFT:
            first = digit_buf;
            first_width = DIGIT_WIDTH;
            first_height = DIGIT_HEIGHT;
            second = target;
            second_width = *target_width;
            second_height = *target_height;
            line_merge = true;
            break;
        case SCROLL_BOTTOM_MERGE_TO_TOP:
            first = digit_buf;
            first_width = DIGIT_WIDTH;
            first_height = DIGIT_HEIGHT;
            second = target;
            second_width = *target_width;
            second_height = *target_height;
            line_merge = false;
            break;
    }
    if (line_merge) {
        for (int row = 0; row < first_height; row++) {
            memcpy(target_buffer + row*3*(first_width + second_width), first + row*3*first_width, 3*first_width);
            memcpy(target_buffer + row*3*(first_width + second_width) + 3*first_width, second + row*3*second_width, 3*second_width);
        }
        *target_width = first_width + second_width;
    } else {
        memcpy(target_buffer, first, 3*first_width*first_height);
        memcpy(target_buffer + 3*first_width*first_height, second, 3*second_width*second_height);
        *target_height = first_height + second_height;
    }
    memcpy(target, target_buffer, sizeof(target_buffer));
}


// 0xGGRRBB
void matrix_fill(long color_GGRRBB) {
    for (int i = 0; i < 25; i++)
    {
      M5.dis.drawpix(i,color_GGRRBB);
    }
}
