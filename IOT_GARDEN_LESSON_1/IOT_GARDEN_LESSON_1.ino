/*
  Tiêu đề:  Demo Nhiệt độ và Độ ẩm
  Mô tả: Đọc giá trị từ cảm biến DHT11 và gửi lên Blynk mỗi 5 giây.
              - Virtual Pin V1 (cho nhiệt độ)
              - Virtual Pin V2 (cho độ ẩm).
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
