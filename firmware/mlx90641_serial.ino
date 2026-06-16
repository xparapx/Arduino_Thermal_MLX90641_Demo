/*
  mlx90641_serial.ino  —  Web Serial 열화상 UI(HTML 앱)용 스트리밍 펌웨어
  보드: Arduino Nano ESP32 (ESP32-S3) / I2C: SDA=A4, SCL=A5 / 115200bps

  동봉 라이브러리 lib/MLX9064x_Universal.zip 설치 후 업로드하세요.
  (라이브러리 예제 02_SerialStream_UI 와 동일한 코드입니다.)

  출력 포맷 — 한 프레임당 한 줄:
    START:v0,v1,...,vN:END          (N = pixelCount-1, 칩에 따라 192/768 자동)
  부팅 시 INFO 한 줄로 해상도를 알려주므로 UI에서 자동 적응에 활용 가능:
    INFO:CHIP=MLX90641,COLS=16,ROWS=12

  칩 종류(MLX90641 16×12 / MLX90640 32×24)는 EEPROM 0x240A bit6으로
  자동 판별되므로 펌웨어 수정 없이 두 모듈 모두 동작합니다.
*/
#include <MLX9064x_Universal.h>

MLX9064xUniversal cam;
float frame[MLX9064X_MAX_PIXELS];

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!cam.begin()) {
    while (1) { Serial.printf("ERR_INIT:%d\n", cam.lastError()); delay(1000); }
  }
  cam.setRefreshRate(MLX9064X_RATE_8HZ);

  Serial.printf("INFO:CHIP=%s,COLS=%u,ROWS=%u\n",
                cam.chip() == MLX9064X_MLX90641 ? "MLX90641" : "MLX90640",
                cam.width(), cam.height());
  delay(300);
}

void loop() {
  if (cam.readFrame(frame) != 0) {
    Serial.printf("ERR_FRAME:%d\n", cam.lastError());
    delay(50);
    return;
  }
  const uint16_t n = cam.pixelCount();
  Serial.print("START:");
  for (uint16_t i = 0; i < n; i++) {
    Serial.print(frame[i], 2);
    if (i < n - 1) Serial.print(",");
  }
  Serial.println(":END");
}
