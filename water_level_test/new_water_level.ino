#define AOUT A0 // URM09 的模拟信号输出引脚

void setup() {
  Serial.begin(9600); // 初始化串口
  pinMode(AOUT, INPUT);
}

void loop() {
  int sensorValue = analogRead(AOUT); // 读取传感器的模拟值
  float distance = sensorValue / 2.0; // 根据拟合公式计算距离

  // 输出模拟值和计算的距离
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Distance: ");
  Serial.print(distance, 2); // 距离保留两位小数
  Serial.println(" cm");

  delay(500); // 每 500ms 输出一次数据
}
