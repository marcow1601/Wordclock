// General WiFi connectivity
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DNSServer.h>

// WiFiManager for remote WiFi configuration
#include <WiFiManager.h>         

// ArduinoOTA for over-the-air software updates
#include <ArduinoOTA.h>

// NTP and time keeping
#include <TimeLib.h>
#include <NtpClientLib.h>

// LED output
#include <Adafruit_NeoPixel.h>

int8_t timeZone = 1;
int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;

int8_t nowHour;
int8_t nowMinute;
int8_t minuteRounded;
int8_t hourCompensated;

int display[10][11] = {0};
Adafruit_NeoPixel leds = Adafruit_NeoPixel(114, 5, NEO_GRB + NEO_KHZ800);

char text[10][11] = {
  {'E','S','K','I','S','T','A','F','Ü','N','F'},
  {'Z','E','H','N','Z','W','A','N','Z','I','G'},
  {'D','R','E','I','V','I','E','R','T','E','L'},
  {'V','O','R','F','U','N','K','N','A','C','H'},
  {'H','A','L','B','A','E','L','F','Ü','N','F'},
  {'E','I','N','S','X','A','M','Z','W','E','I'},
  {'D','R','E','I','P','M','J','V','I','E','R'},
  {'S','E','C','H','S','N','L','A','C','H','T'},
  {'S','I','E','B','E','N','Z','W','Ö','L','F'},
  {'Z','E','H','N','E','U','N','K','U','H','R'},
};

void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

boolean syncEventTriggered = false; // True if a time event has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void setup() {
  pinMode (LED_BUILTIN, OUTPUT); // Onboard LED
  digitalWrite (LED_BUILTIN, HIGH); // Switch off LED

  // Init leds to turn off
  leds.begin();
  leds.show();

  Serial.begin (115200);
  Serial.println ();

  //WiFiManager
  WiFiManager wifiManager;
  //FOR TESTING: reset saved settings
  //wifiManager.resetSettings();
  
  wifiManager.autoConnect("WordclockAP", "passwort");

  wifiFirstConnected = true;
  digitalWrite (LED_BUILTIN, LOW);

  ArduinoOTA.setHostname("wordclock");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  
  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
      ntpEvent = event;
      syncEventTriggered = true;
  });

}

void debugOutput(){
  for(int8_t row=0; row<10; row++){
      for(int8_t col=0; col<11; col++){
        if(display[row][col] ==1){
          Serial.print(text[row][col]);
        }
      }
      Serial.print("\n");
    }
}

