# Обновление прошивки

В этой версии исправлена проблема отображения пробелов на экране устройства Echoear и улучшены аудио-ассеты на русском языке.

> [!WARNING]  
> Сейчас обновление доступно для EchoEar с BaseBoard. Провертье, что на нижней части EchoEar есть разъем для подключения BaseBoard как на фото ниже. Обновление для устройст без BaseBoard выйдет позже. Если прошить устройство без BaseBoard версией 
ESP32-S3, оно не будет работать. Восстановить устройство можно будет с помощью версии ESP32-S3-NOBASE. 

![IMAGE 2026-01-11 17:25:55](https://github.com/user-attachments/assets/098a2812-0492-4a94-b3f1-2bc1dcdba154)


## Требования
- Убедитесь, что устройство EchoEar включено.
- Подключите устройство к компьютеру по USB.
- Убедитесь, что устройство EchoEar имеет разъем для BaseBoard.

## Установка

1. Откройте веб-инструмент ESP Launchpad по этой ссылке:
https://espressif.github.io/esp-launchpad/?flashConfigURL=https://raw.githubusercontent.com/Denis-VR/xiaozhi-esp32/main/launchpad_xiaozhi.toml

2. Нажмите "Connect", выберети устройство и нажмите другую кнопку "Connect" как на скриншоте.

<img width="1165" height="636" alt="image" src="https://github.com/user-attachments/assets/72d8fbf7-d551-4280-9d3f-d73ac8612fec" />

3. Выберете ESP32-S3 и нажмите "Flash" чтоба начать загрузку.

<img width="727" height="307" alt="image" src="https://github.com/user-attachments/assets/9879bd97-346b-4061-938a-7396ccbd4aaa" />

4. После завершения загрузи появиться такое сообщение:

<img width="1167" height="709" alt="image" src="https://github.com/user-attachments/assets/6a042ca8-ff4b-4830-b7c6-fa4f71a33914" />

Устройство прошито, теперь нужно его выключить и включить.

Если устройство перестало запускаться и на нем отсутствует разъем для BaseBoard, нужно повторить процедуру, но выбрать версию ESP32-S3-NOBASE:

<img width="731" height="305" alt="image" src="https://github.com/user-attachments/assets/c7f3d2b4-4072-4716-a190-ad4a76b43165" />

