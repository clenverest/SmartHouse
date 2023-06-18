#define BLYNK_TEMPLATE_ID "TMPL4McSSyhjq"
#define BLYNK_TEMPLATE_NAME "IGardenable"
#define BLYNK_AUTH_TOKEN "eIqnEE4b9bo3BntsFKsSZYwYItRf_lxZ"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const int DHT_PIN = 15;
const int PHOTO_SENSOR_PIN = 35;
const int LED_PIN = 2;

const float GAMMA = 0.7;
const float RL10 = 50;
const int ON = 1;
const int OFF = 0;

int LIGHTvalue;
int pwm = 255;
int lightThreshold;
int Auto;

DHTesp dht;

TempAndHumidity  data;

void setup()
{
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(PHOTO_SENSOR_PIN, INPUT);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  data = dht.getTempAndHumidity();
  float lux = getLuminosity();

  Blynk.run();

  lightControl(lux);

  setData(data.temperature, data.humidity, lux);
}

float getLuminosity()
{
  int analogValue = analogRead(PHOTO_SENSOR_PIN);
  float voltage = analogValue * 5 / 4095.0;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  return pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1/GAMMA));
}

void setData(float temp, float hum, float lux)
{
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V0, hum);
  Blynk.virtualWrite(V3, lux);
}

void lightControl(float lux)
{
  if ((lux < lightThreshold) && (Auto == ON)) 
    lightsIn(lux);
  else if((Auto == OFF) && (LIGHTvalue == ON))
    lightsIn(pwm);
  else
    lightsOut();
}

BLYNK_WRITE(V2)
{
  pwm = param.asInt();
}

BLYNK_WRITE(V5)
{
  Auto = param.asInt();
}

BLYNK_WRITE(V6)
{
  LIGHTvalue = param.asInt();
}

BLYNK_WRITE(V4)
{
  lightThreshold = param.asInt();
}

void lightsIn(int lux)
{
  uint8_t newPWM = 0;
    
  if (Auto == ON)
    newPWM = uint8_t(map(lux, 0, 10000, 255, 0));
  else
    newPWM = uint8_t(lux);

  pinMode(LED_PIN, OUTPUT); 
  analogWrite(LED_PIN, newPWM); 

  
  if (Auto == ON)
    Blynk.virtualWrite(V6, ON);
}

void lightsOut()
{
  pinMode(LED_PIN, INPUT);

  if (Auto == ON)
    Blynk.virtualWrite(V6, OFF);
}