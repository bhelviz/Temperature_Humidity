#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHTPIN D4          // DHT data pin connected to D4 (GPIO2)
#define DHTTYPE DHT11      // Change to DHT22 if using that sensor

const char* ssid     = "SRD";
const char* password = "5Rd@123456"; // Drs@12345

const char* host = "hpvp.bhel.com"; // only google.com not https://google.com

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 sensor with NodeMCU");
  dht.begin();

  delay(10);

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

void loop() {
  delay(60000);  // Wait a few seconds between readings

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius
  float tempF = dht.readTemperature(true);   // Fahrenheit

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println();
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\t");

  Serial.print(tempF);
  Serial.println(" °F");

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
  
  client.setInsecure(); // this is the magical line that makes everything work
  
  client.setTimeout(5000);
  
  if (!client.connect(host, httpPort)) { //works!
    Serial.println("Connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/wsNodeMCU.php";
  url += "?api_key=c1ef41a0";
  url += "&tf=" + String(tempF,2);
  url += "&tc=" + String(temperature,2);
  url += "&rh=" + String(humidity,2);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  Serial.println("Closing connection");
  client.stop();
  yield();
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
}