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
  


  Circuit:
  - ATTINY85
  - Connect a solar cell to a charge controller.
  - Connect the charge controller to the battery.
  - connect a 5v booster/regulator to the battery with output to the ATTINY.
      A constant supply voltage is required as a refernece for the analog to digital conversion.
      The ATTINY could be connected to the 4.2v cell, but the reference would change as the battery charge changes.
  - use a voltage divider on the + and - of the solar cell (or the charge controller input).
      solar + 10k ohm--------ATTINY (A3)---------2k ohm solar - 
  - Connect the gate of a n-channel MOSFET to pin 0.
  - Connect the gate and drain of the MOSFET to the LED - and ground.
  - Connect the LED + to the battery +
  - Connect ATTINY A1 to the battery + with a 1k ohm current limiting resistor
      ATTTINY A1 ---->  1k ohm   -------> Battery +
      This allows the ATTINTY to be programmed without problems
  - connect all the grounds together
      battery, solar cell, and ATTINY
      don't connect the LED - to ground. This goes through the N channel MOSFET.
  
     

  created 3 Mar 2020
  by John Westbury



*/
// todo : increase sleep cycles  -- shoot for 8 mins (about 64 cycles)
//        delay after sunset before going on -- and/or lower sun threshold
//        put a maximum time on light on -- like 3 hours
//        determin battery shut off -- about 3.2 v


#include <SendOnlySoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define Solar_Pin A3
#define Battery_Pin A1
#define MOSFET_Pin 0
#define SLEEP_CYCLES 1   // Each Cycle is eight seconds

SendOnlySoftwareSerial Serial (4);

void setup() {
 
  Serial.begin(9600);
  Serial.println("\n\n\nRun----\n");
  
}


void loop() {

  delay(200);
  int sunValue = analogRead(Solar_Pin);
  int batValue = analogRead(Battery_Pin);

    
  Serial.print("S V = ");
  Serial.println(sunValue);
  Serial.print("B V = ");
  Serial.println(batValue);

  if (sunValue<500 && batValue > 600)
  {
    Serial.println("ON");
    analogWrite(MOSFET_Pin,32);
    delay(5000);
  }
  if (sunValue>500  || batValue < 600)
  {
    Serial.println("OFF");
    analogWrite(MOSFET_Pin,0);
    for (int j=0;j<SLEEP_CYCLES;j++)
    { shutDown_with_WD (0b100001);  // 8 seconds
      delay(50);
    }
  }
  
  
   
  Serial.println("back");
}

//This function is called when the watchdog timer expires. 

ISR(WDT_vect) {
    
    wdt_disable();  // disable watchdog
    
}

//This function puts the ATTINY in sleep mode for 8 seconds.
//To save power, the ADCs are explicitly shutdown.
 
void shutDown_with_WD(const byte time_len)
  {

  noInterrupts();

  wdt_reset();
   
  MCUSR = 0;                          // reset various flags
  WDTCR |= 0b00011000;               // see docs, set WDCE, WDE
  WDTCR =  0b01000110 | time_len;    // set WDIE, and appropriate delay
 
  ADCSRA &= ~_BV(ADEN);

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_bod_disable();
  interrupts();
  ADCSRA |= _BV(ADEN);                    // ADC on
  sleep_mode();     
 
  }
