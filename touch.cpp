#include "touch.h"

#include <SPI.h>

#include "ubitx.h"//pin assignments

#define Z_THRESHOLD     400
#define Z_THRESHOLD_INT  75
#define MSEC_THRESHOLD  3

static uint32_t msraw=0x80000000;
static  int16_t xraw=0, yraw=0, zraw=0;
static uint8_t rotation = 1;

int16_t touch_besttwoavg( int16_t x , int16_t y , int16_t z ) {
  int16_t da, db, dc;
  int16_t reta = 0;
  if ( x > y ) da = x - y; else da = y - x;
  if ( x > z ) db = x - z; else db = z - x;
  if ( z > y ) dc = z - y; else dc = y - z;

  if ( da <= db && da <= dc ) reta = (x + y) >> 1;
  else if ( db <= da && db <= dc ) reta = (x + z) >> 1;
  else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

  return (reta);
}

void touch_update(){
  int16_t data[6];

  uint32_t now = millis();
  if (now - msraw < MSEC_THRESHOLD) return;
  
  SPI.setClockDivider(SPI_CLOCK_DIV8);//2MHz
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0xB1 /* Z1 */);
  int16_t z1 = SPI.transfer16(0xC1 /* Z2 */) >> 3;
  int z = z1 + 4095;
  int16_t z2 = SPI.transfer16(0x91 /* X */) >> 3;
  z -= z2;
  if (z >= Z_THRESHOLD) {
    SPI.transfer16(0x91 /* X */);  // dummy X measure, 1st is always noisy
    data[0] = SPI.transfer16(0xD1 /* Y */) >> 3;
    data[1] = SPI.transfer16(0x91 /* X */) >> 3; // make 3 x-y measurements
    data[2] = SPI.transfer16(0xD1 /* Y */) >> 3;
    data[3] = SPI.transfer16(0x91 /* X */) >> 3;
  }
  else data[0] = data[1] = data[2] = data[3] = 0; // Compiler warns these values may be used unset on early exit.
  data[4] = SPI.transfer16(0xD0 /* Y */) >> 3;  // Last Y touch power down
  data[5] = SPI.transfer16(0) >> 3;
  digitalWrite(CS_PIN, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Return to full speed for TFT

  if (z < 0) z = 0;
  if (z < Z_THRESHOLD) { // if ( !touched ) {
    // Serial.println();
    zraw = 0;
    return;
  }
  zraw = z;
  
  int16_t x = touch_besttwoavg( data[0], data[2], data[4] );
  int16_t y = touch_besttwoavg( data[1], data[3], data[5] );
  
  //Serial.printf("    %d,%d", x, y);
  //Serial.println();
  if (z >= Z_THRESHOLD) {
    msraw = now;  // good read completed, set wait
    switch (rotation) {
      case 0:
      xraw = 4095 - y;
      yraw = x;
      break;
      case 1:
      xraw = x;
      yraw = y;
      break;
      case 2:
      xraw = y;
      yraw = 4095 - x;
      break;
      default: // 3
      xraw = 4095 - x;
      yraw = 4095 - y;
    }
  }
}

void initTouch(){
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
}

bool readTouch(Point *const touch_point_out){
  touch_update();
  if (zraw >= Z_THRESHOLD) {
    touch_point_out->x = xraw;
    touch_point_out->y = yraw;
    //Serial.print(ts_point.x); Serial.print(",");Serial.println(ts_point.y);
    return true;
  }
  return false;
}

void scaleTouch(Point *const touch_point_in_out){
  touch_point_in_out->x = ((long)(touch_point_in_out->x - globalSettings.touchOffsetX) * 10L)/ (long)globalSettings.touchSlopeX;
  touch_point_in_out->y = ((long)(touch_point_in_out->y - globalSettings.touchOffsetY) * 10L)/ (long)globalSettings.touchSlopeY;

  //Serial.print(p->x); Serial.print(",");Serial.println(p->y);
}