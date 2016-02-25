ESP8266 ADC Test/Comparison

I was having a difficult time understanding how to interpret the results of the ESP8266 ADC. The popular guidance
was to "keep the input to no more than 1 volt," which would lead you to believe that the reference for this
ADC was 1 volt.  I found some discussions that said the reference is 0.96 volts, which given my test is close.

So I put an ADS1015 ADC on a proto board and connected it with I2C, and set up a test to read the ADS1015 and the ESP8266 ADC and send the reults to a mysql database running on tha raspberry pi.

Battery Monitor - Test Conditions:

Adafruit Feather ESP8266, fitted with a 500 ma-h lithium-ion battery and an Adafruit ADS1015 ADC breakout board												
ADS1015 is only sampled once (column C)	

Arduino code samples the ESP8266 10 times and provides an averaged result (column D)

ESP8266 sends the data to a Rapberry Pi2 that is running mysql and  is then put to sleep for 5 minutes.			
		ESP.deepSleep(SLEEP_TIME * 1000000, WAKE_RF_DEFAULT);	

Voltage Divider:  100K + 20K resistors (1%) measured values. 100K resistor is tied to the Battery output and then connected to the 20K  
		resistor and the other side of 20K resister tied to ground	

Both ESP8266 and ADS1015 were measuring the voltage across the 20K resistor and ground

ADS1015 was set for a gain of four -> 1 bit = 0.5mV

Column C (ADS1015_v) is therefore the value in column B (ADS1015-raw) * 0.5 mV * 6 (six is the voltage divider ratio)												
Column D (ESP8266_ADC) is the value returned by the 8266 adc.

Column E (ESP8266_v) is the column D value multiplied by (0.96v/1024), which is the 1 bit value, and then multiplied by 6 (voltage 
          divider) Using 0.96v as the reference voltage for the adc.									
												
												
Observations:

	ESP8266 adc values are what they are.  Question is, how do these readings map to the ads1015 readings?

	Column G is the percentage error that the ESP9266 differs from the ADS1015 (scaled by the voltage divider ratio).
													
	Error is between 0 % and 2%

	The battery ran the ESP8266 for 2.5 days for this test. As documented by Adafruit, their Lithium-Ion batteries shut off at 3 volts,
	hence the data ends at 3 volts :-)  It took about four hours to recharge the battery with the HUZZAH Feather ESP8266 plugged into 
	a USB port on the computer (Feather charges at 100 ma)

