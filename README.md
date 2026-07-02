# MLX90641 Thermal Demo — 실시간 열화상 데모

> 적외선 어레이 센서가 읽은 16×12 온도장을 Nano ESP32가 **보정·스트리밍**하고, 브라우저 **Web Serial 앱**이 실시간 열화상으로 렌더링하는 센서 시각화 프로젝트. 고등학교 SW·AI 융합수업용.

🔗 **프로젝트개요:** https://xparapx.github.io/Arduino_Thermal_MLX90641_Demo/  
🖥️ **WebUI:** https://xparapx.github.io/Arduino_Thermal_MLX90641_Demo/ui/mlx90641_thermal_ui.html  
📘 **매뉴얼:** https://xparapx.github.io/Arduino_Thermal_MLX90641_Demo/manual.html

`MLX90641` · `Arduino Nano ESP32` · `I2C` · `Web Serial API` · `Canvas` · `흑체복사·적외선`

---

## 01. 프로젝트 요약

> "열은 어떻게 보이는가"를 **센서로 직접 측정하고, 보정하고, 눈으로 보는** 프로젝트입니다.

MLX90641 적외선 어레이 센서(16×12, 192픽셀)의 온도 프레임을 **Nano ESP32**가 I2C로 읽어 보정 계산 후, USB 시리얼로 PC에 전송하고, 브라우저의 **Web Serial** HTML 앱이 이를 실시간 열화상으로 렌더링합니다. 클라우드도 모델 학습도 없는 **순수 측정·시각화** 파이프라인이라, 흑체복사·적외선·방사율 단원과 곧장 연결됩니다.

## 02. 프로세스 흐름도

```
 ① 센서 측정        ② 보정 계산        ③ 시리얼 스트리밍     ④ 수신·파싱        ⑤ 시각화
  16×12 IR 프레임     온도 변환            CSV 한 줄/프레임       Web Serial          캔버스 열화상
  + Ta + Vdd        (방사율 0.95)        115200bps             깨진 프레임 폐기      통계·트렌드
 [MLX90641] ─I2C→ [Nano ESP32] ─USB→ START:..:END ─수신→ [브라우저 앱] ─render→ [화면]
```

## 03. 준비물

### 하드웨어 (실물)
| 항목 | 비고 |
|---|---|
| Arduino Nano ESP32 | ESP32-S3 · USB-C |
| Grove Thermal Imaging Camera (MLX90641) | I2C `0x33` · 16×12 |
| Grove 케이블 | I2C 포트 연결: **SDA=A4, SCL=A5** |
| PC | Chrome 또는 Edge (Web Serial 필요) |

### 소프트웨어 · 라이브러리
| 도구 | 비고 |
|---|---|
| Arduino IDE | 2.x |
| MLX9064x_Universal (동봉) | `lib/MLX9064x_Universal.zip` ZIP 설치 · ESP32 패치본 |
| Chrome / Edge | Safari·Firefox 불가 |

## 04. 기술 스택

- **센서** — MLX90641 적외선 어레이, 공장 보정값 EEPROM 내장
- **MCU** — Arduino Nano ESP32 (ESP32-S3), I2C 100kHz
- **통신** — I2C(센서), USB Serial 115200bps(CSV 스트리밍)
- **UI** — Web Serial API, Canvas 렌더링·컬러맵(Ironbow/Jet/Medical/Grayscale)·보간, 단일 HTML

## 05. 저장소 구조

```
Arduino_Thermal_MLX90641_Demo/
├─ lib/
│  └─ MLX9064x_Universal.zip          # ★ ESP32 패치 + 칩 자동판별 라이브러리 (ZIP 설치)
├─ firmware/                          # 보드 펌웨어 (.ino)
│  ├─ i2c_diagnostic.ino              # 라이브러리 무관 30초 진단
│  └─ mlx90641_serial.ino             # CSV 스트리밍 펌웨어
├─ ui/
│  └─ mlx90641_thermal_ui.html        # Web Serial 열화상 UI 앱
├─ docs/                              # ★ GitHub Pages 게시 폴더
│  ├─ index.html                      # 프로젝트 개요 (랜딩)
│  ├─ manual.html                     # 운용·트러블슈팅 매뉴얼
│  └─ ui/
│     └─ mlx90641_thermal_ui.html     # UI 앱 Pages 게시 사본 (ui/ 원본과 동일)
├─ README.md
└─ .gitignore
```

> `lib/MLX9064x_Universal.zip` 는 공식 Seeed 라이브러리에서 **ESP32 I2C 드라이버 결함만 교체하고 칩 자동판별(0x240A bit6)을 더한** 포크입니다. 보정 수학(Melexis)은 원본 그대로입니다. **driver.cpp를 손으로 고칠 필요 없이 ZIP만 설치**하면 되고, 라이브러리 재설치 시 패치가 유실되는 문제도 사라집니다.

---

## 최초 1회 설정 (라이브러리 설치)

