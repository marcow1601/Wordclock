#include <TimeLib.h>
#include "WifiConfig.h"
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>

#ifndef WIFI_CONFIG_H
#define YOUR_WIFI_SSID "YOUR_WIFI_SSID"
#define YOUR_WIFI_PASSWD "YOUR_WIFI_PASSWD"
#endif // !WIFI_CONFIG_H

int8_t timeZone = 1;
int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;

int8_t nowHour;
int8_t nowMinute;

int display[10][11] = {0};

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

void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
    Serial.printf ("Connected to %s\r\n", ipInfo.ssid.c_str ());
}

// Start NTP only after IP network is connected
void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
    Serial.printf ("Got IP: %s\r\n", ipInfo.ip.toString ().c_str ());
    Serial.printf ("Connected: %s\r\n", WiFi.status () == WL_CONNECTED ? "yes" : "no");
    digitalWrite (LED_BUILTIN, LOW); // Turn on LED
    wifiFirstConnected = true;
}

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

// Manage network disconnection
void onSTADisconnected (WiFiEventStationModeDisconnected event_info) {
    Serial.printf ("Disconnected from SSID: %s\n", event_info.ssid.c_str ());
    Serial.printf ("Reason: %d\n", event_info.reason);
    digitalWrite (LED_BUILTIN, HIGH); // Turn off LED
    //NTP.stop(); // NTP sync can be disabled to avoid sync errors
}

boolean syncEventTriggered = false; // True if a time event has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void setup() {
  static WiFiEventHandler e1, e2, e3;

    Serial.begin (115200);
    Serial.println ();
    WiFi.mode (WIFI_STA);
    WiFi.begin (YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

    pinMode (LED_BUILTIN, OUTPUT); // Onboard LED
    digitalWrite (LED_BUILTIN, HIGH); // Switch off LED

    NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
        ntpEvent = event;
        syncEventTriggered = true;
    });

    e1 = WiFi.onStationModeGotIP (onSTAGotIP);// As soon WiFi is connected, start NTP Client
    e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
    e3 = WiFi.onStationModeConnected (onSTAConnected);

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
  static int last = 0;

  if (wifiFirstConnected) {
      wifiFirstConnected = false;
      NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
      NTP.setInterval (63);
  }

  if (syncEventTriggered) {
      processSyncEvent (ntpEvent);
      syncEventTriggered = false;
  }

  if ((millis () - last) > 30000) {
    last = millis ();
    nowHour = hour()%12;
    nowMinute = minute();

    int8_t minuteRounded = nowMinute-(nowMinute%5);
    for(int8_t row=0; row<10; row++){
      for(int8_t col=0; col<11; col++){
        display[row][col] = 0;
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
    if(nowHour == 0){
      display[8][6] = 1;
      display[8][7] = 1;
      display[8][8] = 1;
      display[8][9] = 1;
      display[8][10] = 1;
    }
    // "EINS"
    else if(nowHour == 1){
      display[5][0] = 1;
      display[5][1] = 1;
      display[5][2] = 1;
      display[5][3] = 1;
    }
    // "ZWEI"
    else if(nowHour == 2){
      display[5][7] = 1;
      display[5][8] = 1;
      display[5][9] = 1;
      display[5][10] = 1;
    }
    // "DREI"
    else if(nowHour == 3){
      display[6][0] = 1;
      display[6][1] = 1;
      display[6][2] = 1;
      display[6][3] = 1;
    }
    // "VIER"
    else if(nowHour == 4){
      display[6][7] = 1;
      display[6][8] = 1;
      display[6][9] = 1;
      display[6][10] = 1;
    }
    // "FÜNF"
    else if(nowHour == 5){
      display[4][7] = 1;
      display[4][8] = 1;
      display[4][9] = 1;
      display[4][10] = 1;
    }
    // "SECHS"
    else if(nowHour == 6){
      display[7][0] = 1;
      display[7][1] = 1;
      display[7][2] = 1;
      display[7][3] = 1;
      display[7][4] = 1;
    }
    // "SIEBEN"
    else if(nowHour == 7){
      display[8][0] = 1;
      display[8][1] = 1;
      display[8][2] = 1;
      display[8][3] = 1;
      display[8][4] = 1;
      display[8][5] = 1;
    }
    // "ACHT"
    else if(nowHour == 8){
      display[7][7] = 1;
      display[7][8] = 1;
      display[7][9] = 1;
      display[7][10] = 1;
    }
    // "NEUN"
    else if(nowHour == 9){
      display[9][3] = 1;
      display[9][4] = 1;
      display[9][5] = 1;
      display[9][6] = 1;
    }
    // "ZEHN"
    else if(nowHour == 10){
      display[9][0] = 1;
      display[9][1] = 1;
      display[9][2] = 1;
      display[9][3] = 1;
    }
    // "ELF"
    else if(nowHour == 11){
      display[4][5] = 1;
      display[4][6] = 1;
      display[4][7] = 1;
    }
    
    // Is full hour?
    if(minuteRounded == 0){
      if(nowHour == 1) display[5][3] == 0; // "EINS" -> "EIN"

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
  }
  delay (0);
}
