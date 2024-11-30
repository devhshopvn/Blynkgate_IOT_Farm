/*
  Tiêu đề: Giám sát Nhiệt độ, Độ ẩm, Độ ẩm đất, Cảm biến ánh sáng và Điều khiển Máy bơm
  Mô tả: Đọc dữ liệu từ các cảm biến DHT11, độ ẩm đất và ánh sáng, điều khiển máy bơm qua relay, hiển thị thông tin trên màn hình LCD, và thêm tính năng bật tắt máy bơm thủ công qua Blynk.
*/

// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "TMPL6Gyrmi-8_"                    // ID mẫu Blynk
#define BLYNK_TEMPLATE_NAME "blynk gate"                     // Tên mẫu Blynk
#define BLYNK_AUTH_TOKEN "fJltZnwukHtoRz0IctDps7mbZgAod49K"  // Mã thông báo Blynk để kết nối

// Định nghĩa Virtual Pin cho các cảm biến và các tính năng
#define VIRTUAL_PIN_TEMP 1           // Virtual Pin V1 dành cho nhiệt độ
#define VIRTUAL_PIN_HUMIDITY 2       // Virtual Pin V2 dành cho độ ẩm
#define VIRTUAL_PIN_SOIL_MOISTURE 3  // Virtual Pin V3 dành cho độ ẩm đất
#define VIRTUAL_PIN_LIGHT 4          // Virtual Pin V4 dành cho ánh sáng
#define VIRTUAL_PIN_MANUAL_PUMP 5    // Virtual Pin V5 dành cho bật/tắt máy bơm thủ công

#include "BlynkGate.h"              // Thư viện BlynkGate để giao tiếp với ứng dụng Blynk
#include "MKL_DHT.h"                // Thư viện để đọc dữ liệu từ cảm biến DHT11
#include <Wire.h>                   // Thư viện hỗ trợ giao tiếp I2C
#include <MKL_LiquidCrystal_I2C.h>  // Thư viện để điều khiển màn hình LCD I2C

// Định nghĩa Relay và LCD
#define RELAY_PIN 10  // Chân điều khiển relay (máy bơm)

// Thiết lập chân cho DHT và các cảm biến khác
#define DHTPIN A1             // Chân dữ liệu từ cảm biến DHT
#define SOIL_MOISTURE_PIN A2  // Chân cảm biến độ ẩm đất
#define LIGHT_SENSOR_PIN A3   // Chân cảm biến ánh sáng

// Khởi tạo đối tượng LCD
MKL_LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ của LCD là 0x27, màn hình 16x2

MKL_DHT dht(DHTPIN, DHT11);  // Khởi tạo cảm biến DHT11

char ssid[] = "Hshop.vn";  // Tên mạng Wi-Fi
char pass[] = "";          // Mật khẩu Wi-Fi (để trống nếu không có mật khẩu)

unsigned long previousMillis = 0;  // Biến lưu thời gian để gửi dữ liệu theo chu kỳ
const long interval = 5000;        // Khoảng thời gian chờ (5 giây)
bool pumpManualControl = false;    // Biến điều khiển máy bơm thủ công (bật/tắt bằng Blynk)
bool Is_pump_run = false;          // Biến kiểm tra trạng thái máy bơm
unsigned long pumpMillis = 0;      // Biến lưu thời gian bật máy bơm thủ công

void sendSensorData();  // Khai báo hàm gửi dữ liệu cảm biến

void setup() {
  Serial.begin(115200);                       // Khởi tạo Serial để theo dõi thông tin từ Arduino
  dht.begin();                                // Khởi tạo cảm biến DHT11
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  // Kết nối đến Blynk
  pinMode(RELAY_PIN, OUTPUT);                 // Đặt chân điều khiển relay làm output

  // Khởi động màn hình LCD
  lcd.init();
  lcd.backlight();  // Bật đèn nền cho màn hình LCD

  // In thông báo trên LCD
  lcd.setCursor(0, 0);  // Đặt con trỏ vào vị trí dòng 0, cột 0
  lcd.print("IOT Garden");

  delay(2000);  // Dừng 2 giây để hiển thị thông báo trên màn hình LCD
}

