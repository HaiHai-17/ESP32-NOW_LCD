#include <ESP8266WiFi.h>  // Thư viện WiFi dành cho ESP8266

void readMacAddress() {
  uint8_t baseMac[6];
  WiFi.macAddress(baseMac);  // Lấy địa chỉ MAC mặc định của ESP8266

  Serial.print("Default ESP8266 MAC Address : ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                baseMac[0], baseMac[1], baseMac[2],
                baseMac[3], baseMac[4], baseMac[5]);

  Serial.print("Default ESP8266 MAC Address for Arduino code : ");
  Serial.printf("{0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x};\n",
                baseMac[0], baseMac[1], baseMac[2],
                baseMac[3], baseMac[4], baseMac[5]);
}

void setup() {
  Serial.begin(115200);  // Khởi tạo cổng Serial
  delay(2000);           // Đợi khởi động ổn định

  WiFi.mode(WIFI_STA);   // Đặt chế độ WiFi là Station (STA - client)

  readMacAddress();      // Gọi hàm đọc địa chỉ MAC
}

void loop() {
  delay(10);             // Lặp lại với delay để tránh quá tải CPU
}
