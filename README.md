# 3D Scanner - STM32 Firmware

STM32F446REで作ったレーザーライン3Dスキャナーの組み込みファームウェアです。OV7670カメラで画像を撮影し、各行で赤いレーザーラインを検出し、ステッパーモーターでターンテーブルを回転させ、結果をUARTでPCに送信して3D再構成を行います。
<img width="4000" height="3000" alt="20260429_141256" src="https://github.com/user-attachments/assets/b4a0021b-8241-4249-97e8-2d31a01e6a6d" />


## ハードウェア

- **MCU**: STM32F446RE - Cortex-M4, 128KB RAM, 512KB Flash, 16MHz HSI
- **カメラ**: OV7670 - QVGA (320×240) RGB565, 焦点距離3.6mmレンズ
- **ステッパーモーター**: 28BYJ-48 - ハーフステップモード, 4098ステップ/回転, ULN2003ドライバー
- **レーザー**: 赤色ラインレーザー - カメラ軸に対して20°の角度で取り付け

## ピン接続

### OV7670カメラ

- PA8 (MCO1) → XLK: 8MHzカメラクロック (HSI/2)
- PC5 → RET: カメラリセット（アクティブLow）
- PB10 → SCL: I2C2 SCCBクロック (100kHz)
- PC12 → SDA: I2C2 SCCBデータ
- PA4 → HS: DCMI HSYNC
- PB7 → VS: DCMI VSYNC
- PA6 → PCLK: DCMIピクセルクロック
- PC6–PC9, PC11, PB6, PB8–PB9 → D0–D7: DCMI 8ビットパラレルデータ

### ステッパーモーター（28BYJ-48）

- PC0 → IN1
- PC1 → IN2
- PC2 → IN3
- PC3 → IN4

### UART（PC通信）

- PA2 → TX: USART2送信 (115200 baud)
- PA3 → RX: USART2受信（割り込み駆動）

## ビルド方法

`arm-none-eabi-gcc`ツールチェーンとCMake ≥ 3.22が必要です。

```bash
cmake -B build --preset Debug
cmake --build build
```

ビルド結果の `build/3d_scan_repo.elf` をSTM32F446REに書き込んでください。

## アーキテクチャ

### スキャンパイプライン

```
OV7670カメラ (320x240) → DCMI + DMA (ハーフフレーム撮影) → 各行の最輝点検出 → UART TXでPCに送信
                                       |
                                       → ステッパーモーター + ステップカウンター → UART TXでPCに送信
```

1. **撮影**: OV7670がDCMIとDMA転送で320×240 RGB565画像を撮影
2. **ハーフフレーム**: 128KB RAMの制限のため、画像を上下120行ずつ2回に分けてDCMIクロップ機能で撮影し、共通の76.8KBバッファを使い回します
3. **検出**: 各行で赤チャンネルが最大のX座標（レーザーライン位置）を見つけます
4. **送信**: 240個の最輝点位置（480バイト）＋ステッパーのステップカウンター（2バイト）をUARTでPCに送信
5. **回転**: ターンテーブルを `STEP_INCREMENT`（25ハーフステップ ≈ 2.2°）だけ進め、PCからのACKを待ちます

### メモリ戦略

QVGA RGB565のフルフレームは153,600バイトで、128KB RAMには収まりません。そのため2回に分けて撮影します：

```
1回目: 行0–119をクロップ   → frame_buffer[19200] → 処理 → brightest[0..119]
2回目: 行120–239をクロップ → frame_buffer[19200] → 処理 → brightest[120..239]
```

両方とも同じ76.8KBバッファを使い回します。処理後は、コンパクトな480バイトの結果配列だけを送信します。

### UARTプロトコル

**同期フェーズ**（起動時）：
```
MCU: [0xAD 0xEE 0xEE 0xDE] [0xAD 0xEE 0xEE 0xDE] ...   (ACKが来るまで繰り返し)
PC:  0x17 (ACK)
```

