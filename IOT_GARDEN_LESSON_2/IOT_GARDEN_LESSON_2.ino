/*
  Tiêu đề: Demo Nhiệt độ, Độ ẩm, Độ ẩm đất và Cảm biến ánh sáng
  Mô tả: Đọc giá trị từ các cảm biến DHT11, độ ẩm đất và ánh sáng, gửi lên Blynk mỗi 5 giây.
          - Virtual Pin V1 (cho nhiệt độ)
          - Virtual Pin V2 (cho độ ẩm)
          - Virtual Pin V3 (cho độ ẩm đất)
          - Virtual Pin V4 (cho ánh sáng tỉ lệ 0-100%)
*/

// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "GET_FROM_BLYNK_CLOUND"
#define BLYNK_TEMPLATE_NAME "GET_FROM_BLYNK_CLOUND"
#define BLYNK_AUTH_TOKEN "GET_FROM_BLYNK_CLOUND"

// Định nghĩa Virtual Pin cho các cảm biến
#define VIRTUAL_PIN_TEMP 1        // Virtual Pin V1 dành cho nhiệt độ
#define VIRTUAL_PIN_HUMIDITY 2    // Virtual Pin V2 dành cho độ ẩm
#define VIRTUAL_PIN_SOIL_MOISTURE 3 // Virtual Pin V3 dành cho độ ẩm đất
#define VIRTUAL_PIN_LIGHT 4       // Virtual Pin V4 dành cho ánh sáng

#include "BlynkGate.h"
#include "MKL_DHT.h"

// Thiết lập chân cho DHT và các cảm biến khác
#define DHTPIN A1               // Chân dữ liệu từ cảm biến DHT
#define SOIL_MOISTURE_PIN A2    // Chân cảm biến độ ẩm đất
#define LIGHT_SENSOR_PIN A3     // Chân cảm biến ánh sáng

MKL_DHT dht(DHTPIN, DHT11);

char ssid[] = "Hshop.vn";
char pass[] = ""; // Để trống nếu wifi không có mật khẩu

unsigned long previousMillis = 0;
const long interval = 5000; // Khoảng thời gian chờ (5 giây)

void setup() {
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendSensorData();
  }
}

void sendSensorData() {
  // Đọc dữ liệu từ các cảm biến
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  // Đọc giá trị ánh sáng và chuyển đổi sang tỉ lệ 0-100%
  int lightRaw = analogRead(LIGHT_SENSOR_PIN);
  int lightLevel = map(lightRaw, 0, 1023, 0, 100); // Chuyển đổi giá trị ánh sáng

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_TEMP, temp);              // Gửi nhiệt độ lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity);      // Gửi độ ẩm lên Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_SOIL_MOISTURE, soilMoisture); // Gửi độ ẩm đất
  Blynk.virtualWrite(VIRTUAL_PIN_LIGHT, lightLevel);       // Gửi ánh sáng (0-100%)

  // In ra Serial Monitor
  Serial.print("Nhiệt độ: ");
  Serial.print(temp);
  Serial.print(" *C, Độ ẩm: ");
  Serial.print(humidity);
  Serial.print(" %, Độ ẩm đất: ");
  Serial.print(soilMoisture);
  Serial.print(", Ánh sáng: ");
  Serial.print(lightLevel);
  Serial.println(" %");
}

// Cấu trúc BLYNK_WRITE_DEFAULT để nhận dữ liệu từ tất cả các virtual pin
BLYNK_WRITE_DEFAULT() {
  int value = param.asInt();
  Serial.print("Nhập dữ liệu từ V");
  Serial.print(request.pin);
  Serial.print(": ");
  Serial.println(value);
}
