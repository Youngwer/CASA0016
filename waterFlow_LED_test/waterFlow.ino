#include <Adafruit_NeoPixel.h>

// 超声波传感器相关
#define AOUT A0           // URM09 AOUT 接到 A0
#define LED_PIN 6         // LED 数据引脚接 D6
#define NUMPIXELS 8       // LED 的数量
#define CLOSE_DISTANCE 5  // 近距离阈值 (cm)
#define MID_DISTANCE 10   // 中距离阈值 (cm)
#define MAX_DISTANCE 200  // 最大允许距离 (cm)

// 水流传感器相关
#define FLOW_SENSOR_PIN 2 // 水流传感器信号接到 D2
volatile int pulseCount = 0; // 用于中断计数
float flowRate = 0;          // 流量（单位：L/min）
unsigned long oldTime = 0;   // 上一次计算的时间

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN);

// 中断服务函数
void pulseCounter() {
  pulseCount++; // 每次检测到下降沿，计数加 1
}

void setup() {
  Serial.begin(9600);         // 初始化串口
  pixels.begin();             // 初始化 LED
  pixels.clear();             // 确保 LED 全部熄灭
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP); // 水流传感器
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // 配置中断触发模式

  Serial.println("System initialized!");
}

void loop() {
  // 超声波传感器部分
  int sensorValue = analogRead(AOUT);
  float voltage = sensorValue * (3.0 / 1023.0); // 假设 AOUT 输出范围是 0-3.0V
  float distance = voltage * 100;              // 1cm = 10mV

  // 水流传感器部分
  unsigned long currentTime = millis();
  if (currentTime - oldTime >= 1000) { // 每秒计算一次流量
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)); // 暂时关闭中断
    flowRate = (pulseCount / 7.5); // 7.5 为传感器校准系数
    pulseCount = 0;               // 重置脉冲计数
    oldTime = currentTime;        // 更新时间戳
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // 恢复中断

    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
  }

  // 根据距离设置 LED 颜色
  if (distance > MAX_DISTANCE || distance <= 0) {
    setLEDColor(255, 0, 0, NUMPIXELS); // 显示红色警告
  } else if (distance <= CLOSE_DISTANCE) {
    int redLEDs = map(distance, 0, CLOSE_DISTANCE, NUMPIXELS, 0);
    setLEDColor(255, 0, 0, redLEDs);
  } else if (distance <= MID_DISTANCE) {
    setLEDColor(128, 128, 0, NUMPIXELS / 2); // 显示黄色
  } else {
    setLEDColor(0, 128, 0, NUMPIXELS / 2);   // 显示绿色
  }

  // 根据水流量改变 LED 的亮度或颜色
  if (flowRate > 10.0) {
    setLEDColor(0, 0, 255, NUMPIXELS); // 显示蓝色表示水流量高
  } else if (flowRate > 0) {
    setLEDColor(0, 255, 255, NUMPIXELS / 2); // 显示青色表示水流量中等
  }

  delay(200); // 延迟 200ms
}

// 设置 LED 的颜色和亮起的 LED 数量
void setLEDColor(uint8_t red, uint8_t green, uint8_t blue, int numLEDs) {
  pixels.clear();  // 清除当前 LED 显示
  for (int i = 0; i < numLEDs; i++) {
    pixels.setPixelColor(i, red, green, blue); // 设置每个像素的颜色
  }
  pixels.show();  // 更新显示
}
