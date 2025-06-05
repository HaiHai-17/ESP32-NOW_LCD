// 05_ESP-NOW_Monitoring_And_Controlling--SLAVE
//---------------------------------------- Bao gồm các thư viện cần thiết (Including Libraries).
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "DHT.h"
//----------------------------------------

// Định nghĩa kênh Wi-Fi (Wi-Fi channel).
// Kênh cấu hình có thể từ 1 đến 13 (mặc định).
// "ESP32 Master" và "ESP32 Slave" phải dùng cùng kênh Wi-Fi.
#define CHANNEL 1

// Định nghĩa chân (PIN) kết nối với cảm biến DHT11.
#define DHTPIN 15
// Định nghĩa loại cảm biến DHT.
#define DHTTYPE DHT11

// Định nghĩa các chân (PIN) kết nối với module relay để điều khiển bóng đèn.
#define LB_A_PIN 5
#define LB_B_PIN 18

// THAY THẾ BẰNG ĐỊA CHỈ MAC CỦA ESP32-RECEIVER.
// Trong dự án này, thay bằng địa chỉ MAC của ESP32-MASTER.
uint8_t broadcastAddress[] = {0x14, 0x33, 0x5c, 0x02, 0x71, 0xc8};

//---------------------------------------- Cấu trúc dữ liệu để gửi (structure for sending data).
// Phải trùng khớp với cấu trúc bên bộ nhận (receiver).
typedef struct structure_to_send_messages {
  String  s_ID;         // ID gửi
  float   s_Temp;       // Nhiệt độ (temperature)
  int     s_Humd;       // Độ ẩm (humidity)
  bool    s_Sta_LB_A;   // Trạng thái bóng đèn A
  bool    s_Sta_LB_B;   // Trạng thái bóng đèn B
} structure_to_send_messages;

// Tạo một biến kiểu "structure_to_send_messages" tên là "s_Messages".
structure_to_send_messages s_Messages;
//----------------------------------------

//---------------------------------------- Cấu trúc dữ liệu để nhận (structure for receiving data).
// Phải trùng khớp với cấu trúc bên bộ gửi (sender).
typedef struct structure_to_receive_messages {
  String  r_ID;    // ID nhận
  String  r_CMD;   // Lệnh nhận được (command)
} structure_to_receive_messages;

// Tạo biến kiểu "structure_to_receive_messages" tên là "r_Messages".
structure_to_receive_messages r_Messages;
//----------------------------------------

float Temp;   // Biến lưu nhiệt độ
int Humd;     // Biến lưu độ ẩm

// Khai báo đối tượng "esp_now_peer_info_t" với tên "peerInfo" để lưu thông tin peer.
esp_now_peer_info_t peerInfo;

// Khai báo đối tượng cảm biến DHT với tên "DHT_11" và thiết lập chân, loại cảm biến.
DHT DHT_11(DHTPIN, DHTTYPE);

//________________________________________________________________________________
// Hàm đọc dữ liệu từ cảm biến DHT11
void get_Data_From_DHT11_Sensor() {
  // Đọc nhiệt độ theo độ C (mặc định).
  // Temp = DHT_11.readTemperature();
  // Humd = DHT_11.readHumidity();

  // Ở đây tạm thời dùng dữ liệu giả lập ngẫu nhiên để test.
  Temp = random(200, 350) / 10.0;
  Humd = random(40, 91);

  Serial.println();
  Serial.println("-------------Lấy dữ liệu từ cảm biến DHT11");

  // Kiểm tra nếu đọc cảm biến lỗi (NaN), thì đặt lại giá trị mặc định.
  if (isnan(Temp) || isnan(Humd)) {
    Serial.println(F("Không đọc được dữ liệu từ cảm biến DHT11!"));
    Temp = 0.0;
    Humd = 0;
  }

  // In ra nhiệt độ và độ ẩm lên Serial Monitor.
  Serial.print(F("Nhiệt độ : "));
  Serial.print(Temp);
  Serial.print(F(" °C | Độ ẩm : "));
  Serial.print(Humd);
  Serial.println(F(" %"));
  Serial.println("-------------");
}
//________________________________________________________________________________

//________________________________________________________________________________
// Hàm callback khi dữ liệu được gửi đi xong (OnDataSent).
void OnDataSent(const uint8_t * mac_addr, esp_now_send_status_t status) {
  // Biến lưu địa chỉ MAC dạng chuỗi.
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("-------------");
  Serial.println("ESP32 Slave.");
  Serial.print("Gửi đến\t: "); Serial.println(macStr);
  Serial.print("Trạng thái\t: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Gửi thành công" : "Gửi thất bại");
  Serial.println("-------------");
}
//________________________________________________________________________________

//________________________________________________________________________________
// Hàm callback khi nhận dữ liệu (OnDataRecv).
void OnDataRecv(const uint8_t * mac_addr, const uint8_t * incomingData, int len) {
  // Biến lưu địa chỉ MAC dạng chuỗi.
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  // Sao chép dữ liệu nhận được vào biến cấu trúc r_Messages.
  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("-------------");
  Serial.println("ESP32 Slave.");
  Serial.print("Nhận từ\t: "); Serial.println(macStr);
  Serial.print("Số byte nhận\t: "); Serial.println(len);
  Serial.print("ID\t: ");
  Serial.println(r_Messages.r_ID);
  Serial.print("Lệnh\t: ");
  Serial.println(r_Messages.r_CMD);
  Serial.println("-------------");

  // Xử lý dữ liệu nhận được.
  processing_Received_Messages();
}
//________________________________________________________________________________

