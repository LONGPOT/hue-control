#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

const int ledPin = 9;       // pin that the LED is attached to
int analogValue = 0;  
int analogValue2 = 0;// value read from the pot
int brightness = 0; 
int hue = 60000;// PWM pin that the LED is on.
int sat =0;
long lastRequest= 10000;
int requestDelay= 500;

int status = WL_IDLE_STATUS;      // the Wifi radio's status
char hueHubIP[] = "172.22.151.181";  // IP address of the HUE bridge
String hueUserName = "D2cKhR2unkKIseRZdtN0UEzNLwpCFv84yuTQc-Iz"; // hue bridge username

// make a wifi instance and a HttpClient instance:
WiFiClient wifi;
HttpClient httpClient = HttpClient(wifi, hueHubIP);
// change the values of these two in the arduino_serets.h file:
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect.

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass); 

    // declare the led pin as an output:
    pinMode(ledPin, OUTPUT);
    sendRequestReset(1, "on", "true");   // turn light on
    delay(1500);                    // wait seconds
    sendRequestReset(1, "on", "false");  // turn light off
    sendRequestReset(1, "on", "true");   // turn light on
    sendRequestReset(1, "on", "false");  // turn light off
    sendRequestReset(1, "on", "true");   // turn light on

    sendRequest(1, "hue", hue);   // turn light on
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network IP = ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void loop() {
  analogValue = analogRead(A0);    // read the pot value
  brightness = map(analogValue,0, 100, 0, 254);       //divide by 4 to fit in a byte
  //analogWrite(ledPin, brightness);   // PWM the LED with the brightness value
  Serial.println(analogValue);
  Serial.println(brightness);// print the brightness value back to the serial monitor'
  if (millis() - lastRequest > requestDelay) {
  sendRequest(1, "bri", brightness);   // PWM the hue bulb with the brightness value
  }
  analogValue2 = analogRead(A1);    // read the pot value
  hue = map(analogValue2,0, 100, 0, 65535);       //divide by 4 to fit in a byte
  //analogWrite(ledPin, hue);   // PWM the LED with the brightness value
  Serial.println("hue"+hue);        // print the brightness value back to the serial monitor'
  if (millis() - lastRequest > requestDelay) {
  sendRequest(1, "hue", hue);   // turn light on
  }
}

//The bulb blink to confirmed the connection
void sendRequestReset(int light, String cmd, String value) {
  // make a String for the HTTP request path:
  String request = "/api/" + hueUserName;
  request += "/lights/";
  request += light;
  request += "/state/";

  String contentType = "application/json";

  // make a string for the JSON command:
  String hueCmd = "{\"" + cmd;
  hueCmd += "\":";
  hueCmd += value;
  hueCmd += "}";
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  Serial.print("JSON command to server: ");

  // make the PUT request to the hub:
  httpClient.put(request, contentType, hueCmd);
  
   //read the status code and body of the response
   int statusCode = httpClient.responseStatusCode();
   String response = httpClient.responseBody();
//   while(httpClient.connected()){
//    if(httpClient.available()){
//      Serial.println(httpClient.read());
//      }
//  }

  Serial.println(hueCmd);
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Server response: ");
  Serial.println(response);
  Serial.println();
}

void sendRequest(int light, String cmd, int  value) {
  // make a String for the HTTP request path:
  String request = "/api/" + hueUserName;
  request += "/lights/";
  request += light;
  request += "/state/";

  String contentType = "application/json";

  // make a string for the JSON command:
  String hueCmd = "{\"" + cmd;
  hueCmd += "\":";
  hueCmd += value;
  hueCmd += "}";
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  Serial.print("JSON command to server: ");

  // make the PUT request to the hub:
  httpClient.put(request, contentType, hueCmd);

  while(httpClient.connected()){
    if(httpClient.available()){
      Serial.println(httpClient.read());
      }
  }
  // read the status code and body of the response
  //  int statusCode = httpClient.responseStatusCode();
  //  String response = httpClient.responseBody();

  Serial.println(hueCmd);
//  Serial.print("Status code from server: ");
//  Serial.println(statusCode);
//  Serial.print("Server response: ");
//  Serial.println(response);
  Serial.println();
}
