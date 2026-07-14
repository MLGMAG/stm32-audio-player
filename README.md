# STM32 Audio Player

A WAV file audio player for the STM32F411CEU6 microcontroller. Reads tracks from an SD card, streams them over I2S, and shows the playlist on an SSD1306 OLED display. Built on FreeRTOS.

## Features

- Plays PCM WAV files (44.1 kHz, 16-bit, stereo) from an SD card
- Dual-buffer I2S DMA streaming for gapless, interrupt-safe audio output
- SSD1306 OLED display showing the current playlist and track position
- Single-button control: single click to pause/resume, double click to skip
- Playlist loops back to the first track after the last one finishes
- Supports up to 10 tracks

## Hardware

| Peripheral | Interface | Pins |
|------------|-----------|------|
| Audio output (I2S codec/amp) | I2S1 | PA4 (WS), PA5 (CK), PA7 (SD) |
| SD card | SPI2 | PB10 (SCK), PB14 (MISO), PB15 (MOSI), PB13 (CS) |
| OLED display (SSD1306 128×64) | I2C1 | PB6 (SCL), PB7 (SDA) |
| Control button | GPIO EXTI | PB12 (active-low, internal pull-up) |

**MCU:** STM32F411CEU6 — Cortex-M4 at 100 MHz, 512 KB flash, 128 KB RAM.

## SD Card Setup

Format the SD card as FAT32 and create a `music/` directory at the root. Place WAV files inside it:

```
music/
  01-track.wav
  02-track.wav
  ...
```

**Required WAV format:** PCM (uncompressed), stereo, 44100 Hz, 16-bit per channel. Files not matching this format are silently skipped during playlist loading. Filenames must fit within 63 characters.

## Button Controls

| Action | Result |
|--------|--------|
| Single click | Pause / Resume |
| Double click | Skip to next track |

Double-click is detected when two presses occur within 300 ms of each other. After the last track finishes, playback loops back to the first track.

## Building and Flashing

**Prerequisites:** GNU Tools for STM32 (`arm-none-eabi-gcc`), GNU Make.

```bash
# Build
cd Debug && make all

# Flash via ST-Link
st-flash write Debug/stm32-audio-player.bin 0x08000000
```

Build artifacts are placed in `Debug/`: `.elf`, `.hex`, `.bin`, `.map`, and `.list`.

The project can also be built and flashed directly from STM32CubeIDE using the `.project` / `.cproject` configuration files.

## Architecture

The application runs three FreeRTOS tasks:

```
BUTTON_TASK  ──[MANAGER_QUEUE]──▶  MANAGER_TASK  ──[PLAYER_QUEUE]──▶  PLAYER_TASK
(GPIO poll,                         (playlist,                          (WAV stream,
 debounce,                           pause/skip,                         I2S DMA)
 ISR handoff)                        display)
```

- **MANAGER_TASK** (highest priority) owns the SD card mount, playlist state, and display. It interprets button events and sends `UAL_TRACK_MSG_t` messages to the player queue when a track should start or resume.
- **PLAYER_TASK** (lowest priority) blocks on the player queue. When a message arrives it opens the WAV file at the given byte offset and streams it using a double-buffer scheme: one buffer transmits via I2S DMA while the other is filled from SD card.
- **BUTTON_TASK** waits on a semaphore released by the EXTI ISR, then polls for a stable button state and posts `UAL_MANAGER_MSG_TYPE_BUTTON_PRESS` events to the manager queue.

Pause is implemented by stopping the I2S DMA and recording the remaining byte count, allowing resume from the exact position.
