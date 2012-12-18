#include <LiquidCrystal.h>

#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 

byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
LiquidCrystal lcdDisplay(8, 9, 4, 5, 6, 7);
int returnval[5];
int currentLeg = 0;

byte frequencyTable[16][5] = {
        {0x43, 0x90, 0x25, 0x00},
        {0x43, 0x90, 0x25, 0x00},
        {0x03, 0x05, 0x25, 0x00},        
        {0x43, 0x90, 0x35, 0x00},
        {0x03, 0x05, 0x25, 0x00},
        {0x03, 0x05, 0x50, 0x00},
        {0x43, 0x90, 0x25, 0x00},
        {0x05, 0x21, 0x00, 0x00},
        {0x03, 0x05, 0x25, 0x00},
        {0x03, 0x05, 0x25, 0x00},
        {0x43, 0x90, 0x00, 0x00},
        {0x03, 0x00, 0x00, 0x00},
        {0x03, 0x05, 0x25, 0x00},
        {0x00, 0x35, 0x85, 0x00},
        {0x05, 0x21, 0x00, 0x00},
        {0x14, 0x40, 0x00, 0x00}
};

String huntList[16] = {
        "Hunt 2, Leg 1",
        "Hunt 3, Leg 1",
        "Hunt 3, Leg 2",
        "Hunt 3, Leg 3",
        "Hunt 4, Leg 1",
        "Hunt 4, Leg 2",
        "Hunt 4, Leg 3",
        "Hunt 4, Leg 4",
        "Hunt 4, Leg 5",
        "Hunt 6, Leg 1",
        "Hunt 6, Leg 2",
        "Hunt 6, Leg 3",
        "Hunt 8, Leg 1",
        "Hunt 8, Leg 2",
        "Hunt 8, Leg 3",
        "Hunt 9, Leg 1"
};

void setup() {
 lcdDisplay.begin(16, 2);
 Serial.begin(9600);
 pinMode( BUTTON_ADC_PIN, INPUT );
 digitalWrite( BUTTON_ADC_PIN, LOW );
 setRadioFrequency(frequencyTable[0][0], frequencyTable[0][1], frequencyTable[0][2], frequencyTable[0][3], 0); 
}


void loop() {
 byte button;
 button = ReadButtons();
 lcdDisplay.clear();
 String frequency = getFrequencyAndMode();
 lcdDisplay.print(frequency);
 lcdDisplay.setCursor(0,1);
 lcdDisplay.print(huntList[currentLeg]);
 checkButton();
 delay(100);
  
}

void checkButton() {
 byte button;
 button = ReadButtons();
 
 switch( button ) {
  case BUTTON_NONE: {
   break;
  }
  case BUTTON_RIGHT: {
   if (currentLeg == 15) break;
   currentLeg = currentLeg + 1;
   setRadioFrequency(frequencyTable[currentLeg][0], frequencyTable[currentLeg][1], frequencyTable[currentLeg][2], frequencyTable[currentLeg][3], 0); 
   delay(500);
   break;
  }
  case BUTTON_UP: {
   break;
  }
  case BUTTON_DOWN: {
   break;
  }
  case BUTTON_LEFT: {
   if (currentLeg == 0) break;
   currentLeg = currentLeg - 1;
   setRadioFrequency(frequencyTable[currentLeg][0], frequencyTable[currentLeg][1], frequencyTable[currentLeg][2], frequencyTable[currentLeg][3], 0); 
   delay(500);
   break;
  }
  case BUTTON_SELECT: {
   break;
  }
  default: {
   break;
  }
 }
}

void setRadioFrequency(byte f1, byte f2, byte f3, byte f4, int offset ) {
 
 sendCommandToRadio(f1, f2, f3, f4, 1, true);
 lcdDisplay.setCursor(0,1);
  
}

String getFrequencyAndMode() {
  
 sendCommandToRadio(0, 0, 0, 0, 3, 0); 
 return "F: " + frequencyByteExpander(returnval[0], false) + frequencyByteExpander(returnval[1], true) + frequencyByteExpander(returnval[2], false) + frequencyByteExpander(returnval[3], false) + " MHz";
  
}

void sendCommandToRadio(byte msb, byte lsb, byte data1, byte data2, byte cmd, boolean writeToRadio) {
 
 Serial.write(msb);
 Serial.write(lsb);
 Serial.write(data1);
 Serial.write(data2);
 Serial.write(cmd);
 
 Serial.flush();
 
 /*
 if (writeToRadio == true) {
  byte throwaway = Serial.read();
 }*/  
   
 returnval[0] = Serial.read();
 returnval[1] = Serial.read();
 returnval[2] = Serial.read();
 returnval[3] = Serial.read();
 returnval[4] = Serial.read();
 
}

String frequencyByteExpander(byte frequencyByte, boolean decimalPoint) {
  
 byte byteLeft = (frequencyByte & 240) >> 4;
 byte byteRight = (frequencyByte & 15);
 
 if (!decimalPoint) {
  return String(byteLeft) + String(byteRight);
 } else { 
  return String(byteLeft) + '.' + String(byteRight);
 }
  
}

byte ReadButtons()
{
   unsigned int buttonVoltage;
   byte button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn
   
   //read the button ADC pin voltage
   buttonVoltage = analogRead( BUTTON_ADC_PIN );
   //sense if the voltage falls within valid voltage windows
   if( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_RIGHT;
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_UP;
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_DOWN;
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_LEFT;
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_SELECT;
   }
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
   
   //save the latest button value, for change event detection next time round
   buttonWas = button;
   
   return( button );
}
