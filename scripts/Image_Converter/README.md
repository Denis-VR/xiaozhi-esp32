# Конвертер изображений для LVGL

Каталог содержит два скрипта для подготовки изображений под формат LVGL.

## 1. `LVGLImage.py`

Оригинальный скрипт из [официального репозитория LVGL](https://github.com/lvgl/lvgl/tree/master/scripts), используемый для преобразования изображений.

## 2. GUI‑утилита `lvgl_tools_gui.py`

Графическая оболочка, вызывающая `LVGLImage.py` и позволяющая пакетно конвертировать изображения. Подходит, например, для изменения стандартных эмоций Xiaozhi — руководство доступно [по ссылке](https://www.bilibili.com/video/BV12FQkYeEJ3/).

### Особенности

- Дружественный графический интерфейс
- Пакетная обработка файлов
- Автоопределение формата исходника и подбор оптимального цветового формата
- Поддержка разных разрешений

### Как использовать

Создайте виртуальное окружение:
```bash
python -m venv venv
source venv/bin/activate    # Linux/Mac
venv\Scripts\activate      # Windows
```

Установите зависимости:
```bash
pip install -r requirements.txt
```

Запустите конвертер:
```bash
source venv/bin/activate    # Linux/Mac
venv\Scripts\activate      # Windows
python lvgl_tools_gui.py
```