//________________________________________________________________________________
// Hàm xử lý dữ liệu nhận được.
void processing_Received_Messages() {
  // Nếu ID nhận là "MSTR" (Master)
  if (r_Messages.r_ID == "MSTR") {   
    // Nếu lệnh nhận là "GAD" (Get Data - yêu cầu dữ liệu)
    if (r_Messages.r_CMD == "GAD") {
      send_Message();
    } 
    // Các lệnh điều khiển relay bật/tắt bóng đèn A, B.
    else if (r_Messages.r_CMD == "LB_A_ON") {
      digitalWrite(LB_A_PIN, HIGH);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_A_OFF") {
      digitalWrite(LB_A_PIN, LOW);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_B_ON") {
      digitalWrite(LB_B_PIN, HIGH);
      send_Message();
    } else if (r_Messages.r_CMD == "LB_B_OFF") {
      digitalWrite(LB_B_PIN, LOW);
      send_Message();
    } 
  }
}
//________________________________________________________________________________

//________________________________________________________________________________
// Hàm gửi dữ liệu (send_Message).
void send_Message() {
  delay(100);

  //---------------------------------------- Chuẩn bị dữ liệu gửi đi đến ESP32-Master.
  get_Data_From_DHT11_Sensor();
  
  s_Messages.s_ID = "SLVE";  // Gán ID gửi là "SLVE" (Slave)
  s_Messages.s_Temp = Temp;  // Nhiệt độ hiện tại
  s_Messages.s_Humd = Humd;  // Độ ẩm hiện tại

  // Lấy trạng thái của bóng đèn A (bật hay tắt).
  if (digitalRead(LB_A_PIN) == HIGH) {
    s_Messages.s_Sta_LB_A = true;
  } else {
    s_Messages.s_Sta_LB_A = false;
  }

  // Lấy trạng thái của bóng đèn B (bật hay tắt).
  if (digitalRead(LB_B_PIN) == HIGH) {
    s_Messages.s_Sta_LB_B = true;
  } else {
    s_Messages.s_Sta_LB_B = false;
  }
  //----------------------------------------
  
  // Gửi dữ liệu qua ESP-NOW.
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &s_Messages, sizeof(s_Messages));

  if (result != ESP_OK) {
    Serial.println();
    Serial.println("Error sending the data"); // Lỗi khi gửi dữ liệu
  }
}
//________________________________________________________________________________

//________________________________________________________________________________
// Hàm khởi tạo (setup)
void setup() {
  Serial.begin(115200);   // Khởi động Serial với tốc độ 115200 baud
  Serial.println();
  delay(2000);

  Serial.println("ESP32 Slave.");

  Serial.println();
  Serial.println("-------------");
  Serial.println("Bắt đầu khởi tạo.");

  // Cấu hình chân output cho relay điều khiển bóng đèn.
  pinMode(LB_A_PIN, OUTPUT);
  pinMode(LB_B_PIN, OUTPUT);

  // Đặt trạng thái ban đầu là tắt bóng đèn.
  digitalWrite(LB_A_PIN, LOW);
  digitalWrite(LB_B_PIN, LOW);
  
  WiFi.mode(WIFI_STA);   // Đặt chế độ Wi-Fi là Station (STA)

  //---------------------------------------- Kiểm tra hoặc đặt lại kênh Wi-Fi.
  int cur_WIFIchannel = WiFi.channel();
  
  if (cur_WIFIchannel != CHANNEL) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
  }

  Serial.println();
  Serial.print("WiFi Channel : "); Serial.println(WiFi.channel());
  //----------------------------------------
  
  // Khởi tạo ESP-NOW.
  if (esp_now_init() != ESP_OK) {
    Serial.println();
    Serial.println("Lỗi khi khởi tạo ESP-NOW");  // Lỗi khi khởi tạo ESP-NOW
    return;
  }

  // Đăng ký hàm callback OnDataSent để nhận biết trạng thái gửi dữ liệu.
  esp_now_register_send_cb(OnDataSent);

  // Đăng ký peer.
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = CHANNEL;  
  peerInfo.encrypt = false;

  // Thêm peer.
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Thêm peer thất bại");  // Thêm peer thất bại
    return;
  }

  // Đăng ký hàm callback OnDataRecv khi nhận được dữ liệu.
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  DHT_11.begin();  // Khởi động cảm biến DHT11.

  Serial.println();
  Serial.println("Khởi tạo hoàn tất.");
  Serial.println("-------------");
  
  Serial.println();
  Serial.println("Đang chờ tin nhắn từ ESP32 Master...");
}
//________________________________________________________________________________

//________________________________________________________________________________
// Vòng lặp chính (loop)
void loop() {
  // Ở đây không có gì đặc biệt, chỉ delay 1ms.
  delay(1);
}
//________________________________________________________________________________
