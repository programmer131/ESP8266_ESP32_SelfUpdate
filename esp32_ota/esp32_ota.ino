
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "programmer5"
#define AIO_KEY         ""

WiFiClient client_mqtt;
Adafruit_MQTT_Client mqtt(&client_mqtt, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe ota_button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/update-fw");
Adafruit_MQTT_Publish device_log = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/device-log");
void MQTT_connect();
 
const String FirmwareVer={"0.2"}; 
#define URL_fw_Version "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/esp32_bin/master/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/esp32_bin/master/esp32.bin"

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
 
void FirmwareUpdate()
{
  http.begin(URL_fw_Version,"CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33");     // check version URL
  delay(100);
  int httpCode = http.GET();            // get data from version file
  delay(100);
  String payload;
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload = http.getString();  // save received version
    Serial.print("payload:");
    Serial.println(payload );
  }
  else
  {
    Serial.print("error in downloading version file:");
    Serial.println(httpCode);
  }  
  http.end();
  
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    if(payload.equals(FirmwareVer) )
    {   
       Serial.println("Device already on latest firmware version"); 
    }
    else
    {
      Serial.print("payload:");
      Serial.println(payload );
      Serial.print("FirmwareVer:");
      Serial.println(FirmwareVer );
      Serial.println("New firmware detected");
      WiFiClientSecure client;
      client.setCACert(rootCACertificate);
      // The line below is optional. It can be used to blink the LED on the board during flashing
      // The LED will be on during download of one buffer of data from the network. The LED will
      // be off during writing that buffer to flash
      // On a good connection the LED should flash regularly. On a bad connection the LED will be
      // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
      // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
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
 }  
}

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 40000;

 void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      FirmwareUpdate();
    }
 }

struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button button_boot = {0, 0, false};
void IRAM_ATTR isr(void* arg) {
    Button* s = static_cast<Button*>(arg);
    s->numberKeyPresses += 1;
    s->pressed = true;
}

void IRAM_ATTR isr() {
    button_boot.numberKeyPresses += 1;
    button_boot.pressed = true;
}
void connect_wifi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
void setup()
{
  pinMode(button_boot.PIN, INPUT);
  attachInterruptArg(button_boot.PIN, isr, &button_boot, RISING);
  Serial.begin(115200);
  WiFi.begin();
  delay(1000);
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
  pinMode(LED_BUILTIN, OUTPUT);
  mqtt.subscribe(&ota_button);
}
void loop()
{
  if (button_boot.pressed) {//to connect wifi via Android esp touch app  
        connect_wifi();
        button_boot.pressed = false;
  }
  MQTT_connect();
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  repeatedCall();    
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  if ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("mqtt connect failed");
       mqtt.disconnect();
       return;
  }
  Serial.println("MQTT Connected!");
}
