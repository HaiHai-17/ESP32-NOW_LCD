#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#define CHANNEL 1

// địa chỉ MAC của Master
uint8_t broadcastAddress[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

typedef struct structure_to_send_messages {
  char str_s_Text[32];
  int int_s_Count;
} structure_to_send_messages;

structure_to_send_messages s_Messages;

typedef struct structure_to_receive_messages {
  char str_r_Text[32];
  int int_r_Count;
} structure_to_receive_messages;

structure_to_receive_messages r_Messages;

int cnt = 0;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t * mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
                                  mac_addr[0], mac_addr[1], mac_addr[2], 
                                  mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("ESP32 Slave.");
  Serial.print("Send message to\t\t: "); Serial.println(macStr);
  Serial.print("Message sending status\t: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t * incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
                                  mac_addr[0], mac_addr[1], mac_addr[2], 
                                  mac_addr[3], mac_addr[4], mac_addr[5]);
  
  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("ESP32 Slave.");
  Serial.print("Receive from\t: "); Serial.println(macStr);
  Serial.print("Bytes received\t: "); Serial.println(len);
  Serial.print("str_r_Text\t: ");
  Serial.println(r_Messages.str_r_Text);
  Serial.print("int_r_Count\t: ");
  Serial.println(r_Messages.int_r_Count);

  cnt = r_Messages.int_r_Count;

  Send_Reply_Message();
}

void Send_Reply_Message() {
  delay(1500);
  
  strcpy(s_Messages.str_s_Text, "Hello ESP32 Master");
  s_Messages.int_s_Count = cnt;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &s_Messages, sizeof(s_Messages));

  if (result != ESP_OK) {
    Serial.println();
    Serial.println("Error sending the data");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  Serial.println("ESP32 Slave.");

  Serial.println();
  Serial.println("-------------");
  Serial.println("Start Setup.");
  
  WiFi.mode(WIFI_STA);

  int cur_WIFIchannel = WiFi.channel();
  
  if (cur_WIFIchannel != CHANNEL) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
  }

  Serial.println();
  Serial.print("WiFi Channel : "); Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println();
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = CHANNEL;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  Serial.println();
  Serial.println("Setup complete.");
  Serial.println("-------------");
  
  Serial.println();
  Serial.println("Waiting for message from ESP32 Master...");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1);
}






