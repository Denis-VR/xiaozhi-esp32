# Получение Device ID и MAC Address устройства xiaozhi

## Описание

Скрипт `get_device_info.py` позволяет извлечь Device ID (UUID) и MAC address из логов устройства или указать их напрямую.

## Использование

### 1. Из лога файла

```bash
python3 scripts/get_device_info.py < log_file.txt
```

### 2. Из вывода idf.py monitor (pipe)

```bash
idf.py monitor | python3 scripts/get_device_info.py
```

### 3. Прямой ввод значений

```bash
python3 scripts/get_device_info.py --uuid f0a8ee71-9069-49b3-8ecf-f5bf173933a7 --mac 90:e5:b1:c9:f4:20
```

### 4. Из файла лога (опция --file)

```bash
python3 scripts/get_device_info.py --file path/to/log_file.txt
```

## Где найти информацию в логах

### Device ID (UUID)
Ищите строку вида:
```
I (249) Board: UUID=f0a8ee71-9069-49b3-8ecf-f5bf173933a7 SKU=echoear
```

### MAC Address
Ищите строку вида:
```
I (789) wifi:mode : sta (90:e5:b1:c9:f4:20)
```

## Пример вывода

```
============================================================
Информация об устройстве xiaozhi
============================================================
Device ID (UUID): f0a8ee71-9069-49b3-8ecf-f5bf173933a7
MAC Address:      90:e5:b1:c9:f4:20
============================================================

Для использования в API:
  Client-Id: f0a8ee71-9069-49b3-8ecf-f5bf173933a7
  Device-Id: 90:e5:b1:c9:f4:20

Для получения кода активации:
  python3 scripts/get_activation_code.py f0a8ee71-9069-49b3-8ecf-f5bf173933a7
```

## Использование в API

При работе с API xiaozhi используйте:
- **Client-Id**: UUID устройства (Device ID)
- **Device-Id**: MAC адрес устройства

Эти значения используются в HTTP заголовках при запросах к серверу OTA.

## Связанные скрипты

- `get_activation_code.py` - получение кода активации по UUID

