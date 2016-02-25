#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <ESP8266WiFi.h>

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

#define USE_SERIAL Serial
#define AVG_COUNT 100
#define SLEEP_TIME 300
/* #define R1 100000
   #define R2 20000           This is a 6:1 divider, so just define 6
*/
#define divider 6   
#define ESP_ref 0.96  /* assume ESP8266 refence voltage internally is 0.96 volts) */

float ESP8266_resolution = ESP_ref / 1024; 
// float divider = 1/ (R1 / (R1 + R2));
int average = 0;
int x = 0;
int level = 0;

const char* ssid     = "SSID";
const char* password = "PASSWORD";
 
const char* host = "IP ADDRESS of Host";

void setup(void) 
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello!");
  
  Serial.println("ADC Range: +/- 1.024V  1 bit = 0.5mV/ADS1015)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(void) 
{
  int16_t adc0;
  float adcin;
  float battery;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // Read ADS1015 and compute voltage
  average = 0;
  for(x = 0; x < AVG_COUNT; x++) {
    level = ads.readADC_SingleEnded(0);
    average = average + level;
  }
  
  adc0 = average / AVG_COUNT;
  adcin = adc0 * 0.0005;
  battery = adcin * divider;

  Serial.print("AIN0: "); Serial.print(adc0); Serial.print(" ADC input= "); Serial.print(adcin); Serial.print(" BAT= "); Serial.print(battery);

  // Read ESP8266 internall adc and average over AVG_COUNT
  average = 0;
  for(x = 0; x < AVG_COUNT; x++) {
    level = analogRead(A0);
    average = average + level;
  }
  level = average / AVG_COUNT;

  float bat_adc = ((level * ESP8266_resolution) * divider);

  // convert battery level to percent
  int pct = map(level, 530, 733, 0, 100);
  Serial.println();
   
  // We now create a URI for the request
  String url = "/bat8266.php?adc_1015=" + String(adc0) + "&bat_1015=" + String(battery) + "&adc_8266=" + String(level) + "&bat_8266=" + String(bat_adc);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.print("Seconds to sleep ="); Serial.println(SLEEP_TIME);
  Serial.println(" ");

  ESP.deepSleep(SLEEP_TIME * 1000000, WAKE_RF_DEFAULT);
  delay(1000);
}