**データフェーズ**（スキャン1回ごと、ACKで制御）：
```
MCU → PC:  brightest_pixels[240]  (480バイト, 行ごとにuint16_t)
MCU → PC:  totalStepCounter        (2バイト, uint16_t)
PC → MCU:  0x17 (ACK、次のスキャンを開始)
```

Rawフレームデータの送信はデフォルトで無効です（`DISABLE_FRAME_BUFFER_UART = 1`）。有効にすると、フレームごとに追加で153,600バイトのRGB565データが送信されます。

### ソースファイル

- `Core/Src/main.c` - メインループ：同期ハンドシェイク、ハーフフレーム撮影、最輝点検出、UART送受信、ステッパー制御
- `Core/Src/stepper.c` - 28BYJ-48ハーフステップドライバー（8状態シーケンス、4098ステップ/回転）
- `Core/Src/stm32f4xx_hal_msp.c` - ペリフェラルのピン/DMA/割り込みの低レベル設定
- `Core/Src/stm32f4xx_it.c` - 割り込みハンドラ（DCMI, DMA, UART, TIM1）
- `Drivers/OV7670/OV7670.c` - OV7670ドライバー：I2C経由のSCCBレジスタ設定、DCMI+DMA撮影管理
- `Drivers/OV7670/OV7670_REG.h` - OV7670レジスタ初期化テーブル（QVGA RGB565モード）
- `Drivers/OV7670/OV7670_math.c` - 各行の最も赤いピクセルの検出（レーザーライン検出）

### コンパイルフラグ

- `DISABLE_FRAME_BUFFER_UART` (デフォルト: `0`) - `1` に設定すると、RawフレームデータのUART送信をスキップします（115200 baudで約6.7秒/フレーム節約）

### 物理的なセットアップ

```
Camera ← b=100mm → Laser  α = 20° 内側に角度
    |
    |  ← d = 274.75mm
    |
Turntable  ← 半径75mm, 4098ステップ/回転
  Object
```

レーザーが既知の角度で物体にラインを照射します。カメラがそのラインを見て、STM32が各行で最も明るい（赤い）ピクセルを見つけます。ターンテーブルの回転角度と組み合わせることで、スキャン1回につき240個の3D点が得られ、その回転角度での物体の断面を描きます。

### リソース使用量

```
RAM:   79,328 / 131,072 バイト (60.5%)
Flash: 30,240 / 524,288 バイト (5.8%)
```

## クレジット

- OV7670カメラドライバーは https://github.com/PHANzgz/STM32-H7-camera-interface-and-display から取得

---

Embedded firmware for a laser-line 3D scanner built on the STM32F446RE. Captures images with an OV7670 camera, detects a red laser line in each row, rotates a turntable via stepper motor, and streams the results to a PC over UART for 3D reconstruction.

## Hardware

- **MCU**: STM32F446RE - Cortex-M4, 128KB RAM, 512KB Flash, 16MHz HSI
- **Camera**: OV7670 - QVGA (320×240) RGB565, 3.6mm focal length lens
- **Stepper**: 28BYJ-48 - Half-step mode, 4098 steps/revolution, ULN2003 driver
- **Laser**: Red line laser - Mounted at 20° angle from camera axis

## Pin Connections

### OV7670 Camera

- PA8 (MCO1) → XLK: 8MHz camera clock (HSI/2)
- PC5 → RET: Camera reset (active low)
- PB10 → SCL: I2C2 SCCB clock (100kHz)
- PC12 → SDA: I2C2 SCCB data
- PA4 → HS: DCMI HSYNC
- PB7 → VS: DCMI VSYNC
- PA6 → PCLK: DCMI pixel clock
- PC6–PC9, PC11, PB6, PB8–PB9 → D0–D7: DCMI 8-bit parallel data

### Stepper Motor (28BYJ-48)

- PC0 → IN1
- PC1 → IN2
- PC2 → IN3
- PC3 → IN4

### UART (PC Communication)

- PA2 → TX: USART2 transmit (115200 baud)
- PA3 → RX: USART2 receive (interrupt-driven)

## Building

