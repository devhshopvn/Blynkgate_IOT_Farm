# Blynk Gate IoT Garden - Giám sát Nhiệt độ, Độ ẩm, Độ ẩm đất, Cảm biến ánh sáng và Điều khiển Máy bơm

Dự án sử dụng **Arduino Uno** kết hợp với các cảm biến **DHT11**, **độ ẩm đất**, **cảm biến ánh sáng**, và điều khiển máy bơm qua **relay**. Dữ liệu từ các cảm biến được gửi lên **Blynk Cloud** để giám sát điều kiện môi trường từ xa qua Internet. Thêm vào đó, người dùng có thể điều khiển máy bơm thủ công qua ứng dụng Blynk.

## Tổng quan

Trong dự án này, chúng ta sẽ:
- Sử dụng cảm biến **DHT11** để đo nhiệt độ và độ ẩm.
- Sử dụng cảm biến độ ẩm đất để đo độ ẩm của đất.
- Sử dụng cảm biến ánh sáng để đo cường độ ánh sáng.
- Điều khiển máy bơm qua relay, bật/tắt máy bơm tự động hoặc thủ công.
- Hiển thị thông tin lên màn hình LCD và gửi dữ liệu lên **Blynk Cloud** để giám sát từ xa qua ứng dụng Blynk.

## Yêu cầu phần cứng

- **Module BlynkGate**
- **Vietduino Uno** (hoặc Arduino Uno tương thích)
- **Cảm biến DHT11** (Nhiệt độ và Độ ẩm)
- **Cảm biến độ ẩm đất**
- **Cảm biến ánh sáng**
- **Relay** để điều khiển máy bơm
- **Màn hình LCD 16x2** (với giao tiếp I2C)

## Yêu cầu phần mềm

- **Arduino IDE**: [Tải về tại đây](https://www.arduino.cc/en/software)
- **Thư viện BlynkGate** để giao tiếp với ứng dụng Blynk
- **Thư viện MKL_DHT** để giao tiếp với cảm biến DHT11
- **Thư viện MKL_LiquidCrystal_I2C** để điều khiển màn hình LCD

## Sơ đồ mạch
                            +--------------------------+
                            |    Arduino Uno           |
                            |                          |
                            |  +--------------------+  |
                            |  |     Pin 5V         |  |
                            |  |     Pin GND        |  |
                            |  |     Pin A1         |  |
                            |  |     Pin A2         |  |
                            |  |     Pin A3         |  |
                            |  |     Pin A4 (SDA)   |  |
                            |  |     Pin A5 (SCL)   |  |
                            |  |     Pin D10        |  |
                            |  +--------------------+  |
                            +-----------+--------------+
                                        |
                                        |
           +----------------------------+---------------------------+-----------------------------------+
           |                            |                           |                                   |
     +-----+-----+              +-------+-------+           +-------+-------+                   +---------------+    
     |  DHT11     |             |  Soil Moisture|           |  LDR (Light)  |                   |  Relay module |
     |  Sensor    |             |  Sensor       |           |  Sensor       |                   |     pin 10    |
     +-----+-----+              +-------+-------+           +-------+-------+                   +---------------+
           |                            |                           |                                   |  
           |                            |                           |                                   |
    +------+---------+       +----------+---------+        +-------+--------+                   +---------------+    
    | Pin 5V          |      | Pin 5V             |        | Pin 5V         |                   |      PUM      |
    | Pin GND         |      | Pin GND            |        | Pin GND        |                   |               |
    | Pin A1 (SIG)    |      | Pin A2 (SIG)       |        | Pin A3 (SIG)   |                   +---------------+
    +-----------------+       +-------------------+        +----------------+

## Cài đặt

1. **Kết nối phần cứng**:
   - Kết nối cảm biến **DHT11**, **cảm biến độ ẩm đất**, **cảm biến ánh sáng**, **máy bơm qua relay** với Arduino theo sơ đồ mạch trên.

2. **Cài đặt các thư viện cần thiết**:
   - Mở **Arduino IDE**.
   - Vào **Tools > Manage Libraries** và tìm kiếm "MAKERLABVN"
   - Cài đặt các thư viện tương ứng.

3. **Tải code lên Arduino**:
   - Mở **Arduino IDE**.
   - Sao chép và dán đoạn mã sau vào Arduino IDE:

```cpp
/*
  Tiêu đề: Giám sát Nhiệt độ, Độ ẩm, Độ ẩm đất, Cảm biến ánh sáng và Điều khiển Máy bơm
  Mô tả: Đọc dữ liệu từ các cảm biến DHT11, độ ẩm đất và ánh sáng, điều khiển máy bơm qua relay, hiển thị thông tin trên màn hình LCD, và thêm tính năng bật tắt máy bơm thủ công qua Blynk.
*/

// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "GET_FROM_BLYNK_CLOUND"
#define BLYNK_TEMPLATE_NAME "GET_FROM_BLYNK_CLOUND"
#define BLYNK_AUTH_TOKEN "GET_FROM_BLYNK_CLOUND"

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
}

void sendSensorData() {
  // Đọc dữ liệu từ các cảm biến
  float temp = dht.readTemperature();   // Đọc nhiệt độ từ cảm biến DHT11
  float humidity = dht.readHumidity();  // Đọc độ ẩm từ cảm biến DHT11

  // Đọc giá trị độ ẩm đất và chuyển đổi thành phần trăm (0-100%)
  int soilMoisturePercentage = map(analogRead(SOIL_MOISTURE_PIN), 390, 536, 100, 0);  // Chuyển đổi giá trị analog sang phần trăm

  // Đọc giá trị ánh sáng và chuyển đổi sang tỉ lệ 0-100%
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
      Blynk.virtualWrite(VIRTUAL_PIN_MANUAL_PUMP, false);  // Tắt công tắc
    }
  }
}
```
4. **Lấy thông tin từ Blynk Cloud**:
   - Truy cập [Blynk Cloud](https://blynk.cloud/) và tạo một template mới.
   - Sao chép `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, và `BLYNK_AUTH_TOKEN`.

   
5. **Tải code lên Arduino** và đảm bảo nó kết nối với Wi-Fi.
