# 4-Channel-ESP32-Home-Automation
# 🏠 4-Channel ESP32 Home Automation

A smart **4-channel home automation system** based on **ESP32 + Blynk IoT + Manual Switches**, designed with **offline control support**.

The system allows four electrical loads to be controlled remotely through the Blynk mobile application as well as locally using physical switches.

The manual switches continue to work even when Wi-Fi or Blynk is unavailable.

---

## ✨ Features

* 🔌 4-channel relay control
* 📱 Blynk IoT remote control
* 🎛️ Physical manual switches
* 📡 Wi-Fi connectivity
* 🔄 Automatic Wi-Fi reconnection
* ☁️ Blynk cloud control
* 📴 Offline/manual control
* 🔁 Relay state synchronization with Blynk
* ⚡ Active-LOW relay support
* 🚫 Non-blocking Blynk connection attempt
* 🖥️ Serial Monitor status messages
* 💡 Suitable for home automation and IoT projects

---

# 🧰 Hardware Required

| Component                    |    Quantity |
| ---------------------------- | ----------: |
| ESP32 Development Board      |           1 |
| 4-Channel Relay Module       |           1 |
| Push Buttons / Wall Switches |           4 |
| Wi-Fi Network                |           1 |
| Jumper Wires                 | As required |
| Suitable Power Supply        |           1 |

### Optional Loads

The relay channels can be connected to devices such as:

* 💡 Lights
* 🌀 Fans
* 🔌 Sockets
* 💧 Water Pump
* 📺 Appliances
* 🔦 Lamps

> ⚠️ **Safety:** AC mains voltage is dangerous. Use proper electrical isolation, enclosure, fuses, and qualified electrical installation practices. Do not work on live mains wiring.

---

# 🔌 Pin Configuration

## Relay Pins

The project uses four GPIO pins for relay control:

```cpp
#define RELAY1 23
#define RELAY2 22
#define RELAY3 21
#define RELAY4 19
```

| Relay Channel | ESP32 GPIO |
| ------------- | ---------: |
| Relay 1       |    GPIO 23 |
| Relay 2       |    GPIO 22 |
| Relay 3       |    GPIO 21 |
| Relay 4       |    GPIO 19 |

---

## 🎛️ Manual Switch Pins

```cpp
#define SW1 13
#define SW2 12
#define SW3 14
#define SW4 27
```

| Switch   | ESP32 GPIO |
| -------- | ---------: |
| Switch 1 |    GPIO 13 |
| Switch 2 |    GPIO 12 |
| Switch 3 |    GPIO 14 |
| Switch 4 |    GPIO 27 |

The switches use:

```cpp
INPUT_PULLUP
```

Therefore, the switch is considered **pressed when the GPIO reads LOW**.

---

# 📱 Blynk Virtual Pins

Each relay is mapped to a Blynk virtual pin:

```cpp
int virtualPins[4] = {V0, V1, V2, V3};
```

| Relay   | Blynk Virtual Pin |
| ------- | ----------------- |
| Relay 1 | V0                |
| Relay 2 | V1                |
| Relay 3 | V2                |
| Relay 4 | V3                |

### Blynk Control

```text
V0 → Relay 1
V1 → Relay 2
V2 → Relay 3
V3 → Relay 4
```

Create four switch widgets in the Blynk dashboard and assign them to `V0`, `V1`, `V2`, and `V3`.

---

# ⚙️ Relay Logic

This project assumes an **Active-LOW relay module**.

That means:

```text
GPIO LOW  → Relay ON
GPIO HIGH → Relay OFF
```

The code handles this using:

```cpp
digitalWrite(relayPins[ch], !state);
```

So the logical state remains easy to understand:

```text
state = 1 → Appliance ON
state = 0 → Appliance OFF
```

while the actual GPIO output is inverted for the Active-LOW relay.

---

# 📴 Offline Mode

One of the main features of this project is **offline/manual operation**.

The physical switches are checked independently of Blynk:

```cpp
checkSwitches();
```

This function runs continuously in the main loop.

Therefore:

### Wi-Fi Available

```text
Manual Switch
      │
      ▼
    ESP32
      │
      ├── Relay
      │
      └── Blynk
```

### Wi-Fi Unavailable