1. **라이브러리 ZIP 설치** — Arduino IDE `스케치 → 라이브러리 포함하기 → .ZIP 라이브러리 추가`에서 `lib/MLX9064x_Universal.zip` 선택. **driver.cpp를 손으로 패치할 필요 없음** (ZIP 안에 이미 ESP32 패치본 + 칩 자동판별 포함). 기존 `Seeed_Arduino_MLX9064x`가 설치돼 있으면 헤더 충돌 방지를 위해 **삭제 권장**.
2. **펌웨어 업로드** — `firmware/mlx90641_serial.ino` → 보드 `Arduino Nano ESP32`, 포트 선택 후 업로드. 스케치는 래퍼 헤더 **`MLX9064x_Universal.h` 하나만** include 하면 되고, 칩(90641/90640)은 **자동 판별**되므로 직접 고를 필요 없음. (이 펌웨어는 라이브러리 동봉 예제 `02_SerialStream_UI`와 동일 — `파일 → 예제 → MLX9064x_Universal`에서도 열 수 있음.)
3. **1차 확인** — (선택) 먼저 `firmware/i2c_diagnostic.ino`(라이브러리 무관 raw 점검) 또는 라이브러리 예제 `01_Diagnostic`을 올려 `ACK: OK` / `MLX90641 (16x12)` / `통신 OK`를 확인. 이후 스트리밍 펌웨어의 시리얼 모니터(115200)에서 부팅 시 `INFO:CHIP=MLX90641,COLS=16,ROWS=12` → `START:25.31,...:END` 줄이 흐르고 값이 실온(20~30℃)이면 정상.

## 실행 방법

1. **시리얼 모니터 닫기** — IDE 시리얼 모니터가 열려 있으면 포트를 점유한다 (가장 흔한 연결 실패 원인).
2. `ui/mlx90641_thermal_ui.html`을 Chrome/Edge에서 연다 (더블클릭, 서버 불필요). 또는 [GitHub Pages에서 바로 열기](https://xparapx.github.io/Arduino_Thermal_MLX90641_Demo/ui/mlx90641_thermal_ui.html).
3. 통신 속도 `115200` 확인 → **장치 연결** → 팝업에서 Nano ESP32 포트 선택.
4. 센서 앞에 손을 비추면 핫스팟 십자선(H)이 손을 따라가고 최고 온도가 33~36℃를 표시해야 한다.

---

## 핵심 사양

**시리얼 프로토콜** — 부팅 시 1회 `INFO:CHIP=...,COLS=..,ROWS=..` (해상도 자동 통지), 이후 프레임당 한 줄 `START:t0,t1,...,tN:END` (115200bps). 픽셀 수 N+1은 칩에 따라 **자동**: MLX90641=192, MLX90640=768. 프레임 I2C 실패 시 `ERR_FRAME:<code>`.

**기본 보정값** — 방사율 0.95, TA_SHIFT 8. 피부는 0.97~0.98, 금속면은 별도 보정 필요. 정밀 측정 시 기준 온도계로 1점 교정.

**칩 판별** — EEPROM `0x240A` bit6: `1`=MLX90641(16×12), `0`=MLX90640(32×24). 칩-API 불일치 시 Vdd=inf·Ta=NaN 발생.

## 빠른 진단표 (요약)

| 증상 | 원인 / 해결 |
|---|---|
| 전 픽셀 NaN, Vdd=inf | 칩-API 불일치 (동봉 라이브러리는 자동판별로 예방) → 다른 라이브러리 혼용 여부 확인, `MLX9064x_Universal.zip`만 사용 |
| 프레임마다 동일한 비정상 값(동결) | 공식 라이브러리 사용 중 → `lib/MLX9064x_Universal.zip`(패치본)으로 재설치 |
| 컴파일 시 `... .h: No such file` | 스케치는 `MLX9064x_Universal.h` **하나만** include (내부 API 헤더 직접 포함 불필요). 구버전 Seeed 라이브러리 잔존 시 삭제 |
| 0x33 미검출 | Grove 케이블 I2C 포트(A4/A5)·전원 LED 확인, 케이블 교체 |
| 간헐적 ERR_FRAME | I2C 400k→100kHz 하향, 케이블 최단화 |
| UI에서 포트 안 보임 | IDE 시리얼 모니터 닫기, Chrome/Edge 확인, 케이블 재연결 |

> 전체 진단 타임라인·라이브러리 이슈 조사·증상별 상세표는 `docs/manual.html` 참조 (웹: https://xparapx.github.io/Arduino_Thermal_MLX90641_Demo/manual.html).

## 작업 로그

- **2026-06** 칩 오인(90640 가정 → 실제 90641) 발견, MLX90641 API로 전환
- **2026-06** 공식 라이브러리 I2C 드라이버 ESP32 비호환 진단 → `I2CRead` 로컬 패치 적용
- **2026-06** UI 유효성 게이트(깨진 프레임 폐기), 컬러맵·보간·시뮬레이터 구성

---

*Maintainer: xparapx*
