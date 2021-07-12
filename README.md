# SeedBank
Open Source Seed Bank for seed storage.

## Libraries in use

- ESPAsyncWebServer, Wifi, WifiClient,SPIFFS for Asyncronous webserver
- Adafruit_GFX, Adafruit_ST7789 for display
- Adafruit_BMP280 sensor temperature
- [ArduinoJson](https://arduinojson.org)

## How to upload data directory to SPIFFS

- [With official ide](https://github.com/me-no-dev/arduino-esp32fs-plugin)

## Instalation

We use [PlatformIO](https://platformio.org/). Have two methods of work

- [CLI](https://platformio.org/install/cli)
- [IDE, integrated in vscode](https://platformio.org/install/ide?install=vscode)

### Compile & Upload(instructions for cli)

```bash
pio run
pio run -t upload

pio run -t buildfs
pio run -t uploadfs
```

### Update by OTA

- Compile the firmware

    ```bash
    pio run
    ```

- File in .pio/build/{board-name}/firmware.bin
- Build the file fs

    ```bash
    pio run -t buildfs
    ```

- File in .pio/build/{board-name}/spiffs.bin
- Upload in page http://{ip}/update

## TODO

    * Create PID or similar control
    * Communication with server for save logs
    * Send alerts
    * Instalation instructions.
