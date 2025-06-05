// 06_ESP-NOW_Monitoring_And_Controlling--MASTER

//---------------------------------------- Thư viện cần sử dụng.
#include <WiFi.h>                               // Thư viện Wi-Fi.
#include <esp_wifi.h>                           // Thư viện cấu hình Wi-Fi nâng cao.
#include <esp_now.h>                            // Thư viện ESP-NOW để giao tiếp không dây.
#include <lvgl.h>                               // Thư viện LVGL (LittlevGL) - Thư viện giao diện người dùng.
#include <TFT_eSPI.h>                           // Thư viện điều khiển màn hình TFT.
#include <SPI.h>
#include <XPT2046_Touchscreen.h>                // Thư viện cảm ứng XPT2046.
#include "ui.h"                                 // Tập tin giao diện EEZ Studio đã thiết kế.
//---------------------------------------- 

// Cấu hình kênh Wi-Fi cho ESP-NOW (cả Master và Slave phải dùng cùng kênh)
#define CHANNEL 1

// Touchscreen pins - Thử với cấu hình gốc trước
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

// Độ phân giải màn hình
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

//---------------------------------------- Kết quả hiệu chuẩn (calibration) cảm ứng - Thử với giá trị gốc trước
#define touchscreen_Min_X 200
#define touchscreen_Max_X 3700
#define touchscreen_Min_Y 240
#define touchscreen_Max_Y 3800
//---------------------------------------- 

//---------------------------------------- Bộ đếm thời gian/millis
// Khoảng thời gian để lấy dữ liệu từ Slave (cảm biến nhiệt độ, độ ẩm, trạng thái đèn)
const long interval_Millis_Get_Data_From_Slave = 5000;
unsigned long previous_Millis_Get_Data_From_Slave = 0;

// Khoảng thời gian cập nhật giao diện
const long interval_Millis_Update_UI = 5;
unsigned long previous_Millis_Update_UI = 0;
//---------------------------------------- 

// Địa chỉ MAC của thiết bị nhận (ESP32-Slave)
uint8_t broadcastAddress[] = {0xb0, 0xa7, 0x32, 0x22, 0x91, 0xe8};

//---------------------------------------- Cấu trúc gửi dữ liệu
typedef struct structure_to_send_messages {
  String  s_ID;
  String  s_CMD;
} structure_to_send_messages;

structure_to_send_messages s_Messages; // Khởi tạo biến gửi
//----------------------------------------

//---------------------------------------- Cấu trúc nhận dữ liệu
typedef struct structure_to_receive_messages {
  String  r_ID;
  float   r_Temp;
  int     r_Humd;
  bool    r_Sta_LB_A;
  bool    r_Sta_LB_B;
} structure_to_receive_messages;

structure_to_receive_messages r_Messages; // Khởi tạo biến nhận
//----------------------------------------

// Cờ trạng thái gửi/nhận
bool message_Sending_Status = false;
bool message_Receiving_Process_Complete = false;

// Bộ nhớ đệm cho LVGL (đề xuất dùng 1/10 màn hình)
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// Biến lưu toạ độ cảm ứng
int x, y, z; // Thay đổi từ uint16_t thành int như code mẫu

// Lưu thời điểm tick cuối cùng (LVGL)
uint32_t lastTick = 0;

// Thông tin peer ESP-NOW
esp_now_peer_info_t peerInfo;

