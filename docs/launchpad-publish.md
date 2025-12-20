# Публикация прошивки на ESP Launchpad

Это руководство поможет вам опубликовать вашу прошивку XiaoZhi AI на ESP Launchpad, чтобы другие пользователи могли легко прошить её на свои устройства.

## Шаг 1: Подготовка прошивки

### 1.1 Сборка прошивки

Соберите прошивку для нужного чипсета:

```bash
# Для ESP32-S3
idf.py set-target esp32s3
idf.py build

# Создайте объединённый бинарный файл
idf.py merge-bin -o xiaozhi_esp32s3_ru.bin
```

### 1.2 Проверка файлов

Убедитесь, что у вас есть:
- `merged-binary.bin` или созданный вами объединённый файл
- Все необходимые компоненты включены в бинарный файл

## Шаг 2: Создание TOML конфигурации

1. Откройте файл `launchpad_xiaozhi.toml` в корне проекта
2. Настройте следующие параметры:

   - **firmware_images_url**: URL, где будут размещены ваши бинарные файлы
   - **supported_apps**: Список версий прошивки
   - Для каждой версии укажите:
     - **chipsets**: Поддерживаемые чипсеты (ESP32, ESP32-S3, ESP32-C3, ESP32-P4)
     - **image.esp32-xxx**: Имя файла прошивки для каждого чипсета
     - **android_app_url** и **ios_app_url** (опционально): Ссылки на мобильные приложения

### Пример конфигурации:

```toml
esp_toml_version = 1.0
firmware_images_url = "https://github.com/yourusername/xiaozhi-esp32/releases/download/v2.0.0/"

supported_apps = [
    "xiaozhi_esp32s3_v2_0_0"
]

[xiaozhi_esp32s3_v2_0_0]
chipsets = ["ESP32-S3"]
image.esp32-s3 = "xiaozhi_esp32s3_v2_0_0.bin"
android_app_url = ""
ios_app_url = ""
```

## Шаг 3: Размещение файлов в интернете

Вам нужно разместить два типа файлов:

### 3.1 TOML конфигурационный файл

**⚠️ ВАЖНО: Проблема CORS**

GitHub Releases **не поддерживает CORS**, поэтому ESP Launchpad не может загрузить TOML файл напрямую из Releases. Используйте один из следующих способов:

#### ✅ Рекомендуемый способ: Разместить TOML в репозитории

1. **Добавьте TOML файл в репозиторий:**
   - Поместите `launchpad_xiaozhi.toml` в корень репозитория или в папку `docs/`
   - Закоммитьте и запушьте в репозиторий

2. **Используйте raw.githubusercontent.com:**
   ```
   https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml
   ```
   Или для конкретной ветки/тега:
   ```
   https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/v3.0.0_ru/launchpad_xiaozhi.toml
   ```

3. **Альтернатива: jsDelivr CDN:**
   ```
   https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/launchpad_xiaozhi.toml
   ```

#### Другие варианты:

- **GitHub Pages**: Разместите в ветке `gh-pages` или через GitHub Pages
- **Собственный веб-сервер**: Загрузите на ваш хостинг с поддержкой CORS
- **CDN с поддержкой CORS**: Используйте любой CDN, который добавляет CORS заголовки

### 3.2 Бинарные файлы прошивки

**⚠️ ВАЖНО: Проблема CORS с бинарными файлами**

GitHub Releases **не поддерживает CORS** для бинарных файлов, поэтому ESP Launchpad не может загрузить их напрямую. Используйте один из следующих способов:

#### ✅ Рекомендуемый способ: Разместить бинарные файлы в репозитории

1. **Создайте папку для прошивок:**
   ```bash
   mkdir -p firmware
   ```

2. **Скопируйте бинарный файл в папку:**
   ```bash
   cp build/xiaozhi_esp32s3_ru.bin firmware/v3.0.0_ru.bin
   ```

3. **Закоммитьте и запушьте:**
   ```bash
   git add firmware/
   git commit -m "Add firmware binaries for ESP Launchpad"
   git push
   ```

4. **Используйте jsDelivr CDN в TOML:**
   ```toml
   firmware_images_url = "https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/"
   ```

#### Альтернативный способ: GitHub Releases (только для ручной загрузки)

Если вы хотите использовать GitHub Releases для хранения файлов (но не для ESP Launchpad), вы можете:

**Примечание:** GitHub Releases можно использовать для хранения файлов для ручной загрузки пользователями, но для ESP Launchpad нужно использовать файлы в репозитории с jsDelivr.

#### Формат URL для jsDelivr:

Базовый URL для файлов в репозитории:
```
https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/
```

Конкретные файлы будут доступны по адресам:
```
https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/v3.0.0_ru.bin
https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/launchpad_xiaozhi.toml
```

**Важно:** Замените:
- `ваш-username` на ваш GitHub username
- `xiaozhi-esp32` на название вашего репозитория
- `main` на вашу ветку (или используйте тег, например `@v3.0.0_ru`)

#### Пример настройки TOML файла:

