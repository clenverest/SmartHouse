// wifi + сайт
#include <iocontrol.h>
#include <ESP8266WiFi.h>

//-----------------------------

//  Подключаем библиотеки датчиков влажности, освещенности и температуры
#include <Wire.h> // влажность
#include <OneWire.h> // температура
#include <DallasTemperature.h> // температура

//освещенность
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

//-----------------------------

#define ONE_WIRE_BUS 10 // номер пина к которому подключен DS18B20
#define SOILMOISTUREPIN A2   // Определяем номер аналогового входа, к которому подключен датчик влажности почвы.                         
#define MINVALUESOILMOISTURE 220 // значение полного полива
#define MAXVALUESOILMOISTURE 900  // значение критической сухости
#define LED_PIN 3 //объявление переменной целого типа, содержащей номер порта к которому мы подключили второй провод

//----------------------------

#define On true
#define Off false

//-------------------------------

//  Инстанцируем (создаём) объекты библиотек датчиков
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempMeter(&oneWire);

//----------------------------

//Настройка сайта
// Название панели на сайте iocontrol.ru
const char* myPanelName = "IGardenable";

//  Название переменных на сайте
const char* panelSoilHumidity = "SoilHumidity";
const char* panelTemperature = "Temperature";
const char* panelLuminosity = "Luminosity";
const char* panelLight = "Light";
const char* panelAuto = "Auto";
const char* panelLightPWM = "LightPWM";
const char* panelLightThreshold = "LightThreshold";

// Если панель использует ключ
// const char* key = "ключ";

const char* ssid = "ssid_точки_доступа_WiFi";
const char* password = "пароль_точки_доступа_WiFi";

//-----------------------------------------------

// Создаём объект клиента класса WiFiClient
WiFiClient client;

// Создаём объект iocontrol, передавая в конструктор название панели и клиента
iocontrol mypanel(myPanelName, client);

// iocontrol mypanel(myPanelName, key, client); // Если панель использует ключ

//-------------------------------------

//  Переменные для записи на сайт ioControl
bool Auto = false; 

uint16_t humidity; 
uint16_t luminosity; 
float temperature; 

//-------------------------------------

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Ждём подключения
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    tempMeter.begin(); // Инициируем датчик температуры
    tempMeter.setResolution(12); // устанавливаем разрешение датчика от 9 до 12 бит
    
    tsl.begin(); // Инициируем датчик освещенности
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
    
    mypanel.begin(); // Вызываем функцию первого запроса к сервису
}

void loop()
{
    bool light = mypanel.readBool(panelLight);
    int lightThreshold = mypanel.readInt(panelLightThreshold);
    Auto = mypanel.readBool(panelAuto);
    

    //-------------------------------------------------------
    
    //влажность
    int avalue = analogRead(SOILMOISTUREPIN)
    avalue = constrain(avalue, MINVALUESOILMOISTURE,MAXVALUESOILMOISTURE);
    humidity = map(avalue, MIN, MAX, 0, 100);  // адаптируем значения от 0 до 100,

    //освещенность
    sensors_event_t event;
    tsl.getEvent(&event); 
    luminosity = (uint16_t)event.light//  Записываем показания датчика освещённости в переменную

    //температура
    tempMeter.requestTemperatures();
    temperature = tempMeter.getTempCByIndex(0); //  Записываем показания датчика температуры в переменную

    //----------------------------------------

    //  Если освещённость меньше установленного порога
    if (luminosity < lightThreshold && Auto) 
    {
        // Включаем свет
        lightsIn(luminosity);
    }
    //  Если не автоматический режим и флаг света установлен
    else if (!Auto && light) 
    {
        // Включаем свет
        lightsIn(mypanel.readInt(panelLightPWM));
    }
    else 
    {
        // выключаем свет
        lightsOut();
    }

    //-----------------------------------------
    
    // Записываем показания всех датчиков в объект mypanel
    mypanel.write(panelSoilHumidity, humidity);
    mypanel.write(panelLuminosity, luminosity);
    mypanel.write(panelTemperature, temperature);

    //-------------------------------------------

    //  Если показания успешно записаны на сайт ioControl
    if (mypanel.writeUpdate() == OK) 
    {
        //  выводим показания в последовательный порт
        Serial.println("Updated");
        Serial.print("hum: ");
        Serial.println(humidity);
        Serial.print("lum: ");
        Serial.println(luminosity);
        Serial.print("tem: ");
        Serial.println(temperature);
    }

    mypanel.readUpdate(); // Читаем все переменные панели с сайта в объект mypanel
}

//  Функция включения светодиодной ленты
void lightsIn(int l)
{
    uint8_t pwm = 0; //  Создаём переменную для расчёта ШИМ
    
    //  Если установлен автоматический режим работы
    if (Auto)
        pwm = uint8_t(map(l, 0, 300, 255, 0)); // устанавливаем ШИМ обратно пропорционально освещённости
    else
        pwm = uint8_t(l); // устанавливаем ШИМ, переданный функции

    pinMode(LED_PIN, OUTPUT); //  Устанавливаем вывод к которому подключена светодиодная лента в режим "выход"
    analogWrite(LED_PIN, pwm); //  Устанавливаем ШИМ на выводе

    //  Если установлен автоматический режим работы, обновляем переключатель света на сайте
    if (Auto)
        mypanel.write(panelLight, On);
}

//  Функция выключения светодиодной ленты
void lightsOut()
{
    pinMode(LED_PIN, INPUT); //  Устанавливаем вывод к которому подключён вентилятор в режим "вход"
    
    //  Если установлен автоматический режим работы, обновляем переключатель света на сайте
    if (Auto)
        mypanel.write(panelLight, Off);
}
