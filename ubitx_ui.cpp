#include <Arduino.h>
#include "morse.h"
#include "settings.h"
#include "setup.h"
#include "ubitx.h"
#include "nano_gui.h"

static const unsigned int COLOR_TEXT = DISPLAY_WHITE;
static const unsigned int COLOR_BACKGROUND = DISPLAY_NAVY;

static const unsigned int COLOR_ACTIVE_VFO_TEXT = DISPLAY_WHITE;
static const unsigned int COLOR_ACTIVE_VFO_BACKGROUND = DISPLAY_BLACK;

static const unsigned int COLOR_INACTIVE_VFO_TEXT = DISPLAY_GREEN;
static const unsigned int COLOR_INACTIVE_VFO_BACKGROUND = DISPLAY_BLACK;

static const unsigned int COLOR_INACTIVE_TEXT = DISPLAY_GREEN;
static const unsigned int COLOR_INACTIVE_BACKGROUND = DISPLAY_BLACK;
static const unsigned int COLOR_INACTIVE_BORDER = DISPLAY_DARKGREY;

static const unsigned int COLOR_ACTIVE_TEXT = DISPLAY_BLACK;
static const unsigned int COLOR_ACTIVE_BACKGROUND = DISPLAY_ORANGE;
static const unsigned int COLOR_ACTIVE_BORDER = DISPLAY_WHITE;

static const unsigned int LAYOUT_VFO_LABEL_X = 0;
static const unsigned int LAYOUT_VFO_LABEL_Y = 10;
static const unsigned int LAYOUT_VFO_LABEL_WIDTH = 159;
static const unsigned int LAYOUT_VFO_LABEL_HEIGHT = 36;
static const unsigned int LAYOUT_VFO_LABEL_PITCH_X = 160;

static const unsigned int LAYOUT_MODE_TEXT_X = 0;
static const unsigned int LAYOUT_MODE_TEXT_Y = LAYOUT_VFO_LABEL_Y + LAYOUT_VFO_LABEL_HEIGHT + 1;
static const unsigned int LAYOUT_MODE_TEXT_WIDTH = 320;
static const unsigned int LAYOUT_MODE_TEXT_HEIGHT = 36;

static const unsigned int LAYOUT_BUTTON_X = 2;
static const unsigned int LAYOUT_BUTTON_Y = LAYOUT_MODE_TEXT_Y + LAYOUT_MODE_TEXT_HEIGHT + 1;
static const unsigned int LAYOUT_BUTTON_WIDTH = 60;
static const unsigned int LAYOUT_BUTTON_HEIGHT = 36;
static const unsigned int LAYOUT_BUTTON_PITCH_X = 64;
static const unsigned int LAYOUT_BUTTON_PITCH_Y = 40;

static const unsigned int LAYOUT_CW_TEXT_X = 0;
static const unsigned int LAYOUT_CW_TEXT_Y = LAYOUT_BUTTON_Y + 3*LAYOUT_BUTTON_PITCH_Y + 1;
static const unsigned int LAYOUT_CW_TEXT_WIDTH = 320;
static const unsigned int LAYOUT_CW_TEXT_HEIGHT = 36;

static const unsigned int LAYOUT_TX_X = 280;
static const unsigned int LAYOUT_TX_Y = LAYOUT_MODE_TEXT_Y;
static const unsigned int LAYOUT_TX_WIDTH = 40;
static const unsigned int LAYOUT_TX_HEIGHT = 36;

enum btn_set_e {
  BUTTON_VFOA,
  BUTTON_VFOB,
  BUTTON_RIT,
  BUTTON_USB,
  BUTTON_LSB,
  BUTTON_CW,
  BUTTON_SPL,
  BUTTON_80,
  BUTTON_40,
  BUTTON_30,
  BUTTON_20,
  BUTTON_17,
  BUTTON_15,
  BUTTON_10,
  BUTTON_BLANK_1,
  BUTTON_MNU,
  BUTTON_FRQ,
  BUTTON_TOTAL
};

struct Button {
  int x, y, w, h;
  unsigned int id;
  char text[5];
  char morse;
};

