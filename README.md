# SmartHouse
 
## Демонстарция проекта

Изменения состояния тепличной среды отображаются в приложении пользователя (Blynk)

(температура в градус Цельсия, влажность в процентах, освещенность в люксах)

<img src="./readme_assets/gif1.gif" alt="change temperature and humadity" width="100%">


<img src="./readme_assets/gif2.gif" alt="change luminosity" width="100%">


Есть возможность ручного управления светом в теплице.

<img src="./readme_assets/gif3.gif" alt="ON/OFF light" width="100%">


- - 

Есть возможность ручного управления яркостью света в теплице.

При помощи карточки LightPWM можно установить яркость освещения, где 0 - минимальная яркость, когда лампочка не горит, 255 - максимально возможная яркость.

<img src="./readme_assets/gif4.gif" alt="change brightness of the light" width="100%">


Также присутствует авто режим при котором в теплице автоматически включается искусственный свет при недостатке естественного освещения.

В карточке LightThreshold можно установить пороговое значение освещенности. Если текущее значение освещенности ниже порогового, то автоматически включается свет, если выше - выключается.

<img src="./readme_assets/gif5.gif" alt="auto mode" width="100%">


___