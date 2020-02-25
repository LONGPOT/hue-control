#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"
//#include <ColorConverter.h>

const int ledPin = 9;       // pin that the LED is attached to
int analogValue = 0;
int analogValue2 = 0;// value read from the pot
int analogValue3 = 0;
int bri = 0;
int hue = 0;// PWM pin that the LED is on.
int sat = 0;

int noise = 5;
// send interval, in ms:
int sendInterval = 1000;
// previous reading:
int lastBriReading = 0;
int lastSatReading = 0;
int lastHueReading = 0;
// last time you sent a reading, in ms:
long lastBriSendTime = 0;
long lastSatSendTime = 0;
long lastHueSendTime = 0;
int counter = 0;
int oldBri;
int oldSat;
int oldHue;
String b;
String h;
String s;

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
    //pinMode(ledPin, OUTPUT);
    sendRequestreset(1, "on", "true");   // turn light on
    delay(500);                    // wait seconds
    sendRequestreset(1, "on", "false");  // turn light off
    delay(500);
    sendRequestreset(1, "on", "true");   // turn light on
    delay(500);
    sendRequestreset(1, "on", "false");  // turn light off
    delay(500);
    sendRequestreset(1, "on", "true");   // turn light on
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network IP = ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void loop() {
  // if the send interval has passed:
  if (millis() - lastBriSendTime > sendInterval) {
    // read the sensor:
    analogValue = analogRead(A0);    // read the pot value
  bri= map(analogValue, 0, 1024, 0, 254);
  //analogWrite(ledPin, brightness);   // PWM the LED with the brightness value
  //  Serial.print("analogValue1: ");
  //  Serial.println(analogValue);
    // if the difference between current and last > noise:
    if (abs(analogValue - lastBriReading) > noise) {
      // send it:
      b = String (bri);
    //sendRequest(1, "bri", b);
    Serial.print("bri:");
    //Serial.println(bri);
    Serial.println(b);
    }
    // save current reading for comparison next time:
    lastBriReading = analogValue;
    // take a timestamp of when you sent:
    lastBriSendTime = millis();
  }

  counter = counter + 1;
  Serial.print("counter:");
  Serial.println(counter);
  //oldBrightness = brightness;

  // if the send interval has passed:
  if (millis() - lastHueSendTime > sendInterval) {
    // read the sensor:
   analogValue2 = analogRead(A1);    // read the pot value
  hue = map(analogValue2, 0, 1024, 0, 65535);      //divide by 4 to fit in a byte
  //Serial.print("analogValue2: ");
  //Serial.println(analogValue2);
    // if the difference between current and last > noise:
    if (abs(analogValue2 - lastHueReading) > noise) {
      h = String(hue);
    //sendRequest(1, "hue", h);
    Serial.print("hue:");
    Serial.println(hue);
    }
    // save current reading for comparison next time:
    lastHueReading = analogValue2;
    // take a timestamp of when you sent:
    lastHueSendTime = millis();
  }

if (millis() - lastSatSendTime > sendInterval) {
    // read the sensor:
   analogValue3 = analogRead(A2);    // read the pot value
  sat = map(analogValue3, 0, 1024, 0, 244);
  Serial.print("sat:");
    Serial.println(sat);
    // if the difference between current and last > noise:
    if (abs(analogValue3 - lastSatReading) > noise) {
      s = String(sat);
    //sendRequest(1, "sat", s);   // turn light on
    Serial.print("hue:");
    Serial.println(hue);
    }
    // save current reading for comparison next time:
    lastSatReading = analogValue3;
    // take a timestamp of when you sent:
    lastSatSendTime = millis();
  }
    
    
 
   sendrequest(1, b, h, s);


  //convert the color to rgb
  //int mapHue = map(hue,0,65535,0,90);
  //int NEOhue = mapHue*5;
  //RGBColor color = converter.HSItoRGB(h,s,b);
}

//The bulb blink to confirmed the connection

void sendrequest(int light, String briValue, String hueValue, String satValue) {
  // make a String for the HTTP request path:
  String request = "/api/" + hueUserName;
  request += "/lights/";
  request += light;
  request += "/state/";
  String cmd = String("{\"bri\":") + briValue
           + String(",\"hue\":") + hueValue + String(",\"sat\":")
           + satValue + String("}");



  String contentType = "application/json";
//
//  // make a string for the JSON command:
//  String cmd = "{\"" + hueCmd;
//  cmd += "\":";
//  cmd += hueValue;
//  cmd +=",\""+briCmd;
//  cmd += briValue;
//  cmd +=",\""+satCmd;
//  cmd += satValue;
//  cmd += "}";

  
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  Serial.print("JSON command to server: ");

  // make the PUT request to the hub:
  httpClient.put(request, contentType, cmd);

  //  while(httpClient.connected()){
  //    if(httpClient.available()){
  //      Serial.println(httpClient.read());
  //      }
  //  }
  // why bulb does not answer my call after I use this line?
  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  Serial.println(cmd);
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Server response: ");
  Serial.println(response);
  Serial.println();
}


void sendRequestreset(int light, String cmd, String value) {
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