constexpr Button btn_set[BUTTON_TOTAL] PROGMEM = {
  {LAYOUT_VFO_LABEL_X + 0*LAYOUT_VFO_LABEL_PITCH_X, LAYOUT_VFO_LABEL_Y, LAYOUT_VFO_LABEL_WIDTH, LAYOUT_VFO_LABEL_HEIGHT, BUTTON_VFOA, "VFOA", 'A'},
  {LAYOUT_VFO_LABEL_X + 1*LAYOUT_VFO_LABEL_PITCH_X, LAYOUT_VFO_LABEL_Y, LAYOUT_VFO_LABEL_WIDTH, LAYOUT_VFO_LABEL_HEIGHT, BUTTON_VFOB, "VFOB", 'B'},
  
  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_RIT, "RIT", 'R'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_USB, "USB", 'U'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_LSB, "LSB", 'L'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_CW ,  "CW", 'C'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_SPL, "SPL", 'S'},

  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_80, "80", '8'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_40, "40", '4'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_30, "30", '3'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_20, "20", '2'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_17, "17", '7'},

  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_15 ,  "15", '5'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_10 ,  "10", '1'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_BLANK_1, "", '\0'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_MNU, "MNU", 'M'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, BUTTON_FRQ, "FRQ", 'F'},
};

static const unsigned int KEYS_OFFSET = 256;//Unique from buttons
enum keypad_e {
  KEYS_1 = KEYS_OFFSET,
  KEYS_2,
  KEYS_3,
  KEYS_BLANK_1,
  KEYS_OK,
  KEYS_4,
  KEYS_5,
  KEYS_6,
  KEYS_0,
  KEYS_BACKSPACE,
  KEYS_7,
  KEYS_8,
  KEYS_9,
  KEYS_BLANK_2,
  KEYS_CANCEL,
  KEYS_TOTAL = KEYS_CANCEL - KEYS_OFFSET + 1
};
constexpr Button keypad[KEYS_TOTAL] PROGMEM = {
  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_1,        "1", '1'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_2,        "2", '2'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_3,        "3", '3'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_BLANK_1,   "", '\0'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 0*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_OK,      "OK", 'K'},

  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_4,          "4", '4'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_5,          "5", '5'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_6,          "6", '6'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_0,          "0", '0'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 1*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_BACKSPACE, "<-", 'B'},

  {LAYOUT_BUTTON_X + 0*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_7,        "7", '7'},
  {LAYOUT_BUTTON_X + 1*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_8,        "8", '8'},
  {LAYOUT_BUTTON_X + 2*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_9,        "9", '9'},
  {LAYOUT_BUTTON_X + 3*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_BLANK_2,   "", '\0'},
  {LAYOUT_BUTTON_X + 4*LAYOUT_BUTTON_PITCH_X, LAYOUT_BUTTON_Y + 2*LAYOUT_BUTTON_PITCH_Y, LAYOUT_BUTTON_WIDTH, LAYOUT_BUTTON_HEIGHT, KEYS_CANCEL, "Can", 'C'},
};

boolean getButton(btn_set_e index, Button* button){
  if(BUTTON_TOTAL == index){
    return false;
  }
  memcpy_P(button, &(btn_set[index]), sizeof(Button));
  return true;
}