void loop() {
  Blynk.run();  // Chạy Blynk để tiếp tục nhận và gửi dữ liệu

  if (pumpManualControl) {
    if (Is_pump_run == true) {
      // Nếu máy bơm đã bật thủ công, tắt sau 10 giây
      if (millis() - pumpMillis >= 15000) {  // Sau 15 giây
        digitalWrite(RELAY_PIN, LOW);        // Tắt máy bơm
        pumpManualControl = false;           // Dừng điều khiển thủ công
      }
    } else {
      pumpMillis = millis();  // Ghi lại thời gian bật máy bơm thủ công
    }
    digitalWrite(RELAY_PIN, HIGH);    // Bật máy bơm
    Is_pump_run = pumpManualControl;  // Cập nhật trạng thái máy bơm
  }

  unsigned long currentMillis = millis();  // Lấy thời gian hiện tại
  // Gửi dữ liệu cảm biến sau mỗi khoảng thời gian interval (5 giây)
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Cập nhật thời gian
    sendSensorData();                // Gửi dữ liệu từ cảm biến lên Blynk
  }

  // Nếu nút điều khiển máy bơm thủ công được nhấn
}

void sendSensorData() {
  // Đọc dữ liệu từ các cảm biến
  float temp = dht.readTemperature();   // Đọc nhiệt độ từ cảm biến DHT11
  float humidity = dht.readHumidity();  // Đọc độ ẩm từ cảm biến DHT11

  // Đọc giá trị độ ẩm đất và chuyển đổi thành phần trăm (0-100%)
  int soilMoisturePercentage = map(analogRead(SOIL_MOISTURE_PIN), 390, 536, 100, 0);  // Chuyển đổi giá trị analog sang phần trăm

  // Đọc giá trị ánh sáng và chuyển đổi sang tỉ lệ 0-100%
  Serial.println(analogRead(LIGHT_SENSOR_PIN));
  int lightLevel = map(analogRead(LIGHT_SENSOR_PIN), 0, 500, 100, 0);  // Chuyển đổi giá trị ánh sáng

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_TEMP, temp);                             // Gửi nhiệt độ lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity);                     // Gửi độ ẩm lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_SOIL_MOISTURE, soilMoisturePercentage);  // Gửi độ ẩm đất (phần trăm)
  Blynk.virtualWrite(VIRTUAL_PIN_LIGHT, lightLevel);                      // Gửi ánh sáng (0-100%)

  // Điều khiển máy bơm nước thông qua relay
  if (pumpManualControl == false) {                       // Nếu không điều khiển thủ công
    if (soilMoisturePercentage < 40) {                    // Nếu độ ẩm đất thấp (giá trị dưới 40%)
      digitalWrite(RELAY_PIN, HIGH);                      // Bật máy bơm
      Blynk.virtualWrite(VIRTUAL_PIN_MANUAL_PUMP, true);  // Bật công tắc thủ công trên Blynk
    } else {
      digitalWrite(RELAY_PIN, LOW);                        // Tắt máy bơm
      Blynk.virtualWrite(VIRTUAL_PIN_MANUAL_PUMP, false);  // Tắt công tắc thủ công trên Blynk
    }
  }

  // Hiển thị thông tin trên LCD
  lcd.clear();          // Xóa màn hình LCD
  lcd.setCursor(0, 0);  // Đặt con trỏ vào vị trí dòng 0, cột 0
  lcd.print("Temp: ");
  lcd.print(temp);  // Hiển thị nhiệt độ
  lcd.print("C");

  lcd.setCursor(0, 1);  // Đặt con trỏ vào dòng 1, cột 0
  lcd.print("Soil: ");
  lcd.print(soilMoisturePercentage);  // Hiển thị độ ẩm đất dưới dạng phần trăm
  lcd.print("%");

  // In ra Serial Monitor để theo dõi giá trị cảm biến
  Serial.print("Nhiệt độ: ");
  Serial.print(temp);
  Serial.print(" *C, Độ ẩm: ");
  Serial.print(humidity);
  Serial.print(" %, Độ ẩm đất: ");
  Serial.print(soilMoisturePercentage);  // Hiển thị độ ẩm đất phần trăm
  Serial.print(", Ánh sáng: ");
  Serial.print(lightLevel);
  Serial.println(" %");
}

// Cấu trúc BLYNK_WRITE_DEFAULT để nhận dữ liệu từ tất cả các virtual pin
BLYNK_WRITE_DEFAULT() {
  int DetectPin = request.pin;  // Lấy số pin ảo
  int myInt = param.asInt();    // Lấy trạng thái của nút (0 hoặc 1)

  if (DetectPin == VIRTUAL_PIN_MANUAL_PUMP) {  // Nếu pin ảo là pin điều khiển máy bơm thủ công
    if (myInt == 1) {
      pumpManualControl = true;  // Bắt đầu điều khiển thủ công
    }
  }
}
