/*
  Web client

 This sketch connects to a website through a GSM shield. Specifically,
 this example downloads the URL "http://www.arduino.cc/asciilogo.txt" and
 prints it to the Serial monitor.

 Circuit:
 * GSM shield attached to an Arduino
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://www.arduino.cc/en/Tutorial/GSMExamplesWebClient

 */

// libraries
#include <GSM.h>
#include <EEPROMAnything.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN" // replace your GPRS APN
#define GPRS_LOGIN     "login"    // replace with your GPRS login
#define GPRS_PASSWORD  "password" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "lockee.pagekite.me";
char path[] = "/portal/arduino/ping/";
int port = 80; // port 80 is the default for HTTP

char lock_inner_id[17];

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned postingInterval = 2500; // delay between updates, in milliseconds

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  randomSeed(analogRead(0));
  
  char dictionary[] = {'A', 'a', '1', 'B', 'b', '2', 'C', 'c', '3', 'D', 'd', '4', 'E', 'e', '5',
  'F', 'f', '6', 'G', 'g', '7', 'H', 'h', '8', 'I', 'i', '9', 'J', 'j'};
  const byte dexLength = sizeof(dictionary) / sizeof(dictionary[0]);
  
  EEPROM_readAnything(0, lock_inner_id);
  
  if(lock_inner_id[0] == '\0'){
    for (int n = 0; n < 16; n++){
      lock_inner_id[n] = dictionary[random(0, dexLength)];
      lock_inner_id[n + 1] = '\0';
    }
    EEPROM_writeAnything(0, lock_inner_id);
  }

  Serial.print("Arduino Lock ID: ");
  Serial.println(lock_inner_id);

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS("internet", "", "") == GPRS_READY)) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  Serial.println();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Length: 16");
    client.println();
    client.println(lock_inner_id);

    Serial.println("Trimitem: ");
    Serial.print("Lock id: ");
    Serial.print(lock_inner_id);
    Serial.println(" de length: 16");

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
