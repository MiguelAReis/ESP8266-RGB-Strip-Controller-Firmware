#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h> 
#include "LookupTable.h"

#define RED 13
#define BLUE 12
#define GREEN 14
#define ENABLE 5

ESP8266WebServer server(80);

#ifndef STASSID
#define STASSID "xxxxxxxxxxxxxxx"
#define STAPSK  "xxxxxxxxxxxxxxx"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

int mode[]={0,0,0,0,0,0,0,0,0};

bool firstTime =true;
float r, g, b;
bool dir;
float rstep,gstep,bstep;
int i,y;
float brightness;

void setColor(int red, int green, int blue){
  analogWrite(RED,red);
  analogWrite(GREEN,green);
  analogWrite(BLUE,blue);
}

void handleMode() {                       
  mode[0]=server.arg("mode").toInt();
  mode[1]=server.arg("arg0").toInt();
  mode[2]=server.arg("arg1").toInt();
  mode[3]=server.arg("arg2").toInt();
  mode[4]=server.arg("arg3").toInt();
  mode[5]=server.arg("arg4").toInt();
  mode[6]=server.arg("arg5").toInt();
  mode[7]=server.arg("arg6").toInt();
  mode[8]=server.arg("arg7").toInt();
  firstTime=true;
  server.send(200);
  return;
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname("RGB_Quarto_Sara");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });


  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(5,1);
  
  server.on("/mode", HTTP_POST, handleMode);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  switch(mode[0]){
    case 0: //Turn Off all leds
    default:
      if(firstTime){
        digitalWrite(ENABLE,0);
        firstTime=false;
      }
      break;
    case 1: //Solid Color : R G B
      if(firstTime){
        digitalWrite(ENABLE,1);
        setColor(mode[1],mode[2], mode[3]);
      }
      break;
    case 2: //Transition between 2 colors : R0 G0 B0 R1 G1 B1 Speed
      if(firstTime){     
        dir=true;
        r=mode[1];
        g=mode[2];
        b=mode[3];
        firstTime=false;   
        digitalWrite(ENABLE,1);
        rstep=(float)abs((mode[1]-mode[4]))/1024;
        gstep=(float)abs((mode[2]-mode[5]))/1024;
        bstep=(float)abs((mode[3]-mode[6]))/1024;
      }
      if( r != (dir?mode[4]:mode[1]) || g != (dir?mode[5]:mode[2]) || b != (dir?mode[6]:mode[3]) ) {
        if ( r < (dir?mode[4]:mode[1]) ){
          r+=rstep;
        }else if ( r > (dir?mode[4]:mode[1]) ) r-=rstep;
    
        if ( g < (dir?mode[5]:mode[2]) ){
          g+=gstep;
        }else if ( g > (dir?mode[5]:mode[2]) ) g-=gstep;
    
        if ( b < (dir?mode[6]:mode[3] )){
          b+=bstep;
        }else if ( b > (dir?mode[6]:mode[3]) ) b-=bstep;
        
        setColor(r,g,b);
        delayMicroseconds(mode[7]);
      }
      else{
        dir=!dir;
      }
      break;
    case 3:  //Rainbow: Speed Brightness
      if(firstTime){
        brightness =(float)mode[8]/100;     
        r=(float)rainbow[0][0]*brightness;
        g=(float)rainbow[0][1]*brightness;
        b=(float)rainbow[0][2]*brightness;
        i=1;
        y=0;
        
        firstTime=false;   
        digitalWrite(ENABLE,1);
      }
      if(y<1024) {
        if ( r < (float)rainbow[i][0]*brightness ){
          r+=brightness;
        }else if ( r > (float)rainbow[i][0]*brightness ) r-=brightness;
    
        if ( g < (float)rainbow[i][1]*brightness ){
          g+=brightness;
        }else if ( g > (float)rainbow[i][1]*brightness ) g-=brightness;
    
        if ( b < (float)rainbow[i][2]*brightness){
          b+=brightness;
        }else if ( b > (float)rainbow[i][2]*brightness ) b-=brightness;
        
        setColor(r,g,b);
        delayMicroseconds(mode[7]);
        y++;
      }else{
        y=0;
        i>=5?i=0:i++;
      }
      break;
    case 4: // Disco : Speed Brightness
      if(firstTime){     
        i=1;
        y=0;
        brightness =(float)mode[8]/100;
        firstTime=false;   
        digitalWrite(ENABLE,1);
      }
      if(y<=1000){
        setColor(rainbow[i][0]*brightness,rainbow[i][1]*brightness,rainbow[i][2]*brightness);
        y++;
        delayMicroseconds(mode[7]);
      }else{
        y=0;
        i>=5?i=0:i++;
      }
      break;

      
  }
}
