#include <ESP8266WiFi.h>        // Thư viện WiFi cho ESP8266
extern "C" {
  #include <espnow.h>           // Thư viện ESP-NOW cho ESP8266
}

#define CHANNEL 1               // Kênh WiFi

// Địa chỉ MAC của Master
uint8_t broadcastAddress[] = {0x14, 0x33, 0x5c, 0x02, 0x71, 0xc8};

// Cấu trúc dữ liệu gửi
typedef struct structure_to_send_messages {
  char str_s_Text[32];
  int int_s_Count;
} structure_to_send_messages;

structure_to_send_messages s_Messages;

// Cấu trúc dữ liệu nhận
typedef struct structure_to_receive_messages {
  char str_r_Text[32];
  int int_r_Count;
} structure_to_receive_messages;

structure_to_receive_messages r_Messages;

int cnt = 0;

// Gọi khi gửi xong dữ liệu
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
           mac_addr[0], mac_addr[1], mac_addr[2], 
           mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("ESP8266 Slave.");
  Serial.print("Send message to\t\t: "); Serial.println(macStr);
  Serial.print("Message sending status\t: ");
  Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
}

// Gọi khi nhận được dữ liệu
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
           mac_addr[0], mac_addr[1], mac_addr[2], 
           mac_addr[3], mac_addr[4], mac_addr[5]);

  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("ESP8266 Slave.");
  Serial.print("Receive from\t: "); Serial.println(macStr);
  Serial.print("Bytes received\t: "); Serial.println(len);
  Serial.print("str_r_Text\t: "); Serial.println(r_Messages.str_r_Text);
  Serial.print("int_r_Count\t: "); Serial.println(r_Messages.int_r_Count);

  cnt = r_Messages.int_r_Count;

  Send_Reply_Message();
}

// Gửi phản hồi trở lại Master
void Send_Reply_Message() {
  delay(1500);

  strcpy(s_Messages.str_s_Text, "Hello ESP8266 Master");
  s_Messages.int_s_Count = cnt;

  uint8_t result = esp_now_send(broadcastAddress, (uint8_t *) &s_Messages, sizeof(s_Messages));

  if (result != 0) {
    Serial.println();
    Serial.println("Error sending the data");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  Serial.println("ESP8266 Slave.");
  Serial.println("-------------");
  Serial.println("Start Setup.");

  WiFi.mode(WIFI_STA);           // Chuyển sang chế độ Station (STA)

  // Thiết lập kênh WiFi (ESP8266 không thay đổi được kênh như ESP32, phải kết nối AP có cùng kênh)
  WiFi.disconnect();             // Ngắt kết nối WiFi để chọn kênh
  delay(100);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);  // Có thể vừa gửi vừa nhận
  esp_now_register_send_cb(OnDataSent);       // Gắn callback gửi
  esp_now_register_recv_cb(OnDataRecv);       // Gắn callback nhận

  if (esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, CHANNEL, NULL, 0) != 0) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Setup complete.");
  Serial.println("-------------");
  Serial.println("Waiting for message from ESP32 Master...");
}

void loop() {
  delay(1);  // Giữ vòng lặp nhẹ
}
