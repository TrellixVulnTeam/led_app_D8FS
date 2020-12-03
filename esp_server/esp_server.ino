#include <ESP8266WiFi.h>

const char* ssid = "ZTE-GP36AA";        // SSID - router name
const char* password = "pkpsbtccedv9";  // WiFi - password
int ledPin = 4; //LED_BUILTIN
// Auxiliar variables to store the current output state
String ledStatus = "off";
// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

WiFiServer server(80);
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  IPAddress ip(192, 168, 1, 19);            // desired static IP address
  IPAddress gateway(192, 168, 1, 1);        // IP address of the router
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");          // print a message
    String currentLine = "";                // make a String to hold incoming data
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("LED on");
              ledStatus = "off";
              digitalWrite(ledPin, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("LED off");
              ledStatus = "on";
              digitalWrite(ledPin, LOW);
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>LED osvetlenie</h1>");
            
            // Display current state, and ON/OFF buttons for LEDs 
            client.println("<p>LED - chodnik " + ledStatus + "</p>");
            // If the ledStatus is off, it displays the ON button       
            if (ledStatus=="off") {
              client.println("<p><a href=\"/5/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
//            // Display current state, and ON/OFF buttons for GPIO 4  
//            client.println("<p>GPIO 4 - State " + output4State + "</p>");
//            // If the output4State is off, it displays the ON button       
//            if (output4State=="off") {
//              client.println("<p><a href=\"/4/on\"><button class=\"button\">OFF</button></a></p>");
//            } else {
//              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">ON</button></a></p>");
//            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
//  // Wait until the client sends some data
//  Serial.println("new client");
//  while (!client.available()) {
//    delay(1);
//  }
//  // Read the first line of the request
//  String request = client.readStringUntil('\r');
//  Serial.println(request);
//  client.flush();
//  // Match the request
//  int value = LOW;
//  if (request.indexOf("/LED=ON") != -1) {
//    digitalWrite(ledPin, LOW);
//    value = HIGH;
//  }
//  if (request.indexOf("/LED=OFF") != -1) {
//    digitalWrite(ledPin, HIGH);
//    value = LOW;
//  }
//  // Set ledPin according to the request
//  //digitalWrite(ledPin, value);
//  // Return the response
//  client.println("HTTP/1.1 200 OK");
//  client.println("Content-Type: text/html");
//  client.println(""); // do not forget this one
//  client.println("<!DOCTYPE HTML>");
//  client.println("<html>");
//  client.println("<style>html{font-familt: Arial, Helvetica, sans-serif;} h1{text-align: center;font-size: 20vw;} .block {border-radius: 30px;font-size: 20vw;display: block;width: 100%;border: none;background-color: #2196F3;padding: 100px;cursor: pointer;text-align: center;} a{text-decoration: none;}</style>");
//  client.print("<h1>Svetlo ");
//  if (value == LOW) {
//    client.print("je zhasnute!</h1>");
//  } else {
//    client.print("svieti!</h1>");
//  }
//  // HTML for buttons to work LED
//  client.println("<br><br>");
//  if (value == LOW) {
//    client.println("<a href=\"/LED=ON\"\"><button type='button' class='block'>Zapnut </button></a><br />");
//  } else {
//    client.println("<a href=\"/LED=OFF\"\"><button type='button' class='block'>Vypnut </button></a>");
//  }
//  client.println("</html>");
//  delay(1);
//  Serial.println("Client disconnected");
//  Serial.println("");
//}