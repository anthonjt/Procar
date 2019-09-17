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
char url[800];
int httpcode;

int k64fval = 613;
float espval = 123;
char IPaddress[15] = "71.115.155.115";

byte incomingByte;

HTTPClient client1;

void setup(void)
{ 
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
  snprintf(urlbegin, sizeof(urlbegin), "%s%susername=%s&password=%s&dbname=%s&tablename=%s", ip, phpfile, USER, PASSWORD, DB, tablename);
}

void loop() 
{
    //if (Serial.available() > 0) {
      // read the incoming byte:
    //incomingByte = Serial.read();
    snprintf(datastring, sizeof(datastring), "&k64fval=%d&espval=%f&IPaddress=%s", k64fval, espval, IPaddress);
    snprintf(url, sizeof(url), "%s%s", urlbegin, datastring);
    client1.begin(url);
    httpcode = client1.GET();
    Serial.println(url);
    delay(5000);
    // say what you got:
    //Serial.println(incomingByte, DEC);
  //}
}