// Đối tượng cảm ứng
SPIClass touchscreenSPI(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

//________________________________________________________________________________
// Hàm callback khi gửi dữ liệu xong (ESP-NOW)
void OnDataSent(const uint8_t * mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println();
  Serial.println("ESP32 Master.");
  Serial.print("Gửi đến\t: "); Serial.println(macStr);
  Serial.print("Trạng thái\t: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Gửi thành công" : "Gửi thất bại");

  message_Sending_Status = (status == ESP_NOW_SEND_SUCCESS);
}
//________________________________________________________________________________


//________________________________________________________________________________
// Hàm callback khi nhận dữ liệu (ESP-NOW)
void OnDataRecv(const uint8_t * mac_addr, const uint8_t * incomingData, int len) {
  message_Receiving_Process_Complete = false;
  
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  memcpy(&r_Messages, incomingData, sizeof(r_Messages));

  Serial.println();
  Serial.println("ESP32 Master.");
  Serial.print("Nhận từ\t: "); Serial.println(macStr);
  Serial.print("Số byte nhận được\t: "); Serial.println(len);
  Serial.print("ID\t: "); Serial.println(r_Messages.r_ID);
  Serial.print("Nhiệt độ\t: "); Serial.println(r_Messages.r_Temp);
  Serial.print("Độ ẩm\t: "); Serial.println(r_Messages.r_Humd);
  Serial.print("Trạng thái đèn A\t: "); Serial.println(r_Messages.r_Sta_LB_A ? "BAT" : "TAT");
  Serial.print("Trạng thái đèn B\t: "); Serial.println(r_Messages.r_Sta_LB_B ? "BAT" : "TAT");

  message_Receiving_Process_Complete = true;
}
//________________________________________________________________________________


//________________________________________________________________________________
// Xử lý dữ liệu đã nhận và cập nhật UI
void processing_Received_Messages() {
  if (r_Messages.r_ID == "SLVE") {
    char ca_Temp[10];
    lv_snprintf(ca_Temp, sizeof(ca_Temp), "%.2f °C", r_Messages.r_Temp);
    lv_label_set_text(objects.label_temperature, ca_Temp);

    char ca_Humd[7];
    lv_snprintf(ca_Humd, sizeof(ca_Humd), "%d %%", r_Messages.r_Humd);
    lv_label_set_text(objects.label_humidity, ca_Humd);

    lv_label_set_text(objects.label_mntr_lb_a, r_Messages.r_Sta_LB_A ? "Relay 1 : BAT" : "Relay 1 : TAT");
    lv_label_set_text(objects.label_mntr_lb_b, r_Messages.r_Sta_LB_B ? "Relay 2 : BAT" : "Relay 2 : TAT");
  }
}
//________________________________________________________________________________


//________________________________________________________________________________
// Hàm gửi lệnh từ Master sang Slave
void send_Message(String cmd) {
  s_Messages.s_ID = "MSTR";
  s_Messages.s_CMD = cmd;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &s_Messages, sizeof(s_Messages));

  if (result != ESP_OK) {
    Serial.println();
    Serial.println("Lỗi khi gửi dữ liệu");
  }
}
//________________________________________________________________________________


//________________________________________________________________________________
// Ghi log từ thư viện LVGL (nếu cần debug)
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
//________________________________________________________________________________


//________________________________________________________________________________
// Cập nhật giao diện UI
void update_UI() {
  lv_tick_inc(millis() - lastTick);
  lastTick = millis();
  lv_timer_handler(); // Gọi hàm cập nhật LVGL
}
//________________________________________________________________________________


//________________________________________________________________________________
// Đọc dữ liệu cảm ứng (Touchscreen) - Debug version
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data){
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    // Chuyển đổi toạ độ cảm ứng về đúng hướng xoay của LVGL
    // Nếu bạn dùng màn hình ngang, hoán đổi x/y và đảo chiều như sau:
    x = map(p.x, touchscreen_Max_X, touchscreen_Min_X, 1, SCREEN_HEIGHT);
    y = map(p.y, touchscreen_Max_Y, touchscreen_Min_Y, 1, SCREEN_WIDTH);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Giới hạn giá trị toạ độ
    // x = constrain(x, 0, SCREEN_WIDTH - 1);
    // y = constrain(y, 0, SCREEN_HEIGHT - 1);

    data->point.x = x;
    data->point.y = y;

    // Debug toạ độ cảm ứng
    // Serial.printf("X = %d | Y = %d | Pressure = %d\n", x, y, z);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

//________________________________________________________________________________


//________________________________________________________________________________
// Xử lý sự kiện bật/tắt đèn A
static void switch_ctr_light_bulb_a_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    bool checked = lv_obj_has_state((lv_obj_t*) lv_event_get_target(e), LV_STATE_CHECKED);
    send_Message(checked ? "LB_A_ON" : "LB_A_OFF");
  }
}
//________________________________________________________________________________


