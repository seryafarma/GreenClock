//---------------------------------------------------------------------------------------------------------------------
// main.cpp
// 12 hour NTP clock led.
// Style: Procedural
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <ezTime.h>

//---------------------------------------------------------------------------------------------------------------------
// Local Includes
//---------------------------------------------------------------------------------------------------------------------
#include "auth.hpp"

//---------------------------------------------------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------------------------------------------------

#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES   3
#define CS_PIN        D8
#define TIMEZONE      "Europe/Amsterdam"
//---------------------------------------------------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------------------------------------------------
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
Timezone amst;
String clock_to_show;

//---------------------------------------------------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------------------------------------------------
void connect_wifi();
void led_blink();
void update_time();

//---------------------------------------------------------------------------------------------------------------------
// Setup and Loop
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);
    P.begin();
    delay(1500);
    connect_wifi();

    amst.setLocation(TIMEZONE);
    waitForSync();

    clock_to_show = amst.dateTime("g:i");

    pinMode(LED_BUILTIN, OUTPUT);
}

//---------------------------------------------------------------------------------------------------------------------
void loop()
{
    update_time();
    led_blink();

    if (P.displayAnimate())
    {
        // to make this fit, remove the trailing zero.
        // set to 12 hours clock only.
        P.displayText(clock_to_show.c_str(), PA_RIGHT, 50, 15 * 1000, PA_SCROLL_LEFT, PA_DISSOLVE);
    }
    delay(100);
}

//---------------------------------------------------------------------------------------------------------------------
// Functions Definition
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
void connect_wifi()
{
    WiFi.begin(Authentication::WIFI_SSID, Authentication::WIFI_PASSWORD);
    Serial.print("Connecting to ");
    Serial.println(Authentication::WIFI_SSID);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.println(WiFi.localIP());
    Serial.println();

    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

//---------------------------------------------------------------------------------------------------------------------
void led_blink()
{
    static const uint32_t LED_TIME_PERIOD = 1000UL;
    static uint32_t led_previous_gather_millis = 0;
    static bool led_high = true;
    uint32_t led_current_gather_millis = millis();

    // A simple timer actually for a minute...
    if (led_current_gather_millis - led_previous_gather_millis > LED_TIME_PERIOD)
    {
        // Save the last time tick.
        led_previous_gather_millis = led_current_gather_millis;
        digitalWrite(LED_BUILTIN, led_high ? HIGH : LOW);
        led_high = !led_high;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void update_time()
{
    static const uint32_t TIME_PERIOD = 30UL * 1000UL;
    static uint32_t previous_gather_millis = 0;
    uint32_t current_gather_millis = millis();

    // A simple timer actually for a minute...
    if (current_gather_millis - previous_gather_millis > TIME_PERIOD)
    {
        // Save the last time tick.
        previous_gather_millis = current_gather_millis;
        clock_to_show = amst.dateTime("g:i");
    }
}