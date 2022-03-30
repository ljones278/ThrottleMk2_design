/*
 * Example code to operate the grammeter throttle. This 
 * 
 */


#include <Joystick.h>
#define NUMBER_OF_SHIFT_CHIPS   3
#define DATA_WIDTH   8*NUMBER_OF_SHIFT_CHIPS
#define PULSE_WIDTH_USEC   5
#define POLL_DELAY_MSEC   1
#define BYTES_VAL_T unsigned int

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;
// Create the Joystick
Joystick_ Joystick;


long bitValRight = 0;
uint8_t bitValLeft = 0;

int leftT = 0;
int rightT = 0;
int wheel = 0;
int tdcV = 0;
int tdcH = 0;

/*
 * UPDATE THESE TO MATCH YOUR WIRING
 */

int clockPin        = 3; // Connects to the Clock pin the 165
int ploadPin        = 5;  // Connects to Parallel load pin the 165
int clockInhibitPin  = 2;  // Connects to Clock Enable pin the 165
int dataPinLeft        = 15; // Connects to the Q7 pin the 165
int dataPinRight        = 16;

// Constant that maps the physical pin to the joystick button.
const int LeftThrottlePin = A2;
const int RightThrottlePin = A3;
const int TDCLRPin = A8;
const int TDCUDPin = A6;
const int WHEELPin = A1;
uint8_t Shift1Val = 0x00;
uint8_t Shift2Val = 0x00;
uint8_t Shift1ValTemp = 0x00;
uint8_t Shift2ValTemp = 0x00;


BYTES_VAL_T read_shift_regs()
{


    
    BYTES_VAL_T bytesVal = 0;
    /*
     * Start with processing the shift registers.
     */
    
    /* Trigger a parallel Load to latch the state of the data lines,
    */
    digitalWrite(clockInhibitPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(ploadPin, LOW);
    delayMicroseconds(10);
    digitalWrite(ploadPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(clockInhibitPin, LOW);
    delayMicroseconds(40);
    /* Loop to read each bit value from the serial out line
     * of the SN74HC165N.
    */
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitValRight = digitalRead(dataPinRight);
        
        /* Set the corresponding bit in bytesVal.
        */
        Joystick.setButton(i+8,(~bitValRight)&0x01);
        if(i <8)
        {
          bitValLeft = digitalRead(dataPinLeft);
          Joystick.setButton(i,(~bitValLeft)&0x01);
        }
        
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(50);
        digitalWrite(clockPin, LOW);
    }
  /*
   * NOTE: you will need to update the map values to match what your arduino is actually seeing.
   * Mapping from any reduced ranges to the full 0-1024 range helps keep things sane in a flight 
   * simulator control panel.
   */
   
  /*
   * TDC analog inputs
   * 
   */
    
    tdcV = analogRead(TDCUDPin);
    if(tdcV >555)
    {
      tdcV = map(tdcV, 555, 879, 512, 1023);
    }
    else
    {
      tdcV = map(tdcV, 227, 555, 0, 512);
    }
    Joystick.setRyAxis(tdcV);

    tdcH = analogRead(TDCLRPin);
    if(tdcH >492)
    {
      tdcH = map(tdcH, 492, 842, 512, 1023);
    }
    else
    {
      tdcH = map(tdcH, 160, 492, 0, 512);
    }
    Joystick.setRxAxis(tdcH);

    
    /*
     * Throttle analog inputs
     */
    
    leftT = map(analogRead(LeftThrottlePin),282, 797, 0, 1023);
    Joystick.setYAxis(leftT);

  
    rightT = map(analogRead(RightThrottlePin),250, 870, 0, 1023);
    Joystick.setXAxis(rightT);

    /*
     * The WHEEL
     * The WHEEL
     */

    wheel = analogRead(WHEELPin);
    Joystick.setZAxis(wheel);



    return(bytesVal);
}


void setup() {
  
  
  Joystick.begin();
  //Serial.begin(9600);


  //initialise shift register pins
  pinMode(ploadPin, OUTPUT);
  pinMode(clockInhibitPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPinLeft, INPUT);
  pinMode(dataPinRight, INPUT);
  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);

  // as far as I can tell you do not need to initialise pins that 
  // will be connectted to the ADC but better safe than sorry ...
  
  pinMode(LeftThrottlePin, INPUT);
  pinMode(RightThrottlePin, INPUT);
  pinMode(TDCLRPin, INPUT);
  pinMode(TDCUDPin, INPUT);
  pinMode(WHEELPin, INPUT);
    
}

void loop()
{
    read_shift_regs();
}
