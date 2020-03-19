#include <SendOnlySoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define Solar_Pin A1
#define Battery_Pin A3
#define MOSFET_Pin 0
#define SLEEP_CYCLES 10   // Each Cycle is eight seconds



SendOnlySoftwareSerial Serial (4);

void setup() {
 
  Serial.begin(9600);
  Serial.println("\n\n\nRun----\n");
  
}

// the loop function runs over and over again forever
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
    analogWrite(MOSFET_Pin,255);
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

ISR(WDT_vect) {
    
    wdt_disable();  // disable watchdog
    
}

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
