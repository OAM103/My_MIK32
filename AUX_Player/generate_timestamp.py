import datetime

# куда будет записан файл
header_path = "src/build_timestamp.h"

now = datetime.datetime.now()

with open(header_path, "w") as f:
    f.write("/* Auto-generated timestamp */\n")
    f.write("#ifndef BUILD_TIMESTAMP_H\n")
    f.write("#define BUILD_TIMESTAMP_H\n\n")
    f.write(f"#define BUILD_YEAR    {now.year}\n")
    f.write(f"#define BUILD_MONTH   {now.month}\n")
    f.write(f"#define BUILD_DAY     {now.day}\n")
    f.write(f"#define BUILD_HOUR    {now.hour}\n")
    f.write(f"#define BUILD_MINUTE  {now.minute}\n")
    f.write(f"#define BUILD_SECOND  {now.second}\n\n")
    f.write("#endif // BUILD_TIMESTAMP_H\n")

print(f"[OK] Timestamp updated: {now}")