/*
  Solar Powered Lighted Sign
  This sketch uses an ATTINY85 to monitor a 6v solar cell and 4.2v battery.
  When the sun goes down (solar cell voltage is low), a PWM signal is sent to a
  MOSFET. The MOSFET is connected to an LED. When battery voltage drops below about 
  3.2 v, the signal to the MOSFET is stopped and the LED is turned off.
  When the LED is OFF, the ATTINY is put into sleep-power down to save battery power.
  Because the solar cell is 6v, a voltage divider is required to bring the voltage into
  range for the analog input.
  For monitoring, SendOnlySoftwareSerial is used to send a 9600 baud output stream.
  Connect this to an FTDI USB interface or another Arduino to monitor the stream.
  
  You can get the SendOnlySoftwareSerial library at https://github.com/nickgammon/SendOnlySoftwareSerial.
  
  Circuit:
  - ATTINY85
  - Connect a 6v solar cell to a charge controller.
  - Connect the charge controller output to the battery.
  - connect a 5v booster/regulator to the battery with output to the ATTINY.
      A constant supply voltage is required as a referenece for the analog to digital conversion.
      The ATTINY could be connected to the 4.2v cell, but the reference would change as the battery charge changes.
  - use a voltage divider on the + and - of the solar cell (or the charge controller input).
      solar + 10k ohm--------ATTINY (A3)---------2k ohm solar - 
  - Connect the gate of a n-channel MOSFET to pin 0.
  - Connect the source and drain of the MOSFET to the LED - and ground.
  - Connect the LED + to the battery +
  - Connect ATTINY A1 to the battery + with a 1k ohm current limiting resistor
      ATTTINY A1 ---->  1k ohm   -------> Battery +
      This allows the ATTINTY to be programmed while in circuit.
  - connect all the grounds together
      battery, solar cell, and ATTINY
      don't connect the LED - to ground. This connection goes through the N channel MOSFET.
  

The ATTINY85 does not have Serial output. We are tricking the IDE by declaring a 
SendOnlySoftwareSerial called Serial.

     
  created 3 Mar 2020
  by John Westbury
*/



#include <SendOnlySoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define Solar_Pin A3
#define Battery_Pin A1
#define MOSFET_Pin 0
#define SLEEP_CYCLES 72   // Each Cycle is eight seconds
#define sunLevelToTurnOn 300
#define sunLevelToReset 500
#define batteryLevelToShutoff 700

SendOnlySoftwareSerial Serial (4);

void setup() {
 
  Serial.begin(9600); 
  Serial.println("\n\n\nRun----\n");
  
}


bool allowLED = true;
long onTime = 0; //9000 seconds is 2.5 hours


void loop() {
    Serial.println("LOOP Begin----");
    long sunValue = analogRead(Solar_Pin);
    long batValue = analogRead(Battery_Pin);
    
  for (int j=0;j<20;j++){
    delay(20);  
    sunValue = (analogRead(Solar_Pin) + sunValue + sunValue)/3;
    batValue = (analogRead(Battery_Pin) + batValue + batValue)/3;
   
  }

  if (sunValue >= sunLevelToReset) allowLED = true;  
  if (batValue <  batteryLevelToShutoff) allowLED = false;
  if (onTime > 900) {
    allowLED = false;
    onTime=0;
  }
  
  Serial.print("S V = ");
  Serial.println(sunValue);
  Serial.print("B Avg = ");
  Serial.println(batValue);
  Serial.print("ALLOW = ");
  Serial.println(allowLED);
  Serial.print("ON TIME = ");
  Serial.println(onTime);

  
  if (sunValue < sunLevelToTurnOn && batValue > batteryLevelToShutoff && allowLED)
  {
    Serial.println("ON - delay");
    analogWrite(MOSFET_Pin,24); //light on at 24/255 duty cycle
    delay(5000);
    onTime++; // 2.5 hours max on time)
  }
   
  if (sunValue > sunLevelToTurnOn  || batValue < batteryLevelToShutoff || !allowLED)
  {
    Serial.println("OFF - SLEEP");
    
    analogWrite(MOSFET_Pin,0);
    for (int j=0;j<SLEEP_CYCLES;j++)
    { shutDown_with_WD (0b100001);  // 8 seconds
      delay(50);
    }
  }
  
  
  delay(5000);
}


//This function is called when the watchdog timer expires. 

ISR(WDT_vect) {
    
    wdt_disable();  // disable watchdog
    
}

//This function puts the ATTINY in sleep mode.
//To save power, the ADCs are explicitly shutdown.
 
void shutDown_with_WD(const byte time_len)
  {

  noInterrupts();

  wdt_reset();
   
  MCUSR = 0;                          // reset flags
  WDTCR |= 0b00011000;               // set WDCE, WDE
  WDTCR =  0b01000110 | time_len;    // set WDIE and delay
 
  ADCSRA &= ~_BV(ADEN);              //Stop the adc

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_bod_disable();
  interrupts();
  ADCSRA |= _BV(ADEN);                    // ADC on
  sleep_mode();     
 
  }
