#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

const char* ssid     = "home2";
const char* password = "helloworld";

 
const String FirmwareVer={"0.7"}; 
#define URL_fw_Version "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/master/esp32_bin/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/master/esp32_bin/fw.bin"

//#define URL_fw_Version "http://cade-make.000webhostapp.com/version.txt"
//#define URL_fw_Bin "http://cade-make.000webhostapp.com/firmware.bin"
HTTPClient http;
 
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n"
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
"+OkuE6N36B9K\n"
"-----END CERTIFICATE-----\n";

void firmwareUpdate(void)
{
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    httpUpdate.setLedPin(LED_BUILTIN, LOW);   
    t_httpUpdate_return ret = httpUpdate.update(client,URL_fw_Bin);
    
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }   
}
int FirmwareVersionCheck(void)
{
  http.begin(URL_fw_Version,"CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33");     // check version URL
  http.addHeader("Cache-Control", "no-cache");
  delay(100);
  int httpCode = http.GET();            // get data from version file
  
  delay(100);
  String payload;
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload = http.getString();  // save received version
  }
  else
  {
    Serial.print("error in downloading version file:");
    Serial.println(httpCode);
  }  
  http.end();
  
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload.trim();
    if(payload.equals(FirmwareVer) )
    {   
       Serial.printf("\nDevice already on latest firmware version:%s\n",FirmwareVer); 
       return 0;
    }
    else
    {
      Serial.printf("version file payload:\"%s\"");
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  }
}
      
    

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousMillis_2 = 0; 
const long interval = 30000;
const long mini_interval=500;
 void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      if(FirmwareVersionCheck())
      {
        firmwareUpdate();
      }
    }
    if ((currentMillis - previousMillis_2) >= mini_interval) 
    {
      previousMillis_2=currentMillis;
      Serial.print("Y");
      if(millis()%1000==0)
      {
        Serial.println();
      }
    }
 }

struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button button_boot = {0, 0, false};
/*void IRAM_ATTR isr(void* arg) {
    Button* s = static_cast<Button*>(arg);
    s->numberKeyPresses += 1;
    s->pressed = true;
}*/

void IRAM_ATTR isr() {
    button_boot.numberKeyPresses += 1;
    button_boot.pressed = true;
}
void connect_wifi();

void setup()
{
  pinMode(button_boot.PIN, INPUT);
  attachInterrupt(button_boot.PIN, isr, RISING);
  Serial.begin(115200);
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
  pinMode(LED_BUILTIN, OUTPUT);
  connect_wifi();
}
void loop()
{
  if (button_boot.pressed) {//to connect wifi via Android esp touch app 
     Serial.println("Firmware update Starting.."); 
     firmwareUpdate();       
     button_boot.pressed = false;
  }
  repeatedCall(); 
}



void connect_wifi()
{  
  Serial.println("Waiting for WiFi");
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
