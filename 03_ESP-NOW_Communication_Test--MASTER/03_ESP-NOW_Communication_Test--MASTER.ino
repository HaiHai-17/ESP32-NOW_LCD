#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#define CHANNEL 1

unsigned long previousMillis = 0;
const long interval = 8000; // Master gửi data về Slave mỗi 8s

// địa chỉ của Slave
uint8_t broadcastAddress[] = {0xe8, 0xdb, 0x84, 0xc2, 0x48, 0x68};

// Cấu trúc dữ liệu gửi đi (structure_to_send_messages)
typedef struct structure_to_send_messages {
  char str_s_Text[32];
  int int_s_Count;
} structure_to_send_messages;

structure_to_send_messages s_Messages;

// Cấu trúc dữ liệu nhận về (structure_to_receive_messages)
typedef struct structure_to_receive_messages {
  char str_r_Text[32];
  int int_r_Count;
} structure_to_receive_messages;

structure_to_receive_messages r_Messages;

int cnt = 0;

esp_now_peer_info_t peerInfo;

// Callback khi gửi xong (send callback)
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("ESP32 Master.");
  Serial.print("Send to\t\t: "); Serial.println(macStr);
  Serial.print("Send Status\t: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback khi nhận dữ liệu (receive callback)
void OnDataRecv(const esp_now_recv_info_t* recv_info, const uint8_t* incomingData, int len) {
  char macStr[18];
  const uint8_t* mac_addr = recv_info->src_addr;

  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("ESP32 Master.");
  Serial.print("Receive from\t: "); Serial.println(macStr);
  Serial.print("Bytes received\t: "); Serial.println(len);
  Serial.print("str_r_Text\t: "); Serial.println(r_Messages.str_r_Text);
  Serial.print("int_r_Count\t: "); Serial.println(r_Messages.int_r_Count);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("ESP32 Master.");
  Serial.println("-------------");
  Serial.println("Start Setup.");

  WiFi.mode(WIFI_STA);

  // Thiết lập kênh WiFi (WiFi Channel)
  int cur_WIFIchannel = WiFi.channel();
  if (cur_WIFIchannel != CHANNEL) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
  }

  Serial.print("WiFi Channel : "); Serial.println(WiFi.channel());

  // Khởi tạo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback gửi
  esp_now_register_send_cb(OnDataSent);

  // Khởi tạo peer
  memset(&peerInfo, 0, sizeof(peerInfo)); // Làm sạch vùng nhớ peerInfo
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Đăng ký callback nhận
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Setup complete.");
  Serial.println("-------------");
}

void loop() {
  unsigned long currentMillis = millis();

  if (previousMillis == 0 || currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    cnt++;
    if (cnt > 100) cnt = 1;

    strcpy(s_Messages.str_s_Text, "Hello ESP32 Slave");
    s_Messages.int_s_Count = cnt;

    // Gửi dữ liệu
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&s_Messages, sizeof(s_Messages));

    if (result != ESP_OK) {
      Serial.println();
      Serial.print("Error sending the data. Error Code: ");
      Serial.println(result);
    }
  }

  delay(1); // Có thể bỏ nếu không cần
}
