  #include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

const char* ssid = "ClarksonUniversity"; //Enter SSID
const char* password = ""; //Enter Password

//Server information
char ip[] = "http://71.115.155.115:304/";
char phpfile[] = "indata/indataesp01.php?";
char USER[] = "jordan";
char PASSWORD[] = "cameron123";
char DB[] = "EE416";
char tablename[] = "test1";
char urlbegin[200];
char datastring[400];
char incomingData[400];
String dstring;
char url[800];
int datapiece[400];
int httpcode;
int i = 0;
byte startByte = '<';
byte stopByte = '>';


int k64fval = 613;
float espval = 123;
char IPaddress[15] = "71.115.155.115";

byte incomingByte;

HTTPClient client1;

void setup(void)
{ 
  Serial.begin(315200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  datapiece[0] = 1;
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
  snprintf(urlbegin, sizeof(urlbegin), "%s%susername=%s&password=%s&dbname=%s&tablename=%s", ip, phpfile, USER, PASSWORD, DB, tablename);
}

void loop() 
{
static char incomingData[400];
static byte index=0;
 if (Serial.available() > 0 ) {
   char inChar = Serial.read();
   
   if (inChar==startByte) { // If start byte is received
     index=0; // then reset buffer and start fresh
   } else if (inChar==stopByte) { // If stop byte is received
     processData(incomingData); // and process the data
     index=0; // this isn't necessary, but helps limit overflow
   } else { // otherwise
     incomingData[index] = inChar; // put the character into our array
     index++; // and move to the next key in the array
   }
 }
}

void processData(char buffer[]) {
    snprintf(datastring, sizeof(datastring), "&k64fval=%s&espval=%f&IPaddress=%s", buffer, espval, IPaddress);
    snprintf(url, sizeof(url), "%s%s", urlbegin, datastring);
    client1.begin(url);
    httpcode = client1.GET();
    Serial.println(url);
}
