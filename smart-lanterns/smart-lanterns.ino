/*
* CREATED BY: ETIENNE NAUDE - eti@naude.dev
              BEN SMITH
*
* The main file to edit is conf.h this is only for extra editing outside the scope of the workshop
*
* Go to http://192.168.4.1/H in your browser to turn the LEDs on or http://192.168.4.1/L to turn them off
*
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "conf.h"
#include "patterns.h"

bool ledOn = false;

WiFiServer server(80);

void setup() {

  Serial.begin(BAUD_RATE);

  setupLeds();

  Serial.println();
  Serial.println("Configuring access point...");

  if (!WiFi.softAP(WIFI_SSID)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LEDs.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LEDs.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          ledOn = true;
        }
        if (currentLine.endsWith("GET /L")) {
          ledOn = false;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

    if(ledOn){
    drawPattern();
  } else {
    clearLeds();
  }
}


bool onPowerState(const String &deviceId, bool &state) {
  ledOn = state;
  return true;
}

bool onBrightness(const String& deviceId, int& brightness) {
    FastLED.setBrightness(brightness);
    return true;
}

bool onAdjustBrightness(const String& deviceId, int& brightnessDelta) {
    FastLED.setBrightness(FastLED.getBrightness() + brightnessDelta);
    return true;
}
