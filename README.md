🕒🌡️ Time & Temperature Sensor (STM32F401RE)
A microcontroller-based project that displays real-time clock and temperature using an STM32F401RE, an LM35 temperature sensor, and a 16x2 LCD in 4-bit mode.
The system supports Normal Mode (time & temperature display) and Set Mode (user can adjust hours, minutes, and AM/PM using a keypad).
✨ Features
📟 Displays time and temperature on 16x2 LCD
🌡️ Reads temperature from LM35 sensor
⌛ Clock modes: Normal Mode & Set Mode
🔢 Keypad input for setting time
⚡ Built using STM32 HAL libraries in STM32CubeIDE
🛠️ Technologies Used
STM32F401RE (ARM Cortex-M4)
LM35 Temperature Sensor
16x2 LCD (4-bit mode)
Keypad (4x4)
STM32CubeIDE / HAL Drivers
Embedded C
📂 Project Structure
time-temp-sensor/
├── firmware/        # STM32 firmware source code
│   ├── main.c       # Main application code
│   ├── main.h       # Header file
│   ├── lcd4.h       # LCD driver (4-bit mode)
│   └── ... (other files if needed)
├── docs/            # Schematics, design notes (optional)
└── README.md        # Project documentation