```text
Manual Switch
      │
      ▼
    ESP32
      │
      ▼
    Relay
```

The appliance can therefore still be controlled locally even if the internet connection is lost.

---

# 📡 Wi-Fi Reconnection

The ESP32 checks the Wi-Fi status:

```cpp
if (WiFi.status() != WL_CONNECTED)
```

If Wi-Fi is disconnected, the ESP32 attempts reconnection every **10 seconds**.

```cpp
if (millis() - lastReconnect > 10000)
{
    WiFi.begin(ssid, pass);
}
```

This prevents continuous reconnection attempts.

---

# ☁️ Blynk Connection

Blynk is configured using:

```cpp
Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
```

The project does not block the ESP32 for a long time while attempting to connect.

It uses:

```cpp
Blynk.connect(1000);
```

which gives Blynk approximately **1 second** to establish a connection.

This allows the ESP32 to continue performing other tasks, including manual switch monitoring.

---

# 🔄 Blynk State Synchronization

When Blynk successfully connects:

```cpp
BLYNK_CONNECTED()
{
    Serial.println("Blynk Connected");
    Blynk.syncVirtual(V0, V1, V2, V3);
}
```

The ESP32 requests the current values of the four virtual pins.

This helps synchronize the Blynk dashboard with the controller after reconnection.

---

# 🎛️ Manual Switch Working

The project uses edge detection.

Initially:

```cpp
bool lastSwitchState[4] = {
    HIGH, HIGH, HIGH, HIGH
};
```

When a switch changes from:

```text
HIGH → LOW
```

the ESP32 detects a button press.

The corresponding relay state is toggled:

```cpp
setRelay(i, !relayState[i]);
```

For example:

```text
Relay OFF
   │
Switch Press
   ▼
Relay ON
```

Pressing the switch again:

```text
Relay ON
   │
Switch Press
   ▼
Relay OFF
```

---

# 🛡️ Switch Debouncing

Mechanical switches can produce multiple electrical transitions when pressed.

The code uses a short debounce delay:

```cpp
delay(200);
```

This prevents one physical press from being interpreted as multiple presses.

---

# 🧠 Main Program Flow

The overall working sequence is:

```text
                ┌────────────────────┐
                │       ESP32        │
                └─────────┬──────────┘
                          │
             ┌────────────┴────────────┐
             │                         │
             ▼                         ▼
      ┌──────────────┐          ┌──────────────┐
      │ Manual       │          │ Wi-Fi /      │
      │ Switches     │          │ Blynk        │
      └──────┬───────┘          └──────┬───────┘
             │                         │
             └────────────┬────────────┘
                          ▼
                  ┌──────────────┐
                  │ Relay Control│
                  └──────┬───────┘
                         │
             ┌───────────┼───────────┐
             ▼           ▼           ▼
          Relay 1      Relay 2     Relay 3
                                      │
                                   Relay 4
```

---

# 🔁 Complete Operating Logic

```text
                 START
                   │
                   ▼
             Initialize ESP32
                   │
                   ▼
          Configure Relay Pins
                   │
                   ▼
         Configure Switch Pins
                   │
                   ▼
             Start Wi-Fi
                   │
                   ▼
           Configure Blynk
                   │
                   ▼
                 LOOP
                   │
          ┌────────┴────────┐
          ▼                 ▼
      Wi-Fi Check      Check Switches
          │                 │
          ▼                 ▼
   Reconnect if needed   Toggle Relay
          │                 │
          ▼                 │
    Blynk Connected?        │
          │                 │
         YES                │
          ▼                 │
      Blynk.run()           │
          │                 │
          └────────┬────────┘
                   ▼
                 LOOP
```

---

# 🧩 Important Functions

## `setRelay()`

Controls the selected relay and updates Blynk when connected.

```cpp
void setRelay(int ch, bool state)
```

Responsibilities:

* Store relay state
* Turn relay ON/OFF
* Update Blynk virtual pin

---

## `checkSwitches()`

Checks all four physical switches.

```cpp
void checkSwitches()
```

Responsibilities:

* Read switch state
* Detect button press
* Toggle corresponding relay
* Apply debounce

---

## `BLYNK_CONNECTED()`

Runs when the ESP32 successfully connects to Blynk.

Responsibilities:

