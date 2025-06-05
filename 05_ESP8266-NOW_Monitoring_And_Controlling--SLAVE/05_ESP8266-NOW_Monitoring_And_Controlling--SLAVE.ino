// 05_ESP-NOW_Monitoring_And_Controlling--SLAVE (ESP8266)
//---------------------------------------- Bao gồm các thư viện cần thiết
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "DHT.h"
//----------------------------------------

// Định nghĩa kênh Wi-Fi
// Cả ESP8266 (Slave) và ESP32 (Master) phải dùng cùng kênh Wi-Fi
#define CHANNEL 1

// Định nghĩa chân kết nối cảm biến DHT11
#define DHTPIN D2
// Định nghĩa loại cảm biến DHT
#define DHTTYPE DHT11

// Định nghĩa chân kết nối rơ-le điều khiển bóng đèn
#define LB_A_PIN D5
#define LB_B_PIN D6

// ĐỊA CHỈ MAC CỦA ESP32-MASTER (thay bằng địa chỉ thực tế)
uint8_t broadcastAddress[] = {0x14, 0x33, 0x5c, 0x02, 0x71, 0xc8};

//---------------------------------------- Cấu trúc gửi dữ liệu
// Phải trùng khớp với cấu trúc bên nhận
typedef struct structure_to_send_messages {
  String  s_ID;
  float   s_Temp;
  int     s_Humd;
  bool    s_Sta_LB_A;
  bool    s_Sta_LB_B;
} structure_to_send_messages;

// Khởi tạo biến cấu trúc gửi
structure_to_send_messages s_Messages;
//----------------------------------------

//---------------------------------------- Cấu trúc nhận dữ liệu
typedef struct structure_to_receive_messages {
  String  r_ID;
  String  r_CMD;
} structure_to_receive_messages;

// Khởi tạo biến cấu trúc nhận
structure_to_receive_messages r_Messages;
//----------------------------------------

float Temp;
int Humd;

// Khởi tạo cảm biến DHT11
DHT DHT_11(DHTPIN, DHTTYPE);

//________________________________________________________________________________ Lấy dữ liệu từ cảm biến DHT11
void get_Data_From_DHT11_Sensor() {
  Temp = random(200, 350) / 10.0;
  Humd = random(40, 91);

  Serial.println();
  Serial.println("-------------Lấy dữ liệu từ cảm biến DHT11");

  if (isnan(Temp) || isnan(Humd)) {
    Serial.println(F("Không đọc được dữ liệu từ DHT11!"));
    Temp = 0.0;
    Humd = 0;
  }

  Serial.print(F("Nhiệt độ : "));
  Serial.print(Temp);
  Serial.print(F(" °C | Độ ẩm : "));
  Serial.print(Humd);
  Serial.println(F(" %"));
  Serial.println("-------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Xử lý sau khi gửi
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("-------------");
  Serial.println("ESP8266 Slave.");
  Serial.print("Gửi tới\t: "); Serial.println(macStr);
  Serial.print("Trạng thái\t: ");
  Serial.println(sendStatus == 0 ? "Thành công" : "Thất bại");
  Serial.println("-------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Xử lý khi nhận dữ liệu
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("-------------");
  Serial.println("ESP8266 Slave.");
  Serial.print("Nhận từ\t: "); Serial.println(macStr);
  Serial.print("Số byte\t: "); Serial.println(len);
  Serial.print("ID\t: "); Serial.println(r_Messages.r_ID);
  Serial.print("Lệnh\t: "); Serial.println(r_Messages.r_CMD);
  Serial.println("-------------");

  processing_Received_Messages();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Xử lý lệnh nhận
void processing_Received_Messages() {
  if (r_Messages.r_ID == "MSTR") {   
    if (r_Messages.r_CMD == "GAD") {
      send_Message();
    } else if (r_Messages.r_CMD == "LB_A_ON") {
      digitalWrite(LB_A_PIN, LOW);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_A_OFF") {
      digitalWrite(LB_A_PIN, HIGH);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_B_ON") {
      digitalWrite(LB_B_PIN, LOW);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_B_OFF") {
      digitalWrite(LB_B_PIN, HIGH);
      send_Message();
    } 
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Gửi dữ liệu
void send_Message() {
  delay(100);

  get_Data_From_DHT11_Sensor();

  s_Messages.s_ID = "SLVE";
  s_Messages.s_Temp = Temp;
  s_Messages.s_Humd = Humd;
  s_Messages.s_Sta_LB_A = (digitalRead(LB_A_PIN) == LOW);
  s_Messages.s_Sta_LB_B = (digitalRead(LB_B_PIN) == LOW);

  // Gửi dữ liệu qua ESP-NOW
  uint8_t result = esp_now_send(broadcastAddress, (uint8_t *) &s_Messages, sizeof(s_Messages));

  if (result != 0) {
    Serial.println();
    Serial.println("Lỗi khi gửi dữ liệu");
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Hàm setup()
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP8266 Slave.");
  Serial.println("-------------");
  Serial.println("Bắt đầu cài đặt.");

  pinMode(LB_A_PIN, OUTPUT);
  pinMode(LB_B_PIN, OUTPUT);

  digitalWrite(LB_A_PIN, HIGH);
  digitalWrite(LB_B_PIN, HIGH);
  
  WiFi.mode(WIFI_STA);

  // Bắt đầu khởi tạo ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println();
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }

  // Đăng ký hàm xử lý sau khi gửi
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);

  // Thêm thiết bị Master làm peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, CHANNEL, NULL, 0);

  // Đăng ký hàm xử lý khi nhận dữ liệu
  esp_now_register_recv_cb(OnDataRecv);

  DHT_11.begin();

  Serial.println();
  Serial.println("Cài đặt hoàn tất.");
  Serial.println("-------------");
  Serial.println("Chờ tin nhắn từ ESP32 Master...");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Hàm loop()
void loop() {
  delay(1);
}
//________________________________________________________________________________ 
