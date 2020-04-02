# ATTINY85---Low-Power-for-Solar-Powered-Light-Box
Arduino code for ATTINY 85. Two analog inputs are used to sense voltage from a solar cell and battery. One PWM output is used for driving a MOSFET. The ATTINY is put in Sleep - Power Off - ADC off, and then recovers using a watchdog timer.

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