После размещения файлов в репозитории обновите `launchpad_xiaozhi.toml`:

```toml
esp_toml_version = 1.0
firmware_images_url = "https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/"

supported_apps = [
    "xiaozhi_esp32s3_v3_0_0_ru"
]

[xiaozhi_esp32s3_v3_0_0_ru]
chipsets = ["ESP32-S3"]
image.esp32-s3 = "v3.0.0_ru.bin"
android_app_url = ""
ios_app_url = ""
```

#### Проверка доступности файлов:

После загрузки файлов в репозиторий проверьте, что файлы доступны:
- **Бинарный файл**: Откройте в браузере URL файла через jsDelivr (например, `https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/v3.0.0_ru.bin`)
- Должен начаться автоматический скачивание файла
- **TOML файл**: Проверьте доступность через raw.githubusercontent.com или jsDelivr (например, `https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml`)
- Убедитесь, что файлы закоммичены и запушены в репозиторий

## Шаг 4: Тестирование конфигурации

1. Откройте ESP Launchpad: https://espressif.github.io/esp-launchpad/
2. Добавьте параметр `flashConfigURL` с URL вашего TOML файла:
   
   **Если TOML в репозитории:**
   ```
   https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml
   ```
   
   **Или через jsDelivr:**
   ```
   https://espressif.github.io/esp-launchpad/?flashConfigURL=https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/launchpad_xiaozhi.toml
   ```
   
   **⚠️ НЕ используйте GitHub Releases для TOML файла** - будет ошибка CORS!

3. Проверьте, что прошивка отображается в списке доступных приложений
4. Откройте консоль браузера (F12) и проверьте отсутствие ошибок CORS
5. Протестируйте прошивку на реальном устройстве

## Шаг 5: Добавление кнопки на ваш сайт

После успешного тестирования добавьте кнопку "Попробовать с ESP Launchpad" на ваш сайт или в README:

```html
<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml">
    <img alt="Try it with ESP Launchpad" 
         src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" 
         width="250" 
         height="70">
</a>
```

Или в Markdown:

```markdown
[![Try it with ESP Launchpad](https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png)](https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml)
```

**Пример для вашего репозитория:**
```markdown
[![Try it with ESP Launchpad](https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png)](https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/Denis-VR/xiaozhi-esp32/main/launchpad_xiaozhi.toml)
```

## Важные замечания

1. **Версионирование**: Используйте понятные имена версий в формате `xiaozhi_chipset_version` (например, `xiaozhi_esp32s3_v2_0_0`)

2. **Поддержка нескольких чипсетов**: Вы можете добавить несколько версий для разных чипсетов в один TOML файл

3. **Обновления**: При выпуске новой версии:
   - Добавьте новую запись в `supported_apps`
   - Создайте новую секцию `[version_name]` с конфигурацией
   - Загрузите новый бинарный файл

4. **Безопасность**: Убедитесь, что ваши бинарные файлы не содержат чувствительных данных (ключи API, пароли и т.д.)

5. **Размер файлов**: Учитывайте ограничения хостинга. Для больших файлов рассмотрите использование CDN

## Пример полного процесса

```bash
# 1. Сборка прошивки
idf.py set-target esp32s3
idf.py build
idf.py merge-bin -o echoear-3.0-ru.bin

# 2. Создайте папку для прошивок и добавьте файлы в репозиторий
mkdir -p firmware
cp build/xiaozhi_esp32s3_ru.bin firmware/v3.0.0_ru.bin
git add launchpad_xiaozhi.toml firmware/
git commit -m "Add ESP Launchpad configuration and firmware binaries"
git push

# 3. Обновите TOML файл с правильными URL (используя jsDelivr)
# Откройте launchpad_xiaozhi.toml и установите:
# firmware_images_url = "https://cdn.jsdelivr.net/gh/ваш-username/xiaozhi-esp32@main/firmware/"
# 
# Закоммитьте обновлённый TOML файл:
git add launchpad_xiaozhi.toml
git commit -m "Update ESP Launchpad config with jsDelivr URLs"
git push

# 4. Протестируйте ссылку
# https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/ваш-username/xiaozhi-esp32/main/launchpad_xiaozhi.toml
```

### Альтернативный способ через GitHub CLI (gh):

Если у вас установлен GitHub CLI, вы можете создать релиз из командной строки:

```bash
# Установите GitHub CLI (если ещё не установлен)
# macOS: brew install gh
# Linux: см. https://cli.github.com/manual/installation

# Авторизуйтесь
gh auth login

# Создайте релиз и загрузите файлы
gh release create v2.0.0-ru \
  --title "XiaoZhi AI v2.0.0 (Russian)" \
  --notes "Русская версия прошивки XiaoZhi AI" \
  xiaozhi_esp32s3_ru.bin \
  launchpad_xiaozhi.toml
```

## Дополнительные ресурсы

- [ESP Launchpad Documentation](https://github.com/espressif/esp-launchpad)
- [Пример конфигурации EchoEar](https://dl.espressif.com/AE/esp-factory-images/launchpad_EchoEar_xiaozhi.toml)

