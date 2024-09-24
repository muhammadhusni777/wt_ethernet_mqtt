#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial

// Debug Level from 0 to 4
#define ETHERNET_WEBSERVER_LOGLEVEL       3

#include "esp_task_wdt.h"

// Define the watchdog timeout period (in seconds)
#define WATCHDOG_TIMEOUT 10



#include <WebServer_WT32_ETH01.h>

//WebServer server(80);

// Select the IP address according to your local network
IPAddress myIP(123, 45, 0, 101);
IPAddress myGW(123, 45, 0, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

#include <PubSubClient.h>

// Update these with values suitable for your network.
//const char* mqttServer = "broker.example";        // Broker address
const char* mqttServer = "123.45.0.10";        // Broker address
//const char* mqttServer = "broker.shiftr.io";        // Broker address


//IPAddress mqttServer(172, 16, 0, 2);

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE6 }; 

unsigned long message_time;
unsigned long message_time_prev;


unsigned long time_now;
unsigned long time_elapsed;
unsigned long time_prev;

unsigned long time_send;
unsigned long time_send_prev;
int time_message = 1000;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  

  if (String(topic) == "lamp1") {

   Serial.println(messageTemp.toInt());
   }
   

 messageTemp ="";
}

WiFiClient ethClient;
PubSubClient client(mqttServer, 1883, callback, ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("CLMS_Client")) {
      Serial.println("connected");
      client.subscribe("lamp1");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  } 
}


void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  while (!Serial);

  Serial.print("\nStarting MQTTClient_Basic on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);

  // To be called before ETH.begin()
  WT32_ETH01_onEvent();

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);

  WT32_ETH01_waitForConnect();

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  // Allow the hardware to sort itself out
  delay(1500);
  // Initialize the watchdog timer
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true); // Set timeout and panic
  esp_task_wdt_add(NULL); // Add the current task to the watchdog

  Serial.println("Watchdog Timer Initialized");
}



void loop() {
  time_now = millis();

  time_send = millis() - time_send_prev;
  
  if (!client.connected()) {
    reconnect();
  }

  if (time_send > time_message){
    client.publish("system","on");
    time_send_prev = millis();
  }




  esp_task_wdt_reset();
  client.loop();
}
