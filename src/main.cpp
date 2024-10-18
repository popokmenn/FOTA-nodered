/* ESP32 Proximity Sensor Data Sending example */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h> // Include the Ping library

const char *ssid = "Nopleee iPhone";
const char *password = "password";
const char *node_red_url = "http://172.20.10.4:1880/proximity"; // Corrected Node-RED endpoint
const char *remote_ip = "172.20.10.4";

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing Proximity Sensor...");
  pinMode(35, INPUT);

  // Display scanned Wi-Fi list
  Serial.println("Scanning for Wi-Fi networks...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete. Available networks:");

  WiFi.begin(ssid, password);
  for (int i = 0; i < n; ++i)
  {
    Serial.printf("%d: %s (%d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    // Ping the Node-RED server
    if (Ping.ping(remote_ip)) // Ping the IP address
    {
      Serial.println("Ping successful!");

      HTTPClient http;
      http.begin(node_red_url);
      http.addHeader("Content-Type", "application/json"); // Updated header addition
      http.setTimeout(5000);                              // Set timeout to 5000 milliseconds (5 seconds)

      int sensorValue = digitalRead(35);

      char buffpayload[40];
      snprintf(buffpayload, 40, "{\"proximity\":%d}", sensorValue);
      Serial.println("Payload: " + String(buffpayload)); // Debugging: print the payload

      int httpResponseCode = http.POST((String)buffpayload);
      Serial.printf("HTTP Response Code: %d\n", httpResponseCode); // Debugging: print the response code

      if (httpResponseCode > 0)
      {
        Serial.printf("Data sent to Node-RED, response: %d\n", httpResponseCode);
      }
      else
      {
        Serial.printf("Error sending data to Node-RED: %s\n", http.errorToString(httpResponseCode).c_str());
      }
      http.end();
    }
    else
    {
      Serial.println("Ping failed!");
    }
  }
  else
  {
    Serial.println("WiFi not connected");
  }

  delay(1000);
}
