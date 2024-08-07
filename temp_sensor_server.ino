/*
  WAP code partially sourced from WiFiAccessPoint.ino by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <DHT.h>

// Set these to your desired credentials.
const char *ssid = "Smart Home";
const char *password = "securepassword";

NetworkServer server(80);
DHT dht(26, DHT22);

void setup() {
  Serial.begin(115200);
  Serial.println();
  dht.begin();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.printf("<html>\
            <head>\
              <meta http-equiv='refresh' content='4'/>\
              <meta name='viewport' content='width=device-width, initial-scale=1'>\
              <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
              <title>ESP32 DHT Server</title>\
              <style>\
                html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
                h2 { font-size: 3.0rem; }\
                p { font-size: 3.0rem; }\
                .units { font-size: 1.2rem; }\
                .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
              </style>\
            </head>\
            <body>\
              <h2>ESP32 DHT Server!</h2>\
              <p>\
                <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
                <span class='dht-labels'>Temperature</span>\
                <span>%.2f</span>\
                <sup class='units'>&deg;C</sup>\
              </p>\
              <p>\
                <i class='fas fa-tint' style='color:#00add6;'></i>\
                 <span class='dht-labels'>Humidity</span>\
                <span>%.2f</span>\
                <sup class='units'>&percnt;</sup>\
              </p>\
            </body>\
            </html>",
           readDHTTemperature(), readDHTHumidity()
          );

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        /* if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        } */
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.println(h);
    return h;
  }
}
