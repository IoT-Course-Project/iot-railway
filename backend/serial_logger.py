import csv
import time
from datetime import datetime

import serial

ser = serial.Serial(port="COM6", baudrate=115200, timeout=2)
time.sleep(2)

csv_file = open("sensor_log.csv", mode="a", newline="")
writer = csv.writer(csv_file)

if csv_file.tell() == 0:
    writer.writerow(["timestamp", "ultraL", "ultraR", "irL", "irR", "temp", "label"])

print("Logging started. Press Ctrl+C to stop.")

try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        if not line or "=>" not in line:
            continue

        try:
            parts, label = line.split("=>")
            readings = parts.strip().split("|")
            values = [float(r.split(":")[1].strip()) for r in readings]
            label_val = 0 if label.strip() == "misaligned" else 1

            row = [datetime.now().isoformat()] + values + [label_val]
            writer.writerow(row)
            print("Logged:", row)
        except Exception as e:
            print("Parse error:", e)

except KeyboardInterrupt:
    print("Stopped.")
finally:
    ser.close()
    csv_file.close()
