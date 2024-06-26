
#include <ESP8266WiFi.h>
#include <espnow.h>


uint8_t broadcastAddress[] = {0xE8,0xDB,0x84,0xC4,0xCD,0x08};  // REPLACE WITH THE MAC Address of your receiver 


#define but1 5
#define but2 4
#define but3 0
#define but4 2
  
#define led 15 

int value;

// Define variables to store incoming readings

int incomingval;

// Updates  readings every .4 seconds
const long interval = 40; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int val;
   
} struct_message;

// Create a struct_message called Readings to hold sensor readings
struct_message Readings;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  
  incomingval = incomingReadings.val;
  if(incomingval==1){
  digitalWrite(led,HIGH);
  }
  else  if(incomingval==0){ digitalWrite(led,LOW); }
 
  Serial.print("Incoming Value      :  ");
              
  Serial.print(incomingval);
 
}

void sendReadings(){

   if(digitalRead(but1)==0)
  {
    value = 1;
  }
  else if(digitalRead(but2)==0)
  {
    value = 2;
  }
  else if(digitalRead(but3)==0)
  {
    value = 3;
  }
  else if(digitalRead(but4)==0)
  {
    value = 4;
  }
  
 
  else if (digitalRead(but1)==1 || digitalRead(but2) ==1 || digitalRead(but3)==1 || digitalRead(but4) ==1 ){
    value = 0;
  }
  
  Serial.print("\t");
  Serial.print(" Sending Value      :  ");
  Serial.println(value);
//Serial.println(value1);
  
}


 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
int val=0;
int value=0;
pinMode(but1,INPUT_PULLUP);
pinMode(but2,INPUT_PULLUP);
pinMode(but3,INPUT_PULLUP);
pinMode(but4,INPUT_PULLUP);

pinMode(led,OUTPUT); 
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  //esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    //Get DHT readings
    sendReadings();

    //Set values to send
    Readings.val = value;
 
   // Sending Sensor Value

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &Readings, sizeof(Readings));

    
  
}}
