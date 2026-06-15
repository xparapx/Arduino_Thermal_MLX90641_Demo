// i2c_diagnostic.ino
// MLX90641 30초 진단 스케치 — Arduino Nano ESP32
// 목적: 배선(ACK) → 칩 판별(90641/90640) → 통신 무결성을 한 번에 점검.
// 정상 동작이 의심될 때 본 펌웨어를 먼저 올려 시리얼 모니터(115200)로 결과 확인.
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  delay(500);
  Wire.begin();
  Wire.setClock(100000);  // ESP32 안정성 위해 100kHz

  // [1] 0x33 ACK 확인 — 배선/전원 점검
  Wire.beginTransmission(0x33);
  Serial.printf("ACK: %s\n", Wire.endTransmission() == 0 ? "OK" : "FAIL(배선!)");

  // [2] 칩 판별: EEPROM 0x240A bit6 (1=MLX90641, 0=MLX90640)
  Wire.beginTransmission(0x33); Wire.write(0x24); Wire.write(0x0A);
  Wire.endTransmission(false); Wire.requestFrom(0x33, 2);
  uint16_t w = (Wire.read() << 8) | Wire.read();
  Serial.printf("0x240A=0x%04X -> %s\n", w,
                (w & 0x0040) ? "MLX90641(16x12)" : "MLX90640(32x24)");

  // [3] 같은 주소 2회 독취 일치 = 통신 무결성
  uint16_t a[2];
  for (int t = 0; t < 2; t++) {
    Wire.beginTransmission(0x33); Wire.write(0x24); Wire.write(0x30);
    Wire.endTransmission(false); Wire.requestFrom(0x33, 2);
    a[t] = (Wire.read() << 8) | Wire.read();
  }
  Serial.printf("0x2430(gain)=0x%04X, 재독취 %s\n", a[0],
                a[0] == a[1] ? "일치(통신 OK)" : "불일치(신호 문제!)");
}

void loop() { delay(10000); }
