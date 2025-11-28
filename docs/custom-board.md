# Руководство по созданию пользовательской платы

В этом документе описывается, как подготовить собственный профиль платы для голосового робота Xiaozhi. Проект поддерживает более 70 плат на ESP32, каждая из которых имеет отдельную директорию с инициализацией.

## Важное предупреждение

> **Нельзя** просто переписывать конфигурацию существующей платы, если у вас другая разводка IO. Создайте новую плату (уникальный `name`) или настройте `config.json` так, чтобы разные билды имели разные `name` и набор `sdkconfig`‑макросов. Используйте `python scripts/release.py [имя каталога платы]` для сборки/упаковки.
>
> Если перезаписать штатную конфигурацию, OTA‑обновления вернут официальную прошивку и устройство перестанет работать. Каждая плата имеет уникальный идентификатор и собственный канал обновлений, его необходимо сохранить.

## Структура каталога

Типовой набор файлов:

- `xxx_board.cc` — основная инициализация и логика платы
- `config.h` — пины, параметры аудио, дисплея и т. д.
- `config.json` — описание целей сборки
- `README.md` — заметки по конкретной плате

## Алгоритм создания

### 1. Каталог платы

Создайте каталог в `boards/`, например `main/boards/my-custom-board`.

### 2. Конфигурационные файлы

#### config.h

Определите все параметры оборудования:

- Аудиосэмплинг и пины I2S
- I2C‑пины и адреса кодека
- Кнопки и светодиоды
- Параметры дисплея

Пример (из `lichuang-c3-dev`):

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Аудио
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_8
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_13
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// Кнопки
#define BOOT_BUTTON_GPIO        GPIO_NUM_9

// Дисплей
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_5
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_4

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
```

#### config.json

Файл используют скрипты `scripts/release.py`:

```json
{
    "target": "esp32s3",
    "builds": [
        {
            "name": "my-custom-board",
            "sdkconfig_append": [
                "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
                "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
            ]
        }
    ]
}
```

**Пояснения:**
- `target` — целевой чип (esp32, esp32s3, esp32c3, esp32c6, esp32p4 ...)
- `name` — имя пакета прошивки (желательно совпадает с названием каталога)
- `sdkconfig_append` — массив строк, добавляемых к дефолтному sdkconfig

**Полезные параметры:**

```json
"CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y"
"CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y"
"CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y"

"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/4m.csv\""
"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/16m.csv\""

"CONFIG_LANGUAGE_EN_US=y"
"CONFIG_LANGUAGE_ZH_CN=y"

"CONFIG_USE_DEVICE_AEC=y"
"CONFIG_WAKE_WORD_DISABLED=y"
```

### 3. Код инициализации

Создайте `my_custom_board.cc` и реализуйте класс платы.

Основные части:

1. Наследование от `WifiBoard` или `Ml307Board`
2. Инициализация I2C, дисплея, кнопок, IoT
3. Переопределение виртуальных методов (`GetAudioCodec()`, `GetDisplay()`, `GetBacklight()` и др.)
4. Регистрация через макрос `DECLARE_BOARD`

```cpp
#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>

#define TAG "MyCustomBoard"

class MyCustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    void InitializeI2c() {
        i2c_master_bus_config_t cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = { .enable_internal_pullup = 1 },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &codec_i2c_bus_));
    }

    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    void InitializeDisplay() {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;

        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 2;
        io_config.pclk_hz = 80 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = GPIO_NUM_NC;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));

        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, true);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);

        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y,
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

    void InitializeTools() {
        // Регистрируйте MCP‑инструменты здесь
    }

public:
    MyCustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeDisplay();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->SetBrightness(100);
    }

    AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec codec(
            codec_i2c_bus_,
            I2C_NUM_0,
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK,
            AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS,
            AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN,
            AUDIO_CODEC_ES8311_ADDR);
        return &codec;
    }

    Display* GetDisplay() override {
        return display_;
    }

    Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

DECLARE_BOARD(MyCustomBoard);
```

### 4. Настройки сборки

#### Kconfig.projbuild

Добавьте пункт в `choice BOARD_TYPE`:

```kconfig
choice BOARD_TYPE
    prompt "Board Type"
    default BOARD_TYPE_BREAD_COMPACT_WIFI

    config BOARD_TYPE_MY_CUSTOM_BOARD
        bool "My Custom Board (моя плата)"
        depends on IDF_TARGET_ESP32S3
endchoice
```

- Название конфигурации пишется заглавными буквами
- `depends on` должен совпадать с вашим чипом

#### main/CMakeLists.txt

Вставьте новую ветку:

```cmake
elseif(CONFIG_BOARD_TYPE_MY_CUSTOM_BOARD)
    set(BOARD_TYPE "my-custom-board")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
endif()
```

**Выбор шрифтов и эмодзи:**

- OLED 128x64 → `font_puhui_basic_14_1`
- 240x240 → `font_puhui_basic_16_4`
- 240x320 → `font_puhui_basic_20_4`
- 480x320 и выше → `font_puhui_basic_30_4`

Эмодзи:
- `twemoji_32` — маленькие экраны
- `twemoji_64` — большие

### 5. Сборка и прошивка

#### Метод 1: `idf.py`

1. Выбор цели:
   ```bash
   idf.py set-target esp32s3
   ```
2. Полная очистка: `idf.py fullclean`
3. Настройки: `idf.py menuconfig` → `Xiaozhi Assistant -> Board Type`
4. Сборка/прошивка: `idf.py build && idf.py flash monitor`

#### Метод 2: `release.py` (рекомендуется)

```bash
python scripts/release.py my-custom-board
```

Скрипт:
- Выбирает целевой чип из `config.json`
- Применяет `sdkconfig_append`
- Собирает и пакует прошивку

### 6. README

Добавьте описание железа, требований, инструкций по сборке и прошивке.

## Часто используемые компоненты

### 1. Дисплеи

- ST7789 (SPI)
- ILI9341 (SPI)
- SH8601 (QSPI)
- и др.

### 2. Аудиокодеки

- ES8311
- ES7210
- AW88298
- и др.

### 3. PMIC

- AXP2101 и другие контроллеры питания

### 4. MCP‑инструменты

Можно добавить функции для:
- Speaker
- Screen
- Battery
- Light
- и т. д.

## Иерархия классов плат

- `Board`
  - `WifiBoard`
  - `Ml307Board`
  - `DualNetworkBoard`

## Советы

1. Изучите похожую плату и используйте её как шаблон
2. Реализуйте функциональность поэтапно
3. Тщательно проверьте привязку пинов в `config.h`
4. Убедитесь в совместимости драйверов и микросхем

## Возможные проблемы

1. **Дисплей не работает** — проверьте SPI, зеркала, инверсию цветов
2. **Нет аудио** — проверьте I2S, PA, адрес кодека
3. **Wi‑Fi не подключается** — проверьте учётные данные
4. **Нет связи с сервером** — перепроверьте настройки MQTT/WebSocket

## Полезные ссылки

- ESP-IDF: https://docs.espressif.com/projects/esp-idf/
- LVGL: https://docs.lvgl.io/
- ESP-SR: https://github.com/espressif/esp-sr