//________________________________________________________________________________
// Xử lý sự kiện bật/tắt đèn B
static void switch_ctr_light_bulb_b_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    bool checked = lv_obj_has_state((lv_obj_t*) lv_event_get_target(e), LV_STATE_CHECKED);
    send_Message(checked ? "LB_B_ON" : "LB_B_OFF");
  }
}
//________________________________________________________________________________


//________________________________________________________________________________
// HÀM SETUP()
void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  Serial.println("ESP32 + TFT LCD Touchscreen ILI9341 + LVGL + EEZ Studio");
  Serial.println("GIAO DIỆN ĐIỀU KHIỂN KHÔNG DÂY DÙNG ESP-NOW.");
  Serial.println("ESP32 Master.");

  // Khởi động cảm ứng - Cập nhật theo code mẫu
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0); // Xoay theo chiều ngang

  //---------------------------------------- Cài đặt LVGL
  lv_init();
  lv_log_register_print_cb(log_print); // Cho phép debug

  // draw_buf = new uint8_t[DRAW_BUF_SIZE];
  lv_display_t * disp; 
  disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270); // Xoay giao diện

  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  //---------------------------------------- Khởi tạo UI từ EEZ Studio
  ui_init();

  //---------------------------------------- Gắn xử lý sự kiện cho các switch
  lv_obj_add_event_cb(objects.switch_ctr_light_bulb_a, switch_ctr_light_bulb_a_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.switch_ctr_light_bulb_b, switch_ctr_light_bulb_b_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // Thiết lập WiFi chế độ Station
  WiFi.mode(WIFI_STA);

  // Đảm bảo WiFi dùng đúng kênh
  int cur_WIFIchannel = WiFi.channel();
  if (cur_WIFIchannel != CHANNEL) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
  }

  // Khởi tạo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }

  // Đăng ký callback gửi và nhận
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Lỗi thêm peer ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  Serial.println("Khởi tạo hoàn tất.");
}
//________________________________________________________________________________


//________________________________________________________________________________
// HÀM LOOP()
void loop() {
  // Gửi lệnh yêu cầu dữ liệu mỗi 5 giây
  unsigned long current_Millis_Get_Data_From_Slave = millis();
  if (previous_Millis_Get_Data_From_Slave == 0 || current_Millis_Get_Data_From_Slave - previous_Millis_Get_Data_From_Slave >= interval_Millis_Get_Data_From_Slave) {
    previous_Millis_Get_Data_From_Slave = current_Millis_Get_Data_From_Slave;
    send_Message("GAD"); // GAD = Get All Data
  }

  // Cập nhật UI
  unsigned long current_Millis_Update_UI = millis();
  if (previous_Millis_Update_UI == 0 || current_Millis_Update_UI - previous_Millis_Update_UI >= interval_Millis_Update_UI) {
    previous_Millis_Update_UI = current_Millis_Update_UI;

    // Hiển thị trạng thái kết nối
    if (message_Sending_Status) {
      lv_label_set_text(objects.label_slave_info, "Trang thai Slave : Ket noi");
      lv_obj_set_style_text_color(objects.label_slave_info, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
      lv_label_set_text(objects.label_slave_info, "Trang thai Slave : Mat ket noi");
      lv_obj_set_style_text_color(objects.label_slave_info, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // Nếu có dữ liệu mới từ Slave, xử lý và cập nhật giao diện
    if (message_Receiving_Process_Complete) {
      processing_Received_Messages();
      message_Receiving_Process_Complete = false;
    }

    update_UI();
  }
}
//________________________________________________________________________________