void loop() {
  ArduinoOTA.handle();
  
  static int last = 0;

  if (wifiFirstConnected) {
      wifiFirstConnected = false;
      NTP.begin("pool.ntp.org", timeZone, true, minutesTimeZone);
      NTP.setInterval(63);
  }

  if (syncEventTriggered) {
      processSyncEvent (ntpEvent);
      syncEventTriggered = false;
  }

  if ((millis () - last) > 5000) {
    last = millis ();
    nowHour = hour()%12;
    nowMinute = minute();

    Serial.print(nowHour);
    Serial.print(":");
    Serial.println(nowMinute);

    minuteRounded = nowMinute-(nowMinute%5);
    if(minuteRounded >= 25) hourCompensated = (nowHour + 1)%12;
    else hourCompensated = nowHour; 

    // Reset array to zeroes
    for(int8_t row=0; row<10; row++){
      for(int8_t col=0; col<11; col++){
        display[row][col] = 0;
        leds.setPixelColor(row*11+col, 0,0,0);
      }
    }
    
    // "ES IST"
    display[0][0] = 1;
    display[0][1] = 1;
    
    display[0][3] = 1;
    display[0][4] = 1;
    display[0][5] = 1;

    // Hour
    // "ZWÖLF"
    if(hourCompensated == 0){
      display[8][6] = 1;
      display[8][7] = 1;
      display[8][8] = 1;
      display[8][9] = 1;
      display[8][10] = 1;
    }
    // "EINS"
    else if(hourCompensated == 1){
      display[5][0] = 1;
      display[5][1] = 1;
      display[5][2] = 1;
      display[5][3] = 1;
    }
    // "ZWEI"
    else if(hourCompensated == 2){
      display[5][7] = 1;
      display[5][8] = 1;
      display[5][9] = 1;
      display[5][10] = 1;
    }
    // "DREI"
    else if(hourCompensated == 3){
      display[6][0] = 1;
      display[6][1] = 1;
      display[6][2] = 1;
      display[6][3] = 1;
    }
    // "VIER"
    else if(hourCompensated == 4){
      display[6][7] = 1;
      display[6][8] = 1;
      display[6][9] = 1;
      display[6][10] = 1;
    }
    // "FÜNF"
    else if(hourCompensated == 5){
      display[4][7] = 1;
      display[4][8] = 1;
      display[4][9] = 1;
      display[4][10] = 1;
    }
    // "SECHS"
    else if(hourCompensated == 6){
      display[7][0] = 1;
      display[7][1] = 1;
      display[7][2] = 1;
      display[7][3] = 1;
      display[7][4] = 1;
    }
    // "SIEBEN"
    else if(hourCompensated == 7){
      display[8][0] = 1;
      display[8][1] = 1;
      display[8][2] = 1;
      display[8][3] = 1;
      display[8][4] = 1;
      display[8][5] = 1;
    }
    // "ACHT"
    else if(hourCompensated == 8){
      display[7][7] = 1;
      display[7][8] = 1;
      display[7][9] = 1;
      display[7][10] = 1;
    }
    // "NEUN"
    else if(hourCompensated == 9){
      display[9][3] = 1;
      display[9][4] = 1;
      display[9][5] = 1;
      display[9][6] = 1;
    }
    // "ZEHN"
    else if(hourCompensated == 10){
      display[9][0] = 1;
      display[9][1] = 1;
      display[9][2] = 1;
      display[9][3] = 1;
    }
    // "ELF"
    else if(hourCompensated == 11){
      display[4][5] = 1;
      display[4][6] = 1;
      display[4][7] = 1;
    }
    
    // Is full hour?
    if(minuteRounded == 0){
      if(hourCompensated == 1) display[5][3] == 0; // "EINS" -> "EIN"

      // "UHR"
      display[9][8] = 1;
      display[9][9] = 1;
      display[9][10] = 1;
    }
    else {
      // "FÜNF NACH"
      if(minuteRounded == 5){
        // Fünf Minuten
        display[0][7] = 1;
        display[0][8] = 1;
        display[0][9] = 1;
        display[0][10] = 1;

        // Nach
        display[3][7] = 1;
        display[3][8] = 1;
        display[3][9] = 1;
        display[3][10] = 1;
      }
      // "ZEHN NACH"
      else if(minuteRounded == 10){
        // Zehn Minuten
        display[1][0] = 1;
        display[1][1] = 1;
        display[1][2] = 1;
        display[1][3] = 1;

        // Nach
        display[3][7] = 1;
        display[3][8] = 1;
        display[3][9] = 1;
        display[3][10] = 1;
      }
      // "VIERTEL NACH"
      else if(minuteRounded == 15){
        // Viertel
        display[2][4] = 1;
        display[2][5] = 1;
        display[2][6] = 1;
        display[2][7] = 1;
        display[2][8] = 1;
        display[2][9] = 1;
        display[2][10] = 1;

        // Nach
        display[3][7] = 1;
        display[3][8] = 1;
        display[3][9] = 1;
        display[3][10] = 1;
        
      }
      // "ZWANZIG NACH"
      else if(minuteRounded == 20){
        // Zwanzig
        display[1][4] = 1;
        display[1][5] = 1;
        display[1][6] = 1;
        display[1][7] = 1;
        display[1][8] = 1;
        display[1][9] = 1;
        display[1][10] = 1;

        // Nach
        display[3][7] = 1;
        display[3][8] = 1;
        display[3][9] = 1;
        display[3][10] = 1;
      }
      // "FÜNF VOR HALB"
      else if(minuteRounded == 25){
        // Fünf Minuten
        display[0][7] = 1;
        display[0][8] = 1;
        display[0][9] = 1;
        display[0][10] = 1;

        // Vor
        display[3][0] = 1;
        display[3][1] = 1;
        display[3][2] = 1;

        // Halb
        display[4][0] = 1;
        display[4][1] = 1;
        display[4][2] = 1;
        display[4][3] = 1;
      }
      // "HALB"
      else if(minuteRounded == 30){
        // Halb
        display[4][0] = 1;
        display[4][1] = 1;
        display[4][2] = 1;
        display[4][3] = 1;
      }
      // "FÜNF NACH HALB"
      else if(minuteRounded == 35){
        // Fünf Minuten
        display[0][7] = 1;
        display[0][8] = 1;
        display[0][9] = 1;
        display[0][10] = 1;

        // Nach
        display[3][7] = 1;
        display[3][8] = 1;
        display[3][9] = 1;
        display[3][10] = 1;

        // Halb
        display[4][0] = 1;
        display[4][1] = 1;
        display[4][2] = 1;
        display[4][3] = 1;
      }
      // "ZWANGZIG VOR"
      else if(minuteRounded == 40){
        // Zwanzig
        display[1][4] = 1;
        display[1][5] = 1;
        display[1][6] = 1;
        display[1][7] = 1;
        display[1][8] = 1;
        display[1][9] = 1;
        display[1][10] = 1;

        // Vor
        display[3][0] = 1;
        display[3][1] = 1;
        display[3][2] = 1;
      }
      // "DREIVIERTEL"
      else if(minuteRounded == 45){
        // Dreiviertel
        display[2][0] = 1;
        display[2][1] = 1;
        display[2][2] = 1;
        display[2][3] = 1;
        display[2][4] = 1;
        display[2][5] = 1;
        display[2][6] = 1;
        display[2][7] = 1;
        display[2][8] = 1;
        display[2][9] = 1;
        display[2][10] = 1;
      }
      // "ZEHN VOR"
      else if(minuteRounded == 50){
        // Zehn Minuten
        display[1][0] = 1;
        display[1][1] = 1;
        display[1][2] = 1;
        display[1][3] = 1;

        // Vor
        display[3][0] = 1;
        display[3][1] = 1;
        display[3][2] = 1;
      }
      // "FÜNF VOR"
      else if(minuteRounded == 55){
        // Fünf Minuten
        display[0][7] = 1;
        display[0][8] = 1;
        display[0][9] = 1;
        display[0][10] = 1;

        // Vor
        display[3][0] = 1;
        display[3][1] = 1;
        display[3][2] = 1;
      }      
    }
    debugOutput();

    for(int8_t row=0; row<10; ++row){
      for(int8_t col=0; col<11; ++col){
        if(display[row][col] == 1){
          // Odd row => right to left number
          if(row%2){
            // Dim LEDs between 11pm and 6am
            if(hour()>=6 && hour()<=22)
              leds.setPixelColor(row*11+(10-col), 175,175,175);
            else
              leds.setPixelColor(row*11+(10-col), 25,25,25);
          }
          // Even row => left to right number
          else {
            if(hour()>=6 && hour()<=22)
              leds.setPixelColor(row*11+col, 175,175,175);
            else
              leds.setPixelColor(row*11+col, 25,25,25);
          }
        }
      }
    }

    // Display one minute increments between five minute steps using 4 additional LEDs
    // Turn off all four LEDs
    leds.setPixelColor(110, 0,0,0);
    leds.setPixelColor(111, 0,0,0);
    leds.setPixelColor(112, 0,0,0);
    leds.setPixelColor(113, 0,0,0);
    // Turn on between 0 and 4 additional LEDs
    for(int8_t minIncrement=110; (minIncrement-110)<(nowMinute%5); minIncrement++){
      leds.setPixelColor(minIncrement, 175,175,175);
    }
    
    leds.show();
  }
  delay (0);
}
