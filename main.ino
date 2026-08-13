
#define BLYNK_TEMPLATE_ID "*********************"
#define BLYNK_TEMPLATE_NAME "*******************"
#define BLYNK_AUTH_TOKEN "**********************"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "*************";
char pass[] = "************";

// Relay Pins
#define RELAY1 23
#define RELAY2 22
#define RELAY3 21
#define RELAY4 19

// Switch Pins
#define SW1 13
#define SW2 12
#define SW3 14
#define SW4 27

int relayPins[4] = {RELAY1, RELAY2, RELAY3, RELAY4};
int switchPins[4] = {SW1, SW2, SW3, SW4};
int virtualPins[4] = {V0, V1, V2, V3};

bool relayState[4] = {0, 0, 0, 0};
bool lastSwitchState[4] = {HIGH, HIGH, HIGH, HIGH};

unsigned long lastReconnect = 0;

//================ RELAY CONTROL =================

void setRelay(int ch, bool state)
{
  relayState[ch] = state;

  // Active LOW Relay
  digitalWrite(relayPins[ch], !state);

  // Update Blynk only if connected
  if (Blynk.connected())
  {
    Blynk.virtualWrite(virtualPins[ch], state);
  }
}

//================ BLYNK =================

BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");
  Blynk.syncVirtual(V0, V1, V2, V3);
}

BLYNK_WRITE(V0)
{
  setRelay(0, param.asInt());
}

BLYNK_WRITE(V1)
{
  setRelay(1, param.asInt());
}

BLYNK_WRITE(V2)
{
  setRelay(2, param.asInt());
}

BLYNK_WRITE(V3)
{
  setRelay(3, param.asInt());
}

//================ SWITCH =================

void checkSwitches()
{
  for (int i = 0; i < 4; i++)
  {
    bool currentState = digitalRead(switchPins[i]);

    if (lastSwitchState[i] == HIGH && currentState == LOW)
    {
      setRelay(i, !relayState[i]);
      delay(200);   // Debounce
    }

    lastSwitchState[i] = currentState;
  }
}

//================ SETUP =================

void setup()
{
  Serial.begin(115200);

  for (int i = 0; i < 4; i++)
  {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);   // OFF (Active LOW)

    pinMode(switchPins[i], INPUT_PULLUP);
  }

  // Start WiFi
  WiFi.begin(ssid, pass);

  // Configure Blynk (Non-Blocking)
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud",80);
}

//================ LOOP =================

void loop()
{
  // Reconnect WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - lastReconnect > 10000)
    {
      Serial.println("Reconnecting WiFi...");
      WiFi.begin(ssid, pass);
      lastReconnect = millis();
    }
  }

  // Run Blynk if WiFi connected
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!Blynk.connected())
    {
      Blynk.connect(1000);   // Try for 1 second only
    }

    Blynk.run();
  }

  // Manual Switch Always Works
  checkSwitches();
}