inline void drawCommandbar(char* text){
  displayText(text, LAYOUT_MODE_TEXT_X, LAYOUT_MODE_TEXT_Y, LAYOUT_MODE_TEXT_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
}

/** A generic control to read variable values
*/
int getValueByKnob(int minimum, int maximum, int step_size,  int initial, const __FlashStringHelper* prefix, const __FlashStringHelper* postfix)
{
  int knob = 0;
  int knob_value;

  while (btnDown())
    active_delay(100);

  active_delay(200);
  knob_value = initial;

  strncpy_P(b,(const char*)prefix,sizeof(b));
  itoa(knob_value, c, 10);
  strncat(b, c, sizeof(b) - strlen(b));
  strncat_P(b, (const char*)postfix, sizeof(b) - strlen(b));
  drawCommandbar(b);

  while(!btnDown() && digitalRead(PTT) == HIGH){
    knob = enc_read();
    if (knob != 0){
      if (knob_value > minimum && knob < 0)
        knob_value -= step_size;
      if (knob_value < maximum && knob > 0)
        knob_value += step_size;

      strncpy_P(b,(const char*)prefix,sizeof(b));
      itoa(knob_value, c, 10);
      strncat(b, c, sizeof(b) - strlen(b));
      strncat_P(b,(const char*)postfix, sizeof(b) - strlen(b));
      drawCommandbar(b);
    }
  checkCAT();
  }
  b[0] = 0;
  drawCommandbar(b);
  return knob_value;
}

void displayVFO(Vfo_e vfo){
  int x, y;
  int displayColor, displayBackground, displayBorder;
  Button button;

  if (globalSettings.splitOn){
    if (vfo == globalSettings.activeVfo){
      c[0] = 'R';
    }
    else{
      c[0] = 'T';
    }
  }
  else{
    if(VFO_A == vfo){
      c[0] = 'A';
    }
    else if(VFO_B == vfo){
      c[0] = 'B';
    }
    else{
      c[0] = '?';
    }
  }
  c[1] = ':';


  if (VFO_A == vfo){
    getButton(BUTTON_VFOA, &button);
    formatFreq(globalSettings.vfoA.frequency, c+2, sizeof(c)-2);

    if (VFO_A == globalSettings.activeVfo){
      displayColor = COLOR_ACTIVE_VFO_TEXT;
      displayBackground = COLOR_ACTIVE_VFO_BACKGROUND;
      displayBorder = COLOR_ACTIVE_BORDER;
    }else{
      displayColor = COLOR_INACTIVE_VFO_TEXT;
      displayBackground = COLOR_INACTIVE_VFO_BACKGROUND;
      displayBorder = COLOR_INACTIVE_BORDER;
    }
  }

  if (VFO_B == vfo){
    getButton(BUTTON_VFOB, &button);
    formatFreq(globalSettings.vfoB.frequency, c+2, sizeof(c)-2);

    if (VFO_B == globalSettings.activeVfo){
      displayColor = COLOR_ACTIVE_VFO_TEXT;
      displayBackground = COLOR_ACTIVE_VFO_BACKGROUND;
      displayBorder = COLOR_ACTIVE_BORDER;
    } else {
      displayColor = COLOR_INACTIVE_VFO_TEXT;
      displayBackground = COLOR_INACTIVE_VFO_BACKGROUND;
      displayBorder = COLOR_INACTIVE_BORDER;
    }
  }

  displayText(c, button.x, button.y, button.w, button.h, displayColor, displayBackground, displayBorder);
}

void btnDrawActive(Button* button){
  displayText(button->text, button->x, button->y, button->w, button->h, COLOR_ACTIVE_TEXT, COLOR_ACTIVE_BACKGROUND, COLOR_INACTIVE_BORDER);
}
void btnDrawInactive(Button* button){
  displayText(button->text, button->x, button->y, button->w, button->h, COLOR_INACTIVE_TEXT, COLOR_INACTIVE_BACKGROUND, COLOR_INACTIVE_BORDER);
}

void btnDraw(struct Button *button){
  switch(button->id){
    case BUTTON_VFOA:
    {
      displayVFO(VFO_A);
      break;
    }
    case BUTTON_VFOB:
    {
      displayVFO(VFO_B);
      break;
    }
    case BUTTON_RIT:
    {
      if(globalSettings.ritOn){
        btnDrawActive(button);
      }
      else{
        btnDrawInactive(button);
      }
      break;
    }
    case BUTTON_USB:
    {
      if(VFO_MODE_USB == GetActiveVfoMode()){
        btnDrawActive(button);
      }
      else{
        btnDrawInactive(button);
      }
      break;
    }
    case BUTTON_LSB:
    {
      if(VFO_MODE_LSB == GetActiveVfoMode()){
        btnDrawActive(button);
      }
      else{
        btnDrawInactive(button);
      }
      break;
    }
    case BUTTON_SPL:
    {
      if(globalSettings.splitOn){
        btnDrawActive(button);
      }
      else{
        btnDrawInactive(button);
      }
      break;
    }
    case BUTTON_CW:
    {
      if(TuningMode_e::TUNE_CW == globalSettings.tuningMode){
        btnDrawActive(button);
      }
      else{
        btnDrawInactive(button);
      }
      break;
    }
    default:
    {
      btnDrawInactive(button);
      break;
    }
  }//End switch
}


void displayRIT(){
  c[0] = 0;
  displayFillrect(LAYOUT_MODE_TEXT_X,LAYOUT_MODE_TEXT_Y,LAYOUT_MODE_TEXT_WIDTH,LAYOUT_MODE_TEXT_HEIGHT, COLOR_BACKGROUND);
  if(globalSettings.ritOn){
    strncpy_P(c,(const char*)F("TX:"),sizeof(c));
    formatFreq(globalSettings.ritFrequency, c+3, sizeof(c)-strlen(c));
    if (VFO_A == globalSettings.activeVfo)
      displayText(c, LAYOUT_VFO_LABEL_X + 0*LAYOUT_VFO_LABEL_PITCH_X, LAYOUT_MODE_TEXT_Y, LAYOUT_VFO_LABEL_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
    else
      displayText(c, LAYOUT_VFO_LABEL_X + 1*LAYOUT_VFO_LABEL_PITCH_X, LAYOUT_MODE_TEXT_Y, LAYOUT_VFO_LABEL_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
  }
}

void fastTune(){
  //if the btn is down, wait until it is up
  while(btnDown())
    active_delay(50);
  active_delay(50);
  
  strncpy_P(c,(const char*)F("Fast tune"),sizeof(c));
  displayText(c, LAYOUT_MODE_TEXT_X, LAYOUT_MODE_TEXT_Y, LAYOUT_MODE_TEXT_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
  while(true){
    checkCAT();

    //exit after debouncing the btnDown
    if (btnDown()){
      displayFillrect(LAYOUT_MODE_TEXT_X, LAYOUT_MODE_TEXT_Y, LAYOUT_MODE_TEXT_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_BACKGROUND);

      //wait until the button is realsed and then return
      while(btnDown())
        active_delay(50);
      active_delay(50);
      return;
    }
    
    int encoder = enc_read();
    if (encoder != 0){
      uint32_t freq = GetActiveVfoFreq();
      if (encoder > 0 && freq < 30000000l){
        freq += 50000l;
      }
      else if (encoder < 0 && freq > 600000l){
        freq -= 50000l;
      }
      setFrequency(freq);
      displayVFO(globalSettings.activeVfo);
    }
  }// end of the event loop
}

void enterFreq(){
  //force the display to refresh everything
  //display all the buttons
  
  for (int i = 0; i < KEYS_TOTAL; i++){
    Button button;
    memcpy_P(&button, &(keypad[i]), sizeof(Button));
    btnDraw(&button);
  }

  while(btnDown())
    active_delay(50);
  active_delay(50);

  int cursor_pos = 0;
  memset(c, 0, sizeof(c));

  while(1){

    checkCAT();
    if(btnDown()){
      while(btnDown())
        active_delay(50);
      active_delay(50);
      break;
    }
    if(!readTouch())
      continue;
      
    scaleTouch(&ts_point);

    for (int i = 0; i < KEYS_TOTAL; i++){
      Button button;
      memcpy_P(&button, &(keypad[i]), sizeof(Button));

      int x2 = button.x + button.w;
      int y2 = button.y + button.h;
  
      if(button.x < ts_point.x && ts_point.x < x2 && 
         button.y < ts_point.y && ts_point.y < y2){
        //Serial.print(F("Entered key: "));
        //Serial.println(button.text);
        switch(button.id){
          case KEYS_OK:
          {
            long freq = atol(c);
            if((LOWEST_FREQ/1000 <= freq) && (freq <= HIGHEST_FREQ/1000)){
              freq *= 1000L;
              setFrequency(freq);
              if (VFO_A == globalSettings.activeVfo){
                globalSettings.vfoA.frequency = freq;
              }
              else{
                globalSettings.vfoB.frequency = freq;
              }
              saveVFOs();
            }
            break;
          }
          
          case KEYS_BACKSPACE:
          {
            c[cursor_pos] = 0;
            if (cursor_pos > 0){
              cursor_pos--;
            }
            c[cursor_pos] = 0;
            break;
          }
          case KEYS_CANCEL:
          {
            break;
          }
          case KEYS_0:
          case KEYS_1:
          case KEYS_2:
          case KEYS_3:
          case KEYS_4:
          case KEYS_5:
          case KEYS_6:
          case KEYS_7:
          case KEYS_8:
          case KEYS_9:
          {
            c[cursor_pos++] = button.text[0];
            c[cursor_pos] = 0;
            break;
          }
          default:
          {
            //Serial.print(F("Unknown key id: "));
            //Serial.println(button.id);
            break;
          }
        }//switch
      }//if button hit test
    }// end of the button scanning loop
    strncpy(b, c, sizeof(b));
    strncat_P(b,(const char*)F(" KHz"),sizeof(b) - strlen(b));
    displayText(b, LAYOUT_MODE_TEXT_X, LAYOUT_MODE_TEXT_Y, LAYOUT_MODE_TEXT_WIDTH, LAYOUT_MODE_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
    active_delay(300);
    while(readTouch())
      checkCAT();
  } // end of event loop : while(1)

  guiUpdate();
  enc_read();//clear out any tuner turning that happened during entry
}

void drawCWStatus(){
  strncpy_P(b,(const char*)F(" cw: "),sizeof(b));
  int wpm = 1200/globalSettings.cwDitDurationMs;
  itoa(wpm,c, 10);
  strncat(b, c, sizeof(b) - strlen(b));
  strncat_P(b,(const char*)F("wpm, "), sizeof(b) - strlen(b));
  itoa(globalSettings.cwSideToneFreq, c, 10);
  strncat(b, c, sizeof(b) - strlen(b));
  strncat_P(b,(const char*)F("hz"), sizeof(b) - strlen(b));
  displayText(b, LAYOUT_CW_TEXT_X, LAYOUT_CW_TEXT_Y, LAYOUT_CW_TEXT_WIDTH, LAYOUT_CW_TEXT_HEIGHT, COLOR_TEXT, COLOR_BACKGROUND, COLOR_BACKGROUND);
}


void drawTx(){
  if (globalSettings.txActive){
    strncpy_P(b,(const char*)F("TX"), sizeof(b));
    displayText(b, LAYOUT_TX_X, LAYOUT_TX_Y, LAYOUT_TX_WIDTH, LAYOUT_TX_HEIGHT, COLOR_ACTIVE_TEXT, COLOR_ACTIVE_BACKGROUND, COLOR_BACKGROUND);
  }
  else{
    displayFillrect(LAYOUT_TX_X, LAYOUT_TX_Y, LAYOUT_TX_WIDTH, LAYOUT_TX_HEIGHT, COLOR_BACKGROUND);
  }
}
void drawStatusbar(){
  drawCWStatus();
}

void guiUpdate(){

/*
  if (doingCAT)
    return;
*/
  // use the current frequency as the VFO frequency for the active VFO
  displayClear(COLOR_BACKGROUND);

  displayVFO(VFO_A);
  displayVFO(VFO_B);

  checkCAT();
  displayRIT();
  checkCAT();

  //force the display to refresh everything
  //display all the buttons
  for (int i = 0; i < BUTTON_TOTAL; i++){
    Button button;
    memcpy_P(&button, &(btn_set[i]), sizeof(Button));
    btnDraw(&button);
    checkCAT();
  }
  drawStatusbar();
  checkCAT();
}



// this builds up the top line of the display with frequency and mode
void updateDisplay() {
   displayVFO(globalSettings.activeVfo);
}


/**
 * The A7 And A6 are purely analog lines on the Arduino Nano
 * These need to be pulled up externally using two 10 K resistors
 * 
 * There are excellent pages on the Internet about how these encoders work
 * and how they should be used. We have elected to use the simplest way
 * to use these encoders without the complexity of interrupts etc to 
 * keep it understandable.
 * 
 * The enc_state returns a two-bit number such that each bit reflects the current
 * value of each of the two phases of the encoder
 * 
 * The enc_read returns the number of net pulses counted over 50 msecs. 
 * If the puluses are -ve, they were anti-clockwise, if they are +ve, the
 * were in the clockwise directions. Higher the pulses, greater the speed
 * at which the enccoder was spun
 */

/*
int enc_prev_state = 3;

byte enc_state (void) {
    //Serial.print(digitalRead(ENC_A)); Serial.print(":");Serial.println(digitalRead(ENC_B));
    return (digitalRead(ENC_A) == 1 ? 1 : 0) + (digitalRead(ENC_B) == 1 ? 2: 0);
}


int enc_read(void) {
  int result = 0; 
  byte newState;
  int enc_speed = 0;
  
  long stop_by = millis() + 200;
  
  while (millis() < stop_by) { // check if the previous state was stable
    newState = enc_state(); // Get current state  
    
//    if (newState != enc_prev_state)
//      active_delay(20);
    
    if (enc_state() != newState || newState == enc_prev_state)
      continue; 
    //these transitions point to the encoder being rotated anti-clockwise
    if ((enc_prev_state == 0 && newState == 2) || 
      (enc_prev_state == 2 && newState == 3) || 
      (enc_prev_state == 3 && newState == 1) || 
      (enc_prev_state == 1 && newState == 0)){
        result--;
      }
    //these transitions point o the enccoder being rotated clockwise
    if ((enc_prev_state == 0 && newState == 1) || 
      (enc_prev_state == 1 && newState == 3) || 
      (enc_prev_state == 3 && newState == 2) || 
      (enc_prev_state == 2 && newState == 0)){
        result++;
      }
    enc_prev_state = newState; // Record state for next pulse interpretation
    enc_speed++;
    active_delay(1);
  }
  //if (result)
  //  Serial.println(result);
  return(result);
}
*/

/*
 * SmittyHalibut's encoder handling, using interrupts. Should be quicker, smoother handling.
 */
int8_t enc_count;
uint8_t prev_enc;

uint8_t enc_state (void) {
    return (digitalRead(ENC_A)?1:0 + digitalRead(ENC_B)?2:0);
}

/*
 * Setup the encoder interrupts and global variables.
 */
void pci_setup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}

void enc_setup(void) {
  enc_count = 0;
  // This is already done in setup() ?
  //pinMode(ENC_A, INPUT);
  //pinMode(ENC_B, INPUT);
  prev_enc = enc_state();

  // Setup Pin Change Interrupts for the encoder inputs
  pci_setup(ENC_A);
  pci_setup(ENC_B);
}

/*
 * The Interrupt Service Routine for Pin Change Interrupts on A0-A5.
 */
ISR (PCINT1_vect) {
  uint8_t cur_enc = enc_state();
  if (prev_enc == cur_enc) {
    //Serial.println("unnecessary ISR");
    return;
  }
  //Serial.print(prev_enc);
  //Serial.println(cur_enc);
  
  //these transitions point to the enccoder being rotated anti-clockwise
  if ((prev_enc == 0 && cur_enc == 2) || 
      (prev_enc == 2 && cur_enc == 3) || 
      (prev_enc == 3 && cur_enc == 1) || 
      (prev_enc == 1 && cur_enc == 0))
  {
    enc_count-=1;
  }
  //these transitions point to the enccoder being rotated clockwise
  else if ((prev_enc == 0 && cur_enc == 1) || 
      (prev_enc == 1 && cur_enc == 3) || 
      (prev_enc == 3 && cur_enc == 2) || 
      (prev_enc == 2 && cur_enc == 0))
  {
    enc_count+=1;
  }
  else {
    // A change to two states, we can't tell whether it was forward or backward, so we skip it.
    //Serial.println("skip");
  }
  prev_enc = cur_enc; // Record state for next pulse interpretation
}

int enc_read(void) {
  int8_t ret = enc_count;
  enc_count = 0;
  return int(ret);
}




void ritToggle(struct Button *button){
  if(!globalSettings.ritOn){
    ritEnable(GetActiveVfoFreq());
  }
  else{
    ritDisable();
  }
  btnDraw(button);
  displayRIT();
}

void splitToggle(Button *button){

  globalSettings.splitOn = !globalSettings.splitOn;

  btnDraw(button);

  //disable rit as well
  ritDisable();
  
  Button button2;
  getButton(BUTTON_RIT, &button2);
  btnDraw(&button2);
  
  displayRIT();
  displayVFO(VFO_A);
  displayVFO(VFO_B);
}

void vfoReset(){
  Button button;
  if (VFO_A == globalSettings.activeVfo)
    globalSettings.vfoB.frequency = globalSettings.vfoA.frequency;
  else
    globalSettings.vfoA.frequency = globalSettings.vfoB.frequency;

  if(globalSettings.splitOn){
    getButton(BUTTON_SPL, &button);
    splitToggle(&button);
  }

  if(globalSettings.ritOn){
    getButton(BUTTON_RIT, &button);
    ritToggle(&button);
  }
  
  displayVFO(VFO_A);
  displayVFO(VFO_B);

  saveVFOs();
}

void cwToggle(struct Button *b){
  if (TuningMode_e::TUNE_SSB == globalSettings.tuningMode){
    globalSettings.tuningMode = TuningMode_e::TUNE_CW;
  }
  else{
    globalSettings.tuningMode = TuningMode_e::TUNE_SSB;
  }

  setFrequency(GetActiveVfoFreq());
  btnDraw(b);
}

void sidebandToggle(Button* button){
  if(BUTTON_LSB == button->id)
    SetActiveVfoMode(VfoMode_e::VFO_MODE_LSB);
  else
    SetActiveVfoMode(VfoMode_e::VFO_MODE_USB);

  struct Button button2;
  getButton(BUTTON_USB, &button2);
  btnDraw(&button2);
  getButton(BUTTON_LSB, &button2);
  btnDraw(&button2);

  saveVFOs();
}


void redrawVFOs(){
    struct Button button;
    ritDisable();
    getButton(BUTTON_RIT, &button);
    btnDraw(&button);
    displayRIT();
    displayVFO(VFO_A);
    displayVFO(VFO_B);

    //draw the lsb/usb buttons, the sidebands might have changed
    getButton(BUTTON_LSB, &button);
    btnDraw(&button);
    getButton(BUTTON_USB, &button);
    btnDraw(&button);
}


void switchBand(uint32_t bandfreq){

  //Serial.println(frequency);
  //Serial.println(bandfreq);
  uint32_t offset;
  uint32_t freq = GetActiveVfoFreq();
  if (3500000L <= freq && freq <= 4000000L)
    offset = freq - 3500000l;
  else if (24800000L <= freq && freq <= 25000000L)
    offset = freq - 24800000L;
  else 
    offset = freq % 1000000L;

  //Serial.println(offset);

  setFrequency(bandfreq + offset);

  if(bandfreq >= THRESHOLD_USB_LSB){
    SetActiveVfoMode(VfoMode_e::VFO_MODE_USB);
  }
  else{
    SetActiveVfoMode(VfoMode_e::VFO_MODE_LSB);
  }

  updateDisplay();
  saveVFOs();
}

void doCommand(Button* button){
  //Serial.print(F("Doing command: "));
  //Serial.print(button->text);
  //Serial.print(F(", Hitbox: "));
  //Serial.print(button->x);Serial.print(F(","));
  //Serial.print(button->y);Serial.print(F(";"));
  //Serial.print(button->x + button->w);Serial.print(F(","));
  //Serial.println(button->y + button->h);
  switch(button->id){
    case BUTTON_RIT:
    {
      ritToggle(button);
      break;
    }
    case BUTTON_LSB:
    case BUTTON_USB:
    {
      sidebandToggle(button);
      break;
    }
    case BUTTON_CW:
    {
      cwToggle(button);
      break;
    }
    case BUTTON_SPL:
    {
      splitToggle(button);
      break;
    }
    case BUTTON_VFOA:
    {
      if(VFO_A == globalSettings.activeVfo){
        fastTune();
      }
      else{
        switchVFO(VFO_A);
      }
      break;
    }
    case BUTTON_VFOB:
    {
      if(VFO_B == globalSettings.activeVfo){
        fastTune();
      }
      else{
        switchVFO(VFO_B);
      }
      break;
    }
    case BUTTON_80:
    {
      switchBand(3500000L);
      break;
    }
    case BUTTON_40:
    {
      switchBand(7000000L);
      break;
    }
    case BUTTON_30:
    {
      switchBand(10000000L);
      break;
    }
    case BUTTON_20:
    {
      switchBand(14000000L);
      break;
    }
    case BUTTON_17:
    {
      switchBand(18000000L);
      break;
    }
    case BUTTON_15:
    {
      switchBand(21000000L);
      break;
    }
    case BUTTON_10:
    {
      switchBand(28000000L);
      break;
    }
    case BUTTON_FRQ:
    {
      enterFreq();
      break;
    }
    case BUTTON_MNU:
    {
      doSetup2();
      break;
    }
    default:
    {
      //Serial.print(F("Unknown command: "));
      //Serial.println(button.id);
      break;
    }
  }
}

void  checkTouch(){
  if (!readTouch())
    return;

  while(readTouch())
    checkCAT();
  scaleTouch(&ts_point);
 
  /* //debug code
  Serial.print(ts_point.x); Serial.print(' ');Serial.println(ts_point.y);
  */
  for (int i = 0; i < BUTTON_TOTAL; i++){
    Button button;
    memcpy_P(&button, &(btn_set[i]), sizeof(Button));

    int x2 = button.x + button.w;
    int y2 = button.y + button.h;

    if(button.x < ts_point.x && ts_point.x < x2 && 
       button.y < ts_point.y && ts_point.y < y2)
      doCommand(&button);
  }
}

//returns true if the button is pressed
int btnDown(){
  if (digitalRead(FBUTTON) == HIGH)
    return 0;
  else
    return 1;
}


void drawFocus(int ibtn, int color){
  Button button;

  memcpy_P(&button, &(btn_set[ibtn]), sizeof(Button));
  displayRect(button.x, button.y, button.w, button.h, color);
}

void doCommands(){
  int select = 0;
  int prev_button = 0;

  //wait for the button to be raised up
  while(btnDown())
    active_delay(50);
  active_delay(50);  //debounce

  Button button;
  memcpy_P(&button, &(btn_set[select/10]), sizeof(Button));
  morseLetter(button.morse);

  while (true){
    //check if the knob's button was pressed
    if (btnDown()){
      doCommand(&button);

      //unfocus the buttons
      drawFocus(select, COLOR_INACTIVE_BORDER);
      if (VFO_A == globalSettings.activeVfo)
        drawFocus(BUTTON_VFOA, COLOR_ACTIVE_BORDER);
      else
        drawFocus(BUTTON_VFOB, COLOR_ACTIVE_BORDER);
        
      //wait for the button to be up and debounce
      while(btnDown())
        active_delay(100);
      active_delay(500);
      return;
    }

    int knob = enc_read();
    
    if (knob == 0){
      active_delay(50);
      continue;
    }
    
    if (knob > 0){
      if (select + knob < BUTTON_TOTAL * 10)
        select += knob;
    }
    if (knob < 0 && select + knob >= 0)
      select += knob;      //caught ya, i is already -ve here, so you add it
    
    if (prev_button == select / 10)
      continue;
    
    //we are on a new button
    drawFocus(prev_button, COLOR_INACTIVE_BORDER);
    drawFocus(select/10, COLOR_ACTIVE_BORDER);
    memcpy_P(&button, &(btn_set[select/10]), sizeof(Button));
    morseLetter(button.morse);
    enc_read();//Clear any rotation that was done while the letter was playing
    prev_button = select/10;
  }
//  guiUpdate();

  //debounce the button
  while(btnDown())
    active_delay(50);
  active_delay(50);

  checkCAT();
}
