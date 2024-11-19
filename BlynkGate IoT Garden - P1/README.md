
# Blynk Gate IoT Garden - P1: Giám sát Nhiệt độ và Độ ẩmsử dụng Arduino Uno & DHT11

Dự án sử dụng **Arduino** đọc giá trị nhiệt độ và độ ẩm bằng cảm biến **DHT11** và gửi dữ liệu lên **Blynk Cloud** bằng module **BlynkGate** giúp giám sát điều kiện môi trường từ xa qua Internet.

## Tổng quan

Trong dự án này, chúng ta sẽ:
- Sử dụng cảm biến **DHT11** để đo nhiệt độ và độ ẩm.
- Kết nối **Arduino** với **Blynk Cloud** thông qua module Wi-Fi **BlynkGate**.
- Gửi dữ liệu cảm biến lên **Blynk Cloud** và hiển thị qua ứng dụng Blynk.

## Yêu cầu phần cứng

- **Module BlynkGate**
- **Vietduino Uno**
- **Mạch MakerEdu Shield for Vietduino**
- **Cảm biến độ ẩm nhiệt độ MKE-S14 DHT11**

## Yêu cầu phần mềm

- **Arduino IDE**: [Tải về tại đây](https://www.arduino.cc/en/software)
- **Thư viện "MAKERLABVN"**.

## Sơ đồ mạch Uno

| **Arduino UNO**      | **DHT11**  |
|--------------|---------------|
| VCC          | 5V            |
| GND          | GND           |
| A1           | SIG            |

| **Arduino UNO**   | **BlynkGate**  |
|--------------|----------------|
| VCC          | 5V             |
| GND          | GND            |
| A4           | SDA            |
| A5           | SCL            |

## Cài đặt

1. **Kết nối Arduino Uno với cảm biến DHT11** và **module BlynkGate** theo sơ đồ mạch.
2. **Cài đặt các thư viện cần thiết**:
   - Mở Arduino IDE.
   - Vào **Tools > Manage Libraries** và tìm kiếm `MAKERLABVN`.
   - Cài đặt thư viện phiên bản mới nhất.
3. **Tải code lên Arduino Uno**:
   - Mở **Arduino IDE**.
   - Sao chép và dán đoạn mã sau vào Arduino IDE:

```cpp
/*
  Tiêu đề:  Demo Nhiệt độ và Độ ẩm
  Mô tả: Đọc giá trị từ cảm biến DHT11 và gửi lên Blynk mỗi 5 giây.
              - Virtual Pin V1 (cho nhiệt độ)
              - Virtual Pin V2 (cho độ ẩm)
*/

// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "GET_FROM_BLYNK_CLOUND"
#define BLYNK_TEMPLATE_NAME "GET_FROM_BLYNK_CLOUND"
#define BLYNK_AUTH_TOKEN "GET_FROM_BLYNK_CLOUND"

// Định nghĩa Virtual Pin cho nhiệt độ và độ ẩm
#define VIRTUAL_PIN_TEMP 1 // Virtual Pin V1 dành cho nhiệt độ
#define VIRTUAL_PIN_HUMIDITY 2 // Virtual Pin V2 dành cho độ ẩm

#include "BlynkGate.h"
#include "MKL_DHT.h"

// Thiết lập chân cho DHT
#define DHTPIN A1 // Chân dữ liệu từ cảm biến DHT
MKL_DHT dht(DHTPIN, DHT11);

char ssid[] = "Hshop.vn";
char pass[] = ""; // Để trống nếu wifi không có mật khẩu

// Biến để sử dụng với millis()
// Variables for millis timing
unsigned long previousMillis = 0;
const long interval = 5000; // Khoảng thời gian chờ (5 giây)

void setup() {
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  // Lấy thời gian hiện tại
  // Get the current time
  unsigned long currentMillis = millis();

  // So sánh thời gian đã trôi qua với khoảng thời gian mong muốn
  // Compare the time passed with the interval
  if (currentMillis - previousMillis >= interval) {
    // Cập nhật thời gian trước đó
    // Update the previous time
    previousMillis = currentMillis;

    // Gọi hàm gửi dữ liệu cảm biến
    // Call the function to send sensor data
    sendSensorData();
  }
}

void sendSensorData() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Gửi lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_TEMP, temp); // gửi nhiệt độ lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity); // gửi độ ẩm lên Blynk

  Serial.print("Nhiệt độ: ");
  Serial.print(temp);
  Serial.print(" *C, Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");
}

// Cấu trúc BLYNK_WRITE_DEFAULT để nhận dữ liệu từ tất cả các virtual pin
BLYNK_WRITE_DEFAULT() {
  // Nhận giá trị từ ứng dụng Blynk (int, float, string, etc.)
  int myInt = param.asInt();  // Đọc giá trị dạng int, có thể dùng asFloat(), asString() cho các kiểu khác
  
  Serial.print("input V");
  Serial.print(request.pin); // In ra số pin ảo đang nhận dữ liệu
  Serial.print(": ");
  Serial.println(myInt);     // In ra giá trị nhận được
}

```

4. **Lấy thông tin từ Blynk Cloud**:
   - Truy cập [Blynk Cloud](https://blynk.cloud/) và tạo một template mới.
   - Sao chép `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, và `BLYNK_AUTH_TOKEN`.

   
5. **Tải code lên Arduino** và đảm bảo nó kết nối với Wi-Fi.

## Sử dụng

1. **Chạy ứng dụng Blynk**:
   - Tạo một dự án mới trên ứng dụng Blynk.
   - Thêm hai widget (Gauge hoặc Value Display) cho nhiệt độ và độ ẩm.
   - Gán **Virtual Pin V1** cho nhiệt độ và **Virtual Pin V2** cho độ ẩm.

2. **Xem dữ liệu**:
   - Khi mã đã chạy và thiết bị kết nối với Wi-Fi, bạn có thể giám sát giá trị nhiệt độ và độ ẩm theo thời gian thực qua ứng dụng Blynk.

