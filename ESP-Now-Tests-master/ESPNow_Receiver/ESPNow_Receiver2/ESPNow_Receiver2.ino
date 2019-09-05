// EspnowSlave.ino
// Oroginal code is here : https://forum.arduino.cc/index.php?topic=551310.0 
// a minimal program derived from
//          https://github.com/HarringayMakerSpace/ESP-Now

// This is the program that receives the data. (The Slave)



#include <ESP8266WiFi.h>

extern "C" {
    #include <espnow.h>
    #include <user_interface.h>
}

// it seems that the mac address needs to be set before setup() is called
//      and the inclusion of user_interface.h facilitates that
//      presumably there is a hidden call to the function initVariant()

/* Set a private Mac Address
 *  http://serverfault.com/questions/40712/what-range-of-mac-addresses-can-i-safely-use-for-my-virtual-machines
 * Note: by setting a specific MAC you can replace this slave ESP8266 device with a new one
 * and the new slave will still pick up the data from controllers which use that MAC
 */
uint8_t mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};


void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
}

//==============

#define WIFI_CHANNEL 4

    // must match the controller struct
struct __attribute__((packed)) DataStruct {
    char text[32];
    float temperature;
    float humidity;
    float pressure;
    unsigned int time;
};

DataStruct myData;
//================================Probe Request defines starts==========
#define CONTROLLER_ID 01

#define SWITCH 12
#define DEVICE_BUTTON 0

WiFiEventHandler probeRequestPrintHandler;
volatile boolean buttonPressed;
//================================Probe Request defines ends==========

void setup() {
    Serial.begin(115200); Serial.println();
    
  //================================Probe Request setup starts==========
    pinMode(SWITCH, OUTPUT);
    pinMode(DEVICE_BUTTON, INPUT);
   attachInterrupt(DEVICE_BUTTON, buttonPress, HIGH); 
    WiFi.persistent(false);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Controller", "<notused>", 6, 0, 0);
    probeRequestPrintHandler = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequest);
  
  Serial.println("Starting Probe Request receiver");  
 //================================Probe Request setup ends==========  
 
 
 //================================ESPNow setup starts==========    
    Serial.println("Starting Espnow receiver");
   
     
    Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
    Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());
    
    
    if (esp_now_init()!=0) {
        Serial.println("*** ESP_Now init failed");
        while(true) {};
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

    esp_now_register_recv_cb(receiveCallBackFunction);
//================================EspNow setup ends==========  
    

    Serial.println("End of setup - waiting for Probe Requests and ESPNow messages");
 
}



void loop() {
  if (buttonPressed) {
    digitalWrite(SWITCH, !digitalRead(SWITCH));
    Serial.print("Button pressed, swiched: "); Serial.println(digitalRead(SWITCH) ? "on" : "off");
    delay(250); // to debounce button
    buttonPressed = false;
   
  }
 }

ICACHE_RAM_ATTR void buttonPress() {
  buttonPressed = true;
}

//================================ESPNow function starts==========
    


void receiveCallBackFunction(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
    memcpy(&myData, incomingData, sizeof(myData));
    
    Serial.print(myData.text);
    Serial.print("  Time : ");
    Serial.print(myData.time);
     Serial.print("  device : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[5], DEC);
    }
    Serial.print("  Battery : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[3], DEC);
    }
    Serial.print("  Temperature : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[0], DEC);
    }
    Serial.print("  Humidity : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[1], DEC);
    }
    Serial.print("  Pressure : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[2], DEC);
    }

    Serial.print("  Light : ");
    for (byte n = 0; n < 1; n++) {
    Serial.print (senderMac[4], DEC);
    } 
  /*Serial.print("  MsgLen ");
    Serial.print(len);
    Serial.print("  Text ");
    Serial.print(myData.text);
    */
   
    Serial.println();
}

//================================ESPNow function ends==========

//================================Probe Request function starts==========
   
void onProbeRequest(const WiFiEventSoftAPModeProbeRequestReceived& dataReceived) {
//  if (dataReceived.mac[5] != CONTROLLER_ID) return;
  
   Serial.print("Probe Request:- ");
   
  //if (dataReceived.mac[5] == 0x01) {
   // if (WiFi.hostname() == WiFi.hostname()) {
    //digitalWrite(SWITCH, HIGH);
    Serial.print(" Controller:  ");
    Serial.print(dataReceived.mac[5],DEC);
    Serial.print(" Temperature:  ");
    Serial.print(dataReceived.mac[0],DEC);
    Serial.print(" Humidity:  ");
    Serial.print(dataReceived.mac[1],DEC);
    Serial.print(" Pressure:  ");
    Serial.print(dataReceived.mac[2],DEC);
    Serial.print(" Light:  ");
    Serial.println(dataReceived.mac[3],DEC);
 //   } 
  // else {
   // digitalWrite(SWITCH, LOW);
  //  Serial.println("Data Error");
 // }

 
}

//================================Probe Request function ends==========
