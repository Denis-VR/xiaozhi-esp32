# EchoEar 喵伴

## Введение

<div align="center">
    <a href="https://oshwhub.com/esp-college/echoear"><b> Платформа LCSC Open Source </b></a>
</div>

EchoEar 喵伴 — интеллектуальный AI‑набор на базе модуля ESP32-S3-WROOM-1. Плата оснащена круговым QSPI‑тачскрином 1,85", массивом из двух микрофонов, поддерживает офлайн‑пробуждение по голосу и алгоритмы определения направления звука. Подробнее о железе можно узнать на [странице проекта LCSC](https://oshwhub.com/esp-college/echoear).

## Настройка и сборка

**Выбор целевого чипа ESP32-S3**

```bash
idf.py set-target esp32s3
```

**Открытие menuconfig**

```bash
idf.py menuconfig
```

Далее последовательно настройте параметры.

### Базовые настройки
- `Xiaozhi Assistant` → `Board Type` → выбрать `EchoEar`

### Выбор стиля UI

EchoEar поддерживает несколько вариантов отображения интерфейса, переключаемых через menuconfig:
- `Xiaozhi Assistant` → `Select display style` → выбрать нужный стиль

#### Доступные стили

##### Эмоциональные анимации (Emote animation style) — рекомендовано
- **Опция**: `USE_EMOTE_MESSAGE_STYLE`
- **Описание**: применяет систему `EmoteDisplay`
- **Возможности**: богатые анимации эмоций, движение глаз, иконки состояния
- **Сценарий**: голосовой ассистент с выразительным UI
- **Класс**: `emote::EmoteDisplay`

**⚠️ Важно**: для этого стиля требуется указать собственный набор ресурсов:
1. `Xiaozhi Assistant` → `Flash Assets` → `Flash Custom Assets`
2. `Xiaozhi Assistant` → `Custom Assets File` → URL ресурса:
   ```
   https://dl.espressif.com/AE/wn9_nihaoxiaozhi_tts-font_puhui_common_20_4-echoear.bin
   ```

##### Стандартный стиль сообщений (Default message style)
- **Опция**: `USE_DEFAULT_MESSAGE_STYLE` (по умолчанию)
- **Описание**: классический экран с текстом и иконками
- **Класс**: `SpiLcdDisplay`

##### Стиль WeChat (WeChat message style)
- **Опция**: `USE_WECHAT_MESSAGE_STYLE`
- **Описание**: интерфейс в стиле мессенджера WeChat
- **Применение**: для пользователей, предпочитающих подобный дизайн
- **Класс**: `SpiLcdDisplay`

> **Примечание**: EchoEar оснащён флешем 16 МБ, поэтому требуется специальная таблица разделов для распределения OTA, приложений и ресурсов.

Сохраните настройки (`S`), выйдите (`Q`).

**Сборка**

```bash
idf.py build
```

**Прошивка**

Подключите EchoEar к компьютеру, **включите питание** и выполните:

```bash
idf.py flash
```
