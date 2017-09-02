/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */

#define RELAY_PIN 4
#define LED_PIN 16

#include <ESP8266WiFi.h>

/* Set these to your desired credentials. */
const char *ssid = "ESP_Fi";
const char *password = "123456789";

int requestCount = 0;

WiFiServer server(8000);

void ProcessClientRequest(WiFiClient client)
{
  String receivedString = "";
  delay(200);
  if (!client.available()) return;
  while(client.available())
  {
    char receivedChar = client.read();
    receivedString += receivedChar;
  }

  //  int firstIndex = receivedString.indexOf('$');
  //  int lastIndex = receivedString.indexOf(';');
  //  if (firstIndex < 0 || lastIndex < 0 || (lastIndex - firstIndex != 3)) return;
  //  int switchState = receivedString.substring(firstIndex, lastIndex + 1).substring(2, 3).toInt();
  int index = receivedString.indexOf("switch=");
  if(index < 0) return;
  int switchState = receivedString[index + 7] - 48; // "switch=".length() == 7
  digitalWrite(LED_BUILTIN, switchState == 1 ? HIGH : LOW);
  digitalWrite(LED_PIN, switchState == 1 ? HIGH : LOW);
  digitalWrite(RELAY_PIN, switchState == 1 ? LOW : HIGH);
  Serial.println(switchState);

  switchState = switchState == 1 ? 0 : 1;
  String webPage = "<a href=\".?switch=" + String(switchState) + "\">";
  webPage += switchState == 1 ? "OFF" : "ON";
  webPage += "</a>";
  
  client.flush();
  client.print(webPage);
  delay(1);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(RELAY_PIN, LOW);
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);
  delay(1000);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
  //	server.on("/", handleRoot);
  delay(1000);
	server.begin();
	Serial.println("HTTP server started");
}

void loop() {
  //	server.handleClient();
  WiFiClient client = server.available();
  delay(100);
  if (!client) return;
  Serial.println("New Client Connected");
  ProcessClientRequest(client);
  delay(100);
  client.stop();
}