Requires `arm-none-eabi-gcc` toolchain and CMake ≥ 3.22.

```bash
cmake -B build --preset Debug
cmake --build build
```

Flash the resulting `build/3d_scan_repo.elf` to the STM32F446RE.

## Architecture

### Scan Pipeline

```
OV7670 Camera (320x240) → DCMI + DMA (half-frame capture) → Brightest Pixel Detection (per row) → UART TX to PC
                                       |
                                       → Stepper Motor + Step Counter → UART TX to PC
```

1. **Capture**: OV7670 captures a 320×240 RGB565 image via DCMI with DMA transfer
2. **Half-frame**: Due to 128KB RAM, the image is captured in two 120-row halves using DCMI crop windowing into a shared 76.8KB buffer
3. **Detection**: For each row, finds the X-coordinate with the maximum red channel value (laser line position)
4. **Transmission**: Sends the 240 brightest-pixel positions (480 bytes) + stepper step counter (2 bytes) to PC over UART
5. **Rotation**: Advances the turntable by `STEP_INCREMENT` (25 half-steps ≈ 2.2°), then waits for PC ACK

### Memory Strategy

A full QVGA RGB565 frame is 153,600 bytes, too large for the 128KB RAM. The firmware captures in two passes:

```
Pass 1: crop rows 0–119   → frame_buffer[19200] → process → brightest[0..119]
Pass 2: crop rows 120–239 → frame_buffer[19200] → process → brightest[120..239]
```

Both halves reuse the same 76.8KB buffer. After processing, only the compact 480-byte result array is transmitted.

### UART Protocol

**Sync phase** (startup):
```
MCU: [0xAD 0xEE 0xEE 0xDE] [0xAD 0xEE 0xEE 0xDE] ...   (repeat until ACK)
PC:  0x17 (ACK)
```

**Data phase** (per scan iteration, ACK-gated):
```
MCU → PC:  brightest_pixels[240]  (480 bytes, uint16_t per row)
MCU → PC:  totalStepCounter        (2 bytes, uint16_t)
PC → MCU:  0x17 (ACK, triggers next scan)
```

Raw frame data transmission is disabled by default (`DISABLE_FRAME_BUFFER_UART = 1`). When enabled, an additional 153,600 bytes of RGB565 data are sent per frame.

### Source Files

- `Core/Src/main.c` - Main loop: sync handshake, half-frame capture, brightest pixel detection, UART TX/RX, stepper control
- `Core/Src/stepper.c` - 28BYJ-48 half-step driver (8-state sequence, 4098 steps/revolution)
- `Core/Src/stm32f4xx_hal_msp.c` - Low-level peripheral pin/DMA/interrupt configuration
- `Core/Src/stm32f4xx_it.c` - ISR dispatch (DCMI, DMA, UART, TIM1)
- `Drivers/OV7670/OV7670.c` - OV7670 driver: SCCB register config via I2C, DCMI+DMA capture management
- `Drivers/OV7670/OV7670_REG.h` - OV7670 register initialization table (QVGA RGB565 mode)
- `Drivers/OV7670/OV7670_math.c` - Per-row brightest red pixel detection (laser line finding)

### Compile Flags

- `DISABLE_FRAME_BUFFER_UART` (default: `0`) - Set to `1` to skip transmitting raw frame data over UART (saves ~6.7s per frame at 115200 baud)

### Physical Setup

```
Camera ← b=100mm → Laser  α = 20° angle inward
    |
    |  ← d = 274.75mm
    |
Turntable  ← 75mm radius, 4098 steps/revolution
  Object
```

The laser projects a line onto the object at a known angle. The camera sees this line, and the STM32 finds the brightest (reddest) pixel per row. Combined with the turntable rotation angle, each scan produces 240 3D points tracing the object's profile at that rotation step.

### Resource Usage

```
RAM:   79,328 / 131,072 bytes (60.5%)
Flash: 30,240 / 524,288 bytes (5.8%)
```

## Credits

- OV7670 camera driver obtained from https://github.com/PHANzgz/STM32-H7-camera-interface-and-display