* Print connection status
* Synchronize Blynk virtual pins

---

## `BLYNK_WRITE(V0-V3)`

Receives commands from Blynk.

Example:

```cpp
BLYNK_WRITE(V0)
{
    setRelay(0, param.asInt());
}
```

Each virtual pin controls one relay.

---

# 📚 Required Libraries

Install the following libraries in Arduino IDE:

```cpp
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
```

### WiFi Library

Used for connecting the ESP32 to the local Wi-Fi network.

### Blynk Library

Used for communication between the ESP32 and Blynk Cloud.

---

# 💻 Software Requirements

* Arduino IDE
* ESP32 Board Package
* Blynk Library
* Blynk IoT Account
* Wi-Fi Network

---

# 🔐 Blynk Configuration

Add your Blynk credentials to the code:

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

Configure Wi-Fi:

```cpp
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
```

> 🔒 **Never publish your real Blynk Auth Token or Wi-Fi password on GitHub.**

Use placeholders before uploading your code publicly.

---

# 🚀 Installation

## 1. Clone the Repository

## 2. Open Arduino IDE

Open the `.ino` project file.

## 3. Install ESP32 Board

Select your ESP32 board from:

```text
Tools → Board → ESP32
```

## 4. Install Blynk Library

Install the **Blynk** library from the Arduino Library Manager.

## 5. Configure Credentials

Enter your:

* Blynk Template ID
* Blynk Template Name
* Blynk Auth Token
* Wi-Fi SSID
* Wi-Fi Password

## 6. Configure Blynk Dashboard

Create four control widgets:

```text
V0 → Relay 1
V1 → Relay 2
V2 → Relay 3
V3 → Relay 4
```

Set the widgets as switch controls.

## 7. Upload

Select the correct ESP32 board and COM port, then upload the firmware.

#

---

# 🧪 Testing

Test the project in the following order:

### Test 1 — Relay

Verify that all four relays turn OFF during startup.

### Test 2 — Manual Switch

Press each physical switch.

Expected:

```text
SW1 → Relay 1
SW2 → Relay 2
SW3 → Relay 3
SW4 → Relay 4
```

### Test 3 — Wi-Fi

Connect the ESP32 to Wi-Fi and verify the Serial Monitor.

Expected:

```text
Blynk Connected
```

### Test 4 — Blynk

Operate each virtual switch.

Expected:

```text
V0 → Relay 1
V1 → Relay 2
V2 → Relay 3
V3 → Relay 4
```

### Test 5 — Offline Mode

Disconnect Wi-Fi.

The physical switches should continue controlling the relays.

---

# ⚠️ Important Notes

* This project assumes an **Active-LOW relay module**.
* Relay modules should have a suitable power supply.
* ESP32 GPIO pins should not directly drive high-current loads.
* Use a proper relay driver/module.
* Do not connect AC mains directly to ESP32 GPIO pins.
* Use proper electrical isolation for mains-powered appliances.
* Keep Wi-Fi credentials and Blynk tokens private.
* Test the project with low-voltage loads before working with mains equipment.

---

# 🔮 Future Improvements

Possible upgrades:

* 📊 Energy monitoring
* ⚡ Current and voltage measurement
* 🕐 Scheduled appliance control
* 🌡️ Temperature-based automation
* 🌙 Automatic night-light control
* 👤 Multiple-user access
* 🔐 Authentication and security
* 📈 Power consumption dashboard
* 📱 Mobile notifications
* 🧠 AI-based automation
* 🏠 Motion-based lighting
* 🔄 Persistent relay-state storage using EEPROM/Preferences
* 🌐 Local web dashboard
* 📴 Improved fully offline automation

---

# 👨‍💻 Developer

**Surya Mani Bajpai**

Electronics Engineer | Embedded Systems | IoT | Robotics | PCB Design

Lucknow, Uttar Pradesh, India

---

# 📜 License

This project is intended for **educational, experimental, and prototyping purposes**.

You are free to modify and improve the project according to your requirements.

---

# ⭐ Support

If you find this project useful:

* ⭐ Star the repository
* 🍴 Fork the repository
* 🐛 Report issues
* 💡 Suggest improvements
* 🤝 Contribute to the project

---

**Made with ❤️ using ESP32 + Blynk IoT + Embedded Systems**

