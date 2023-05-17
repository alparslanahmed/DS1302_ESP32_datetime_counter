/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <ThreeWire.h>  
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS1302.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define START_DATE 1652775083

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// CONNECTIONS:
// CLK --> 14
// DAT/IO --> 2
// RST --> 0

ThreeWire myWire(2,14,0); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
    Serial.begin(115200);
  
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
}
 
void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    uint timestamp = now.Unix64Time();
    Serial.println(timestamp);
    uint passedSeconds = timestamp - START_DATE;
    uint passedYears = (uint)floor(passedSeconds / 31556952);
    uint passedMonths = (uint)floor(passedSeconds / 2629800 % 12);
    uint passedDays = (uint)floor(passedSeconds / 86400 % 30);
    updateScreen(passedYears, passedMonths, passedDays);
    delay(10000); // ten seconds
}

void updateScreen(uint y, uint m, uint d){
  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Draw white text
  
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(20, 0);
  display.write("17/05/2022\n"); // 1652775083 epoch

  display.setTextSize(2);      // Normal 1:1 pixel scale

  display.setCursor(20, 14);
  char years[20];
  sprintf(years, "%d YRS\n", y);
  display.write(years);

  display.setCursor(20, 32);  
  char months[20];
  sprintf(months,"%d MTHS \n", m);
  display.write(months);

  display.setCursor(20, 48); 
  char days[20];
  sprintf(days, "%d DYS\n", d);
  display.write(days);

  display.display();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
