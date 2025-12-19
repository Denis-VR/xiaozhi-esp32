#!/usr/bin/env python3
"""
Скрипт для получения Device ID (UUID) и MAC address устройства xiaozhi.

Может работать в двух режимах:
1. Извлечение из лога (файл или stdin)
2. Прямой вывод значений из предоставленных данных

Использование:
    # Из файла лога
    python3 scripts/get_device_info.py < log_file.txt
    
    # Из вывода idf.py monitor (pipe)
    idf.py monitor | python3 scripts/get_device_info.py
    
    # Прямой ввод значений
    python3 scripts/get_device_info.py --uuid <UUID> --mac <MAC>
"""

import sys
import re
import argparse

def extract_uuid_from_log(line):
    """Извлекает UUID из строки лога."""
    # Паттерн: I (249) Board: UUID=f0a8ee71-9069-49b3-8ecf-f5bf173933a7 SKU=echoear
    pattern = r'UUID=([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})'
    match = re.search(pattern, line, re.IGNORECASE)
    if match:
        return match.group(1)
    return None

def extract_mac_from_log(line):
    """Извлекает MAC адрес из строки лога."""
    # Паттерн: I (789) wifi:mode : sta (90:e5:b1:c9:f4:20)
    patterns = [
        r'sta\s*\(([0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2})\)',
        r'mac[:\s]+([0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2})',
        r'([0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2})',
    ]
    for pattern in patterns:
        match = re.search(pattern, line, re.IGNORECASE)
        if match:
            mac = match.group(1).lower()
            # Проверяем, что это валидный MAC (не 00:00:00:00:00:00)
            if mac != '00:00:00:00:00:00':
                return mac
    return None

def parse_log(input_lines):
    """Парсит лог и извлекает UUID и MAC адрес."""
    uuid = None
    mac = None
    
    for line in input_lines:
        # Ищем UUID
        if uuid is None:
            found_uuid = extract_uuid_from_log(line)
            if found_uuid:
                uuid = found_uuid
        
        # Ищем MAC адрес
        if mac is None:
            found_mac = extract_mac_from_log(line)
            if found_mac:
                mac = found_mac
        
        # Если оба найдены, можно прекратить поиск
        if uuid and mac:
            break
    
    return uuid, mac

def print_device_info(uuid, mac):
    """Выводит информацию об устройстве в удобном формате."""
    print("=" * 60)
    print("Информация об устройстве xiaozhi")
    print("=" * 60)
    
    if uuid:
        print(f"Device ID (UUID): {uuid}")
    else:
        print("Device ID (UUID): НЕ НАЙДЕН")
    
    if mac:
        print(f"MAC Address:      {mac}")
    else:
        print("MAC Address:      НЕ НАЙДЕН")
    
    print("=" * 60)
    
    if uuid and mac:
        print("\nДля использования в API:")
        print(f"  Client-Id: {uuid}")
        print(f"  Device-Id: {mac}")
        print("\nДля получения кода активации:")
        print(f"  python3 scripts/get_activation_code.py {uuid}")

def main():
    parser = argparse.ArgumentParser(
        description='Извлекает Device ID (UUID) и MAC address из логов устройства xiaozhi',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Примеры использования:
  # Из файла лога
  python3 scripts/get_device_info.py < log_file.txt
  
  # Из вывода idf.py monitor
  idf.py monitor | python3 scripts/get_device_info.py
  
  # Прямой ввод значений
  python3 scripts/get_device_info.py --uuid f0a8ee71-9069-49b3-8ecf-f5bf173933a7 --mac 90:e5:b1:c9:f4:20
        """
    )
    
    parser.add_argument('--uuid', type=str, help='UUID устройства (Device ID)')
    parser.add_argument('--mac', type=str, help='MAC адрес устройства')
    parser.add_argument('--file', type=str, help='Путь к файлу лога')
    
    args = parser.parse_args()
    
    # Если указаны прямые значения, используем их
    if args.uuid or args.mac:
        uuid = args.uuid
        mac = args.mac
        if not uuid:
            uuid = None
        if not mac:
            mac = None
    else:
        # Читаем из файла или stdin
        if args.file:
            try:
                with open(args.file, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
            except FileNotFoundError:
                print(f"Ошибка: файл '{args.file}' не найден", file=sys.stderr)
                sys.exit(1)
            except Exception as e:
                print(f"Ошибка при чтении файла: {e}", file=sys.stderr)
                sys.exit(1)
        else:
            # Читаем из stdin
            lines = sys.stdin.readlines()
        
        uuid, mac = parse_log(lines)
    
    print_device_info(uuid, mac)
    
    # Возвращаем код ошибки, если не все данные найдены
    if not uuid or not mac:
        sys.exit(1)

if __name__ == "__main__":
    main()

