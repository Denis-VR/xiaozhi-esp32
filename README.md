# Чатбот на базе MCP

([English](README_en.md))


## Введение

Чатбот XiaoZhi AI на базе голосового взаимодействия использует возможности ИИ больших моделей, таких как Qwen / DeepSeek, и реализует управление с нескольких терминалов через протокол MCP.

<img src="docs/mcp-based-graph.jpg" alt="Control everything via MCP" width="320">

## Заметки о версиях

Текущая версия v2 несовместима с таблицей разделов v1, поэтому обновление с v1 на v2 через OTA невозможно. Подробности о таблице разделов см. в [partitions/v2/README.md](partitions/v2/README.md).

Все устройства на v1 можно обновить до v2 путём ручной прошивки firmware.

Стабильная версия v1 — 1.9.2. Переключиться на v1 можно командой `git checkout v1`. Ветка v1 будет поддерживаться до февраля 2026 года.

### Реализованные функции

- Wi-Fi / ML307 Cat.1 4G
- Офлайн-пробуждение голосом [ESP-SR](https://github.com/espressif/esp-sr)
- Поддержка двух протоколов связи ([Websocket](docs/websocket.md) или MQTT+UDP)
- Аудиокодек OPUS
- Голосовое взаимодействие на базе потокового ASR + LLM + TTS
- Распознавание спикера, идентификация текущего говорящего [3D Speaker](https://github.com/modelscope/3D-Speaker)
- Дисплей OLED / LCD, поддержка отображения эмодзи
- Отображение батареи и управление питанием
- Поддержка нескольких языков (китайский, английский, японский)
- Поддержка чипов ESP32-C3, ESP32-S3, ESP32-P4
- MCP на стороне устройства для управления (динамик, LED, серво, GPIO и т.д.)
- MCP на стороне облака для расширения возможностей больших моделей (управление умным домом, работа с ПК, поиск знаний, email и т.д.)
- Настраиваемые слова пробуждения, шрифты, эмодзи и фоны чата с онлайн-редактором на веб ([Custom Assets Generator](https://github.com/78/xiaozhi-assets-generator))

## Аппаратная часть

### DIY на макетной плате

См. руководство в документе Feishu:

👉 ["Энциклопедия чатбота XiaoZhi AI"](https://ccnphfhqs21z.feishu.cn/wiki/F5krwD16viZoF0kKkvDcrZNYnhb?from=from_copylink)

Демо на макетной плате:



### Поддержка 70+ открытых аппаратных плат (частичный список)

- <a href="https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-shi-zhan-pai-esp32-s3-kai-fa-ban" target="_blank" title="Плата разработки LiChuang ESP32-S3">Плата разработки LiChuang ESP32-S3</a>
- <a href="https://github.com/espressif/esp-box" target="_blank" title="Espressif ESP32-S3-BOX3">Espressif ESP32-S3-BOX3</a>
- <a href="https://docs.m5stack.com/zh_CN/core/CoreS3" target="_blank" title="M5Stack CoreS3">M5Stack CoreS3</a>
- <a href="https://docs.m5stack.com/en/atom/Atomic%20Echo%20Base" target="_blank" title="AtomS3R + Echo Base">M5Stack AtomS3R + Echo Base</a>
- <a href="https://gf.bilibili.com/item/detail/1108782064" target="_blank" title="Magic Button 2.4">Magic Button 2.4</a>
- <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.8.htm" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">Waveshare ESP32-S3-Touch-AMOLED-1.8</a>
- <a href="https://github.com/Xinyuan-LilyGO/T-Circle-S3" target="_blank" title="LILYGO T-Circle-S3">LILYGO T-Circle-S3</a>
- <a href="https://oshwhub.com/tenclass01/xmini_c3" target="_blank" title="XiaGe Mini C3">XiaGe Mini C3</a>
- <a href="https://oshwhub.com/movecall/cuican-ai-pendant-lights-up-y" target="_blank" title="Movecall CuiCan ESP32S3">Подвеска CuiCan AI</a>
- <a href="https://github.com/WMnologo/xingzhi-ai" target="_blank" title="WMnologo-Xingzhi-1.54">WMnologo-Xingzhi-1.54TFT</a>
- <a href="https://www.seeedstudio.com/SenseCAP-Watcher-W1-A-p-5979.html" target="_blank" title="SenseCAP Watcher">SenseCAP Watcher</a>
- <a href="https://www.bilibili.com/video/BV1BHJtz6E2S/" target="_blank" title="ESP-HI Low Cost Robot Dog">Робот-собака ESP-HI низкой стоимости</a>

<div style="display: flex; justify-content: space-between;">
  <a href="docs/v1/lichuang-s3.jpg" target="_blank" title="Плата разработки LiChuang ESP32-S3">
    <img src="docs/v1/lichuang-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/espbox3.jpg" target="_blank" title="Espressif ESP32-S3-BOX3">
    <img src="docs/v1/espbox3.jpg" width="240" />
  </a>
  <a href="docs/v1/m5cores3.jpg" target="_blank" title="M5Stack CoreS3">
    <img src="docs/v1/m5cores3.jpg" width="240" />
  </a>
  <a href="docs/v1/atoms3r.jpg" target="_blank" title="AtomS3R + Echo Base">
    <img src="docs/v1/atoms3r.jpg" width="240" />
  </a>
  <a href="docs/v1/magiclick.jpg" target="_blank" title="Magic Button 2.4">
    <img src="docs/v1/magiclick.jpg" width="240" />
  </a>
  <a href="docs/v1/waveshare.jpg" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">
    <img src="docs/v1/waveshare.jpg" width="240" />
  </a>
  <a href="docs/v1/lilygo-t-circle-s3.jpg" target="_blank" title="LILYGO T-Circle-S3">
    <img src="docs/v1/lilygo-t-circle-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/xmini-c3.jpg" target="_blank" title="XiaGe Mini C3">
    <img src="docs/v1/xmini-c3.jpg" width="240" />
  </a>
  <a href="docs/v1/movecall-cuican-esp32s3.jpg" target="_blank" title="CuiCan">
    <img src="docs/v1/movecall-cuican-esp32s3.jpg" width="240" />
  </a>
  <a href="docs/v1/wmnologo_xingzhi_1.54.jpg" target="_blank" title="WMnologo-Xingzhi-1.54">
    <img src="docs/v1/wmnologo_xingzhi_1.54.jpg" width="240" />
  </a>
  <a href="docs/v1/sensecap_watcher.jpg" target="_blank" title="SenseCAP Watcher">
    <img src="docs/v1/sensecap_watcher.jpg" width="240" />
  </a>
  <a href="docs/v1/esp-hi.jpg" target="_blank" title="ESP-HI Low Cost Robot Dog">
    <img src="docs/v1/esp-hi.jpg" width="240" />
  </a>
</div>

## Программная часть

### Прошивка firmware

Новичкам рекомендуется использовать готовую прошивку без настройки среды разработки.

Прошивка по умолчанию подключается к официальному серверу [xiaozhi.me](https://xiaozhi.me). Личные пользователи могут зарегистрировать аккаунт для бесплатного использования модели Qwen в реальном времени.

👉 [Руководство по прошивке для новичков](https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS)

### Среда разработки

- Cursor или VSCode
- Установите плагин ESP-IDF, выберите версию SDK 5.4 или выше
- Linux предпочтительнее Windows для более быстрой компиляции и меньшего количества проблем с драйверами
- Проект использует стиль кода Google C++, соблюдайте его при отправке кода

### Документация для разработчиков

- [Руководство по кастомным платам](docs/custom-board.md) — узнайте, как создавать кастомные платы для XiaoZhi AI
- [Использование MCP для IoT-управления](docs/mcp-usage.md) — узнайте, как управлять IoT-устройствами через протокол MCP
- [Схема взаимодействия протокола MCP](docs/mcp-protocol.md) — реализация протокола MCP на стороне устройства
- [Документация протокола MQTT + UDP](docs/mqtt-udp.md)
- [Подробная документация протокола WebSocket](docs/websocket.md)

## Настройка больших моделей

Если у вас уже есть устройство чатбота XiaoZhi AI и оно подключено к официальному серверу, вы можете войти в консоль [xiaozhi.me](https://xiaozhi.me) для настройки.

👉 [Видеоурок по работе с бэкендом (старая версия интерфейса)](https://www.bilibili.com/video/BV1jUCUY2EKM/)

## Связанные открытые проекты

Для развертывания сервера на ПК обратитесь к следующим open-source проектам:

- [xinnan-tech/xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) Сервер на Python
- [joey-zhou/xiaozhi-esp32-server-java](https://github.com/joey-zhou/xiaozhi-esp32-server-java) Сервер на Java
- [AnimeAIChat/xiaozhi-server-go](https://github.com/AnimeAIChat/xiaozhi-server-go) Сервер на Golang

Другие клиентские проекты с протоколом связи XiaoZhi:

- [huangjunsen0406/py-xiaozhi](https://github.com/huangjunsen0406/py-xiaozhi) Клиент на Python
- [TOM88812/xiaozhi-android-client](https://github.com/TOM88812/xiaozhi-android-client) Клиент для Android
- [100askTeam/xiaozhi-linux](http://github.com/100askTeam/xiaozhi-linux) Клиент для Linux от 100ask
- [78/xiaozhi-sf32](https://github.com/78/xiaozhi-sf32) Прошивка для Bluetooth-чипа от Sichuan
- [QuecPython/solution-xiaozhiAI](https://github.com/QuecPython/solution-xiaozhiAI) Прошивка QuecPython от Quectel

Инструменты для кастомных ассетов:

- [78/xiaozhi-assets-generator](https://github.com/78/xiaozhi-assets-generator) Генератор кастомных ассетов (слова пробуждения, шрифты, эмодзи, фоны)

## О проекте

Это открытый проект на ESP32 под лицензией MIT, который можно использовать бесплатно, включая коммерческие цели.

Проект помогает понять разработку аппаратного ИИ и применять быстро развивающиеся большие языковые модели на реальных устройствах.

Если есть идеи или предложения, создавайте Issues или присоединяйтесь к QQ-группе: 1011329060

## История звёзд

<a href="https://star-history.com/#78/xiaozhi-esp32&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
 </picture>
</a>

Sources
