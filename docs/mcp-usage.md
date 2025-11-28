# Руководство по использованию протокола MCP в IoT‑контроле

> В этом документе описано, как реализовать управление устройствами ESP32 с помощью протокола MCP. Подробный ход обмена сообщениями приведён в [`mcp-protocol.md`](./mcp-protocol.md).

## Введение

MCP (Model Context Protocol) — рекомендуемый протокол нового поколения для IoT‑контроля. Он использует формат JSON-RPC 2.0 и позволяет бэкенду обнаруживать и вызывать «инструменты» (Tool), предоставляемые устройством, что обеспечивает гибкое управление.

## Типовой рабочий процесс

1. После запуска устройство устанавливает соединение с сервером по базовому протоколу (WebSocket, MQTT и т. д.).
2. Бэкенд инициализирует MCP‑сессию методом `initialize`.
3. Бэкенд запрашивает список инструментов устройства через `tools/list`, чтобы получить поддерживаемые функции и их параметры.
4. Бэкенд вызывает конкретный инструмент при помощи `tools/call`, тем самым управляя устройством.

За деталями формата сообщений обращайтесь к [`mcp-protocol.md`](./mcp-protocol.md).

## Регистрация инструментов на устройстве

Устройство регистрирует инструменты через метод `McpServer::AddTool`. Типичная сигнатура:

```cpp
void AddTool(
    const std::string& name,           // имя инструмента; лучше использовать иерархию, напр. self.dog.forward
    const std::string& description,    // краткое описание для людей/LLM
    const PropertyList& properties,    // список входных параметров (может быть пустым); поддерживаются bool/int/string
    std::function<ReturnValue(const PropertyList&)> callback // обработчик вызова
);
```
- name: уникальный идентификатор, рекомендуется стиль «модуль.функция».
- description: естественно-языковое описание для лучшего понимания пользователями и моделями.
- properties: список параметров (bool/int/string, допускается задание диапазонов и значений по умолчанию).
- callback: фактическая логика, выполняемая при вызове; может возвращать bool/int/string.

## Пример регистрации (на базе ESP-Hi)

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();
    // Пример 1: без параметров, заставляем робота двигаться вперёд
    mcp_server.AddTool("self.dog.forward", "Робот движется вперёд", PropertyList(), [this](const PropertyList&) -> ReturnValue {
        servo_dog_ctrl_send(DOG_STATE_FORWARD, NULL);
        return true;
    });
    // Пример 2: с параметрами, задаём RGB цвет
    mcp_server.AddTool("self.light.set_rgb", "Установить RGB-цвет", PropertyList({
        Property("r", kPropertyTypeInteger, 0, 255),
        Property("g", kPropertyTypeInteger, 0, 255),
        Property("b", kPropertyTypeInteger, 0, 255)
    }), [this](const PropertyList& properties) -> ReturnValue {
        int r = properties["r"].value<int>();
        int g = properties["g"].value<int>();
        int b = properties["b"].value<int>();
        led_on_ = true;
        SetLedColor(r, g, b);
        return true;
    });
}
```

## Примеры JSON-RPC вызовов

### 1. Получение списка инструментов
```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": { "cursor": "" },
  "id": 1
}
```

### 2. Команда шасси двигаться вперёд
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.go_forward",
    "arguments": {}
  },
  "id": 2
}
```

### 3. Переключение режима подсветки
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.switch_light_mode",
    "arguments": { "light_mode": 3 }
  },
  "id": 3
}
```

### 4. Переворот изображения с камеры
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.camera.set_camera_flipped",
    "arguments": {}
  },
  "id": 4
}
```

## Примечания
- Окончательные названия инструментов, параметры и типы возвращаемых значений определяются регистрацией через `AddTool`.
- Для всех новых проектов рекомендуется использовать MCP в качестве базового протокола IoT‑контроля.
- Дополнительные сведения о протоколе и продвинутых сценариях смотрите в [`mcp-protocol.md`](./mcp-protocol.md).