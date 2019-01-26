### **Proyecto Arduino**
### _Termotanque eléctrico automatizado_

**Objetivo:**
Realizar un sistema de control de temperatura utilizando Arduino. Los actuadores que utilizaremos son, una **electroválvula** para abrir y cerrar el paso de agua y una **resistencia térmica** para calentar el agua. Utilizaremos un sensor de Ultrasonido, para poder medir la distancia del agua y de esta manera poder saber si esta lleno o vacio el tanque de agua, dicho sensor lo nombraremos **Sensor U**.
Además contará con 1 pulsadores físicos, uno para el encendido/apagado del sistema y un potenciómetro para el **seteo de la temperatura**.
Y por último un **LED RGB** para poder indicar a simple vista el estado del sistema y un **buzzer** que suene cuando haya llegado a la temperatura seteada.

### **Lógica del sistema:**
A continuación se detallara las condiciones que deberá cumplir el sistema.
- La Resistencia **NO** se podrá activar si la medición del **Sensor U** es mayor a **25 cm**, una vez que rompa el umbral de **25 cm** podrá encender la **Resistencia**.
- El Calentador deberá cortar cuando el sensor de temperatura sense la temperatura ingresada manualmente, mediante el potenciómetro y mantener la temperatura +/- 5°C.
- Si se modifica el valor del potenciómetro mientras el proceso de calentamiento está en curso, este se deberá actualizar.
- La electroválvula se deben cerrar cuando el **Sensor U,** devuelva un valor de 6 cm.
- En cualquier momento, si se presiona el pulsador de **Encendido/Apagado** , deberá detener el proceso si se encuentra funcionamiento y deberá iniciar si encontraba apagado.
- Una vez que el tanque esté lleno de agua y se encuentre en la temperatura ingresada, permita que se vacíe sin abrir la **electroválvula** o encender la **resistencia** hasta que el **Sensor U** mida **20 cm**.
- Si el sistema está finalizado, se usa pero no llega a los 20 cm como para que reanude el sistema, este deberá llenarse nuevamente si no se detecta diferencia en los valores del **sensor U** después de 1 minuto.
- Si luego de  **10 segundos** de electroválvula abierta no detecta un aumento en el **Sensor U,** deberá aparecer un mensaje de avería **“Falta de agua”**. 
- Si la electroválvula se encuentra abierta y la diferencia entra la nueva medición y la anterior es mayor a **6 cm**, aparecer un mensaje de avería **“Falla Llenado”**.
- Si luego de **15 minutos** la temperatura sensada no aumenta de valor deberá y la **electroválvula está encendida** aparecer avería **“Falla Calentador”**. 
- Si luego de **5 minutos** la temperatura sensada no aumenta de valor deberá y la **electroválvula está apagada** aparecer avería **“Falla Calentador”**.
- Tanto para la avería **“Falta de agua”**, **“Falla Llenado**” y **“Falla Calentador”** se deberá mantenerse apretado por **3 segundos** el Boton de **Encendido/Apagado** para reiniciarlo.
- La temperatura máxima ingresada no podrá ser mayor a **65°C** y la mínima no podrá ser menor que la temperatura sensada por el sensor.

### Diagrama Simbólico:
![alt text](https://i.imgur.com/lFwMWCA.png)

### Tabla Lógica:
![alt text](https://i.imgur.com/0VBJtm0.png)

### Leyenda:
|  	|  	|
|-------------------------------------------------	|-------------------	|
| P = P = Pulsador 	| LV = Led Verde 	|
| U = Sensor de ultrasonido (>=25 = 0 ; 6<25 = 1) 	| LA = Led Amarillo 	|
| T = Sensor de Temperatura (Seleccionada = 1). 	| LR = Led Rojo 	|
| R = Resistencia Térmica 	| B = Buzzer 	|
| E = Electroválvula 	|  	|

### Código de Alertas:
| Estado 	| Color de LED 	| Texto en Display 	|
|-------------------	|-------------------	|-----------------------------------------------	|
| Temperatura alcanzada	| Verde 	| Finalizado (señal sonora) 	|
| En funcionamiento 	| Amarillo 	| Calentando / Llenando 	|
| En funcionamiento 	| Azul 	| Llenando 	|
| Avería 	| Rojo Intermitente 	| Falta de agua/ Falla Calentador/Falla llenado 	|

### Puertos a utilizar
| Puertos Analogicos 	| Puertos Digitales 	| Puertos  Digitales 	|
|---------------------------	|-------------------------------	|-----------------------	|
| A0: Entrada potenciómetro 	| 0: 	| 7: Salida Resistencia 	|
| A1: Salida Buzzer 	| 1: 	| 8: 	|
| A2: 	| 2: Entrada Pulsador 	| 9: Salida Led G 	|
| A3: 	| 3: Entrada Sensor U triger 	| 10: Salida Led B 	|
| A4: SDA (Data) 	| 4: Salida Sensor U echo 	| 11: Salida Led R 	|
| A5: SCL (Clock) 	| 5: Entrada Sensor Temperatura 	| 12: 	|
|  	| 6: Salida electroválvula 	| 13: 	|

### Lista de Materiales:
| Material 	| Cantidad 	| Estado 	|
|-------------------------------	|----------	|--------	|
| Arduino UNO 	| 1 	| OK 	|
| Sensor Ultrasonido HC-SR04 	| 1 	| OK 	|
| Sensor de temperatura Ds18b20 	| 1 	| OK 	|
| LED RGB 	| 1 	| OK 	|
| Resistencia 1K 	| 3 	| OK 	|
| Display 16x2 	| 1 	| OK 	|
| Pulsador 	| 1 	| OK 	|
| Potenciómetro 10K 	| 1 	| OK 	|
| Buzzer 	| 1 	| OK 	|
| Electroválvula 	| 1 	| OK 	|
| Resistencia 	| 1 	| OK 	|
| Módulo Relé x 2 	| 1 	| OK 	|

### Lógica Resistencia:
![alt text](https://imgur.com/Q4ErCQU.png)
La resistencia no deberá activarse si el valor devuelto por el sensor de ultrasonido es mayor al **valor** mínimo representado por la variable **min_distan** en el código. Esto está pensado de esta manera para que la resistencia no trabaje en vacío y alargar su vida útil.
El corte de la resistencia estará determinado por el valor de temperatura ingresada por el usuario.

### Lógica Electroválvula:
![alt text](https://imgur.com/422naDL.png)
La electroválvula se activa siempre que el valor devuelto por el sensor de ultrasonido sea mayor o igual al **valor máximo** representado por la variable **max_distan** en el código. El objetivo de este límite es para que el agua no rebalse del tanque.

### Conexionado:
![alt text](https://imgur.com/ijYfWSL.png)

| Leyenda 	|  	|
|---------------------------	|--------------------------------	|
| 1 - Arduino Uno 	| 6 - Sensor de Temperatura 	|
| 2 - Display 16 x 2 	| 7 - Pulsador Encendido/Apagado 	|
| 3 - Sensor Ultrasonido 	| 8 - Buzzer 	|
| 4 - Salida Electroválvula 	| 9 - Potenciometro 10K 	|
| 5 - Salida Resistencia 	| 10 - LED RGB 	|

### Diseño de la Caja:
![alt text](https://imgur.com/BtjbwTi.png)

![alt text](https://imgur.com/sQVOoDQ.png)

