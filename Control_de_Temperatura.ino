/* Proyecto Arduino - Control de Temperatura
   Inicio: Abril 2017
   Finalizado: 
   Información sobre el proyecto: https://docs.google.com/document/d/1yk49qOYsdUS0hTn-DKBiWXxVarbfeVRix3b75LXEF70/edit?usp=sharing
   Circuit 123: https://www.tinkercad.com/things/hI49j2M1SzS-proyecto-arduino-control-de-temperatura
   Desarrollado por: Matias Retamar y Pablo Retamar.*/
  
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#define Pin 5                            // Entrada Sensor Temperatura
#define  LED_OFF  0
#define  LED_ON  1
OneWire ourWire(Pin);                    // Se establece el pin declarado como bus para la comunicación OneWire
DallasTemperature sensors(&ourWire);     // Se instancia la librería DallasTemperature
LiquidCrystal_I2C lcd(0x27,16,2);

/* ************************** CONSTANTES ************************** */
int const pote = A0;                     // Entrada Potenciómetro
int const buzzer = A1;                   // Salida Buzzer
int const push = 2;                      // Entrada Pulsador
int const u_echo = 3;                    // Entrada U Echo - Cable Color: Verde y blanco
int const u_triger = 4;                  // Salida U Triger - Cable Color: Verde
int const electro = 6;                   // Salida Electroválvula
int const resis = 7;                    // Salida Resistencia
int const led_g = 9;                    // Salida LED G
int const led_b = 10;                    // Salida LED B
int const led_r = 11;                    // Salida LED R
int const temp_max = 65;                 // Temperatura maxima
int const temp_min = 0;                  // Temperatura maxima
int const v_resis =1023;                 // Valor pico de entrada analogica
int const d_reanudar = 28;               // Distancia para reanudar el sistema
int const max_distan = 12;               // Distancia maxima para que la electrovalvula cierre
int const min_distan = 33;               // Distancia minima para que la resistencia encienda
long const t_fallo_temp = 20*60000;      // Tiempo de fallo de calentador 20 min = 20*60000
long const t_fallo_templl = 10*60000;    // Tiempo de fallo de calentador cuando la electrovalvula esta cerrada 10 min = 10*60000
long const t_actualizado = 2000;         // Tiempo de actualizacion de los sensores
long const t_memoria = 2*60000;          // Tiempo de memoria del sensor ultrasonido
long const t_faltaagua = 5*60000;          // Tiempo de memoria del sensor ultrasonido
/* ************************** VARIABLES ************************** */
int i = 0;                               // Variable utilizada como contador
int nivel_a;                             // Nivel de agua
int temp_ini;                            // Temperatura marcada por el sensor
int temp_fin;                            // Temperatura indicada por potenciometro
int mem_fin;                             // Memoria temperatura indicada por potenciometro
int mem_ini;                             // Memoria temperatura indicada por sensor
int mem_dist;                            // Memoria de distancia
int distancia;                           // Medicion sensada por el Sensor de Ultrasonido
unsigned long currentMillis;             // Tiempo transcurrido
unsigned long resis_time;                // Resetea el tiempo de temperatura transcurrido
unsigned long refresh_time;              // Resetea el tiempo de actualizado
unsigned long memoria_time;              // Resetea de memoria del sensor ultrasonido
unsigned long faltaagua_time;            // Resetea de memoria de falta de agua
boolean mem_buzzer = false;              // Memoria para que el buzzer no suene mas de una vez
boolean lpower = true;                   // Memoria Encendido/Apagado
boolean power = false;                   // Encendido/Apagado
boolean buttonActive = false;            // Estado del boton
boolean longPressActive = false;         // Detener la pulsación larga

/* ************************** CARACTERES ESPECIALES  ************************** */
byte up[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
};
byte down[8] = {
    B00100,
    B00100,
    B00100,
    B00100,
    B11111,
    B01110,
    B00100,
};
byte grade[8] = {
    B01110,
    B10001,
    B10001,
    B01110,
    B00000,
    B00000,
    B00000,
};
byte line0[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
};
byte line1[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
};
byte line2[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
};
byte line3[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
};
byte line4[8] = {
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
};
byte line5[8] = {
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
};
byte line6[8] = {
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
};
byte line7[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
};

/* ****************************** SETUP ****************************** */
void setup() {
    colors('*'); 
    pinMode(pote, INPUT);        // Entrada Potenciometro
    pinMode(push, INPUT_PULLUP); // Pulsador
    pinMode(u_echo, INPUT);      // Entrada U Echo
    pinMode(led_g, OUTPUT);      // Salida LED G
    pinMode(led_b, OUTPUT);      // Salida LED B
    pinMode(led_r, OUTPUT);      // Salida LED R
    pinMode(u_triger, OUTPUT);   // Salida U Triger
    pinMode(buzzer, OUTPUT);     // Salida Buzzer
    pinMode(resis, OUTPUT);      // Salida Resistencia
    pinMode(electro, OUTPUT);    // Salida Electroválvula
    digitalWrite(resis, HIGH);
    digitalWrite(electro, HIGH);
    sensors.begin();             // Se inician el sensor NTC
    analogWrite(buzzer, 0);      // Apaga el Buzzer
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Inicializando...");
    delay(2000);
    lcd.noDisplay();
    lcd.createChar(1, grade);         // Crea el caracter Grados centigrados lo guarda en 1
    lcd.createChar(2, up);            // Crea el caracter fleha arriba lo guarda en 2
    lcd.createChar(3, down);          // Crea el caracter fleha abajo lo guarda en 3
    lcd.clear();
    lcd.setBacklight(LED_OFF);
    distancia = DistanceFunction();   // Funcion para el calculo para el sensor de ultrasonido
    temperature();           
}

/* ****************************** FUNCION DE ENCENDIDO ****************************** */
void PowerFunction(){
    if (digitalRead(push) == LOW && power != lpower){    //Espera hasta que el pulsador sea presionado
        if (power == HIGH){                   // **** APAGADO DEL SISTEMA ****
            power = LOW;
            lpower = !power;                  // Guarda el ultimo estado de lpower 
            KillOutputs ();                   // Mata todas las salidas
            colors('*');
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Apagando");            // Muestra en pantalla
            for (i=0; i <= 3; i++){           // Animacion de puntos
                lcd.print(".");
                delay(300);
            }
            lcd.noDisplay();
            lcd.setBacklight(LED_OFF);
            lcd.clear();
            mem_buzzer = false;               // Reinicia la memoria del Buzzer
            resis_time = millis();
        }else{
            power = HIGH;                     // ****  ENCENDIDO DEL SISTEMA ****
            lpower = !power;                  // Guarda el ultimo estado de lpower 
            colors('g');                      // Enciende el Led Verd
            lcd.init();
            lcd.setBacklight(LED_ON);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Encendiendo");         // Muestra en pantalla
            for (i=0; i <= 3; i++){           // Animacion de puntos
                lcd.print(".");
                delay(300);
            }
            mem_buzzer = false;               // Reinicia la memoria del Buzzer
            a_display(1);
            resis_time = millis();
            memoria_time = millis();
        }
    }
}

/* ****************************** FUNCION DE ERROR ****************************** */
boolean ErrorFunction(){
    long buttonTimer = 0;
    long longPressTime = 4000;
    boolean e_exit = false;
    KillOutputs();                        // Mata todas las salidas
    colors('*');
    while(exit != false){
        if(digitalRead(push) == LOW) {
            if (buttonActive == false) {
                buttonActive = true;
                buttonTimer = millis ();
            }
            if ((buttonTimer > longPressTime) && (longPressActive == false)){
                longPressActive = true;
                break;
            }
        }else{
            longPressActive = false;
            colors('*');                      // Apaga el led RGB
            delay(1000);
            colors('r');                      // Enciende el LED ROJO
            delay(1000);
            buttonActive = false;
            resis_time = millis();
            memoria_time = millis();
            KillOutputs();                        // Mata todas las salidas
        }
    }
    PowerFunction();
}


/* ****************************** KILLOUTPUTS ****************************** */ 
void KillOutputs (){
    digitalWrite(u_triger, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(resis, HIGH);
    digitalWrite(electro, HIGH);
}

/* ****************************** CONTROL DEL LED ****************************** */
void colors(char color){                    // La funcion recibe un parametro que se guarda en variable color
  switch(color){                          // Se compara variable color con dato guardado
    case 'y':   analogWrite(led_r,55);    // Color Amarillo
                analogWrite(led_g,55);
                analogWrite(led_b,255);
    break;
    case 'r':   analogWrite(led_r,0);     // Color Rojo
                analogWrite(led_g,255);
                analogWrite(led_b,255);
    break;
    case 'g':   analogWrite(led_r,255);   // Color Verde
                analogWrite(led_g,0);
                analogWrite(led_b,255);
    break;
    case '*':   analogWrite(led_r,255);   // Apagado
                analogWrite(led_g,255);
                analogWrite(led_b,255);
    break;
  }
}

/* ****************************** ACTUALIZA EL NIVEL DE AGUA ****************************** */ 
void lines(int nivel){                              // La funcion recibe un valor del 1 al 7 en adelante del nivel del agua y lo traduce en carracter
  switch(nivel){                                  // Se compara variable color con dato guardado
    default:  lcd.createChar(0, line0);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 1:   lcd.createChar(0, line1);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 2:   lcd.createChar(0, line2);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 3:   lcd.createChar(0, line3);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 4:   lcd.createChar(0, line4);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 5:   lcd.createChar(0, line5);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case 6:   lcd.createChar(0, line6);         // Crea el caracter
              lcd.setCursor(7 ,1);
              lcd.print((char)8);               // Muestra el caracter
              lcd.print((char)8);               // Muestra el caracter
    break;
    case  7 ... 10:   lcd.createChar(0, line7); // Crea el caracter
                      lcd.setCursor(7 ,1);
                      lcd.print((char)8);       // Muestra el caracter
                      lcd.print((char)8);       // Muestra el caracter
    break;
    case  100:  lcd.setCursor(10 ,1);
                lcd.print(" ");
                lcd.setCursor(10 ,1);
                lcd.print((char)2);             // Muestra el caracter fecha arriba
    break;
    case  150:  lcd.setCursor(10 ,1);
                lcd.print(" ");
                lcd.setCursor(10 ,1);
                lcd.print((char)3);             // Muestra el caracter fecha abajo
    break;
  }
}
/* ****************************** ACTUALIZA DISPLAY ****************************** */
void a_display(int caso){
    switch(caso){                                  
        case 1:                  // Actualiza todo el display
            lcd.setCursor(0 ,1);
            lcd.print("I:");
            lcd.setCursor(4 ,1);
            lcd.createChar(1, grade);         // Crea el caracter Grados centigrados lo guarda en 1
            lcd.print((char)1);
            lcd.setCursor(11, 1);
            lcd.print("S:");
            lcd.setCursor(15 ,1);
            lcd.createChar(1, grade);         // Crea el caracter Grados centigrados lo guarda en 1
            lcd.print((char)1);
            lcd.setCursor(13, 1);
            lcd.print(temp_ini);
        break;
        case 100:               // FALLA CALENTADOR
            lcd.clear();
            lcd.setCursor(0 ,0);
            lcd.print("----- FALLA ----");
            lcd.setCursor(0 ,1);
            lcd.print("-- CALENTADOR --");
        break;
        case 20:               // FALTA DE AGUA
            lcd.setCursor(0 ,0);
            lcd.print("  Falta de Agua ");
        break;
        case 30:                // LLENANDO
            lcd.setCursor(0,0);
            lcd.print("    Llenando   ");
        break;
        case 40:                // CALENTANDO
            lcd.setCursor(0,0);
            lcd.print("   Calentando   ");
        break;
        case 50:                // FINALIZADO
            lcd.setCursor(0,0);
            lcd.print("   Finalizado   ");
        break;
    }
}

/* ****************************** FUNCION FALTA DE AGUA ****************************** */
boolean FaltaDeAgua(){
    long duration, inches;
        for(i=0 ; i<3; i++){
          analogWrite(buzzer, 200);         // Enciende el buzzer
          delay(50);
          analogWrite(buzzer, 0);           // Apaga el buzzer
          delay(50);
        }
    do {
        if ((unsigned long)(currentMillis - refresh_time) >= t_actualizado - 1000){
            refresh_time = millis();
            digitalWrite(u_triger, HIGH);
            delayMicroseconds(10);
            digitalWrite(u_triger, LOW);
            duration = (pulseIn(u_echo, HIGH)/2);
            cm = (duration * 0.0343);
            nivel_a = map(distancia, min_distan + 5, max_distan, 0, 7);
            lines(nivel_a);                                          // Muestra las lineas de llenado en Display
            temp_ini = temperature();                                           // Actualiza el valor del sensor de Temperatura
            a_display(1); 
        }
        temp_fin = map(analogRead(pote), 0, v_resis, 0, temp_max); 
        temp_fin = constrain(temp_fin , temp_ini, temp_max); 
        lcd.setCursor(2, 1);
        lcd.print(temp_fin);                      // Muestra la temperatura ingresada 
        if (distancia > min_distan){              // Si la distancia es mayor o igual a la minima distancia apaga la Resistencia
          digitalWrite(resis, HIGH);              // Apaga la Resistencia
        } else if(temp_fin != temp_ini){
          digitalWrite(resis, LOW);               // Enciende la Resistencia
        }
        if (temp_fin <= temp_ini){                // Si temperatura sensada es igual o mayor a la ingresada, apaga la resistencia 
            digitalWrite(resis, HIGH);
        }
        if(mem_ini < temp_ini){                   // Si la temperatura nueva es mayor a la anterior imprime en pantalla una flecha para arriba
          mem_ini = temp_ini;
          lines(100);             
          resis_time = currentMillis;             // Resetea el tiempo de temperatura transcurrido porque aumento la temperatura
        } else if (mem_ini > temp_ini){           // Si la temperatura nueva es menor a la anterior imprime en pantalla una flecha para abajo
          mem_ini = temp_ini;
          lines(150);
        }
          PowerFunction();
          if (power == false){                   //Sale del While
            KillOutputs ();                      // Mata todas las salidas
            colors('*');
            break;
          }
    } while(mem_dist == cm);
    mem_dist = distancia + 2;
    memoria_time = millis();
}

/* ****************************** DEVUELVE EL VALOR DEL ULTRASONIDO ****************************** */
long  DistanceFunction(){
    long duration, inches, cm;
    digitalWrite(u_triger, HIGH);
    delayMicroseconds(10);
    digitalWrite(u_triger, LOW);
    // medimos el pulso de respuesta
    duration = (pulseIn(u_echo, HIGH)/2);
    // convert the time into a distance
    cm = (duration * 0.0343);
    if (cm >= max_distan - 1 && cm <= max_distan + 1 ){
         cm = min(cm, distancia);
    } 
    if (cm >= min_distan - 1 && cm <= min_distan + 1){
         cm = max(cm, distancia);
    } 
    if (digitalRead(electro) == LOW){
        if (mem_dist == cm){
              if ((unsigned long)(currentMillis - faltaagua_time) >= t_faltaagua){
                faltaagua_time = millis();
                mem_dist = cm;
                a_display(20);                       // Mensaje - FALTA DE AGUA
                FaltaDeAgua();    
            }
        }
    }else if (digitalRead(electro) == HIGH && distancia <= max_distan){
      memoria_time = millis();
    }
    return cm;
}

/* ****************************** DEVUELVE EL VALOR TEMPERATURA ****************************** */
int temperature(){
    sensors.requestTemperatures();                               // Prepara el sensor para la lectura
    temp_ini= sensors.getTempCByIndex(0);                        // Se lee e imprime la temperatura en grados Celsius
    return temp_ini;
}

/* ****************************** ACTUALIZACION DE SENSORES ****************************** */
void a_sensores(){
    if ((unsigned long)(currentMillis - memoria_time) >= t_memoria){
        memoria_time = millis();
        mem_dist = distancia;    
    }
    if ((unsigned long)(currentMillis - refresh_time) >= t_actualizado){
        refresh_time = millis();
        distancia = DistanceFunction();                          // Funcion para el calculo para el sensor de ultrasonido
        nivel_a = map(distancia, min_distan + 5, max_distan, 0, 7);
        lines(nivel_a);                                          // Muestra las lineas de llenado en Display
        temperature();                                           // Actualiza el valor del sensor de Temperatura
        a_display(1);                                            // Muestra la temperatura del sensor en Display
    }    
}

/* ****************************** MAIN LOOP ****************************** */
void loop(){
  PowerFunction();
  if (power == false){                  //Sale del While
    KillOutputs ();                   // Mata todas las salidas
    colors('*');
  }
  resis_time = millis();
  memoria_time = millis();
  mem_buzzer = false;                   // Reinicia el Buzzer
  while(power != LOW){
    currentMillis = millis();
    a_sensores();                         // Atualiza todos los valores de los sensores
    PowerFunction();
    if (power == false) break;            // Sale del While si se se presiono el boton
    if(mem_ini < temp_ini){               // Si la temperatura nueva es mayor a la anterior imprime en pantalla una flecha para arriba
      mem_ini = temp_ini;
      lines(100);             
      resis_time = currentMillis;       // Resetea el tiempo de temperatura transcurrido porque aumento la temperatura
    } else if (mem_ini > temp_ini){         // Si la temperatura nueva es menor a la anterior imprime en pantalla una flecha para abajo
      mem_ini = temp_ini;
      lines(150);
    }
    if (digitalRead(resis) != HIGH){
      if (digitalRead(electro) == HIGH){
        if ((unsigned long)(currentMillis - resis_time) >= t_fallo_templl){   // Tiempo de falla mas corto, porque no esta entrando agua fria
          a_display(100);                    // FALLA CALENTADOR
          ErrorFunction();
          if (power == false) break;                                      //Sale del While
        }
        }else{
          if ((unsigned long)(currentMillis - resis_time) >= t_fallo_temp){     // Tiempo de falla mas largo, porque esta entrando agua fria
            a_display(100);                    // FALLA CALENTADOR
            ErrorFunction();
          if (power == false) break;         // Sale del While
        }
      }
    }
    temp_fin = map(analogRead(pote), 0, v_resis, 0, temp_max); 
    temp_fin = constrain(temp_fin , temp_ini, temp_max); 
    lcd.setCursor(2, 1);
    lcd.print(temp_fin);                      // Muestra la temperatura ingresada 
    if (distancia - 1 >= max_distan){
      digitalWrite(electro, LOW);
      colors('y');                          // Enciende el Led Amarillo
      if (digitalRead(resis) == HIGH  && digitalRead(electro) == LOW){
        a_display(30);                      // Display Llenando
      }
    }
    PowerFunction();
    if (power == false) break;                // Sale del While
    if (distancia <= max_distan){             // Si la distancia es menor o igual a la maxima distancia apaga la electrovalvula
      digitalWrite(electro, HIGH);
    }
    else if (temp_fin != temp_ini && digitalRead(resis) != HIGH ){
      digitalWrite(electro, LOW);
      colors('y');                          // Enciende el Led Amarillo
      a_display(40);                        // Display Calentando
    }
    else if (temp_fin != temp_ini ){
      digitalWrite(electro, LOW);
      colors('y');                          // Enciende el Led Amarillo
    }
    if (distancia >= d_reanudar){
      mem_buzzer = false;                   // Reinicia el Buzzer
    }
    if (distancia > min_distan){             // Si la distancia es mayor o igual a la minima distancia apaga la Resistencia
      digitalWrite(resis, HIGH);            // Apaga la Resistencia
      colors('y');                          // Enciende el Led Amarillo
      if (digitalRead(resis) == HIGH  && digitalRead(electro) == LOW){
        a_display(30);                      // Display Llenando
      }else{
        a_display(40);                      // Display Calentando
      }
    }else if(temp_fin != temp_ini){
      a_display(40);                        // Display Calentando
      digitalWrite(resis, LOW);             // Enciende la Resistencia
      colors('y');                          // Enciende el Led Amarillo
    }
    PowerFunction();
    if (power == false) break;           //Sale del While
    if (temp_fin <= temp_ini && distancia <= max_distan){    // Si temperatura sensada es igual o mayor a la ingresada, apaga la resistencia "FINALIZADO"    
      digitalWrite(resis, HIGH);
      a_display(50);                        // Display Finalizado
      if (mem_buzzer == false){             // Suena el Buzzer 3 veces y destella el LED en verde
        for(i=0 ; i<3; i++){
          analogWrite(buzzer, 200);         // Enciende el buzzer
          colors('g');                      // Enciende el led RGB
          delay(300);
          analogWrite(buzzer, 0);           // Apaga el buzzer
          colors('*');                      // Apaga el Led Verde
          delay(300);
        }
        colors('g');                        // Enciende el Led Verde
        mem_buzzer = true;                  // Esto hace que no vuelva a sonar el buzzer a no ser que la electrovalvula se encienda
      }
      if (digitalRead(resis) == HIGH && digitalRead(electro) == HIGH){  
        mem_fin = temp_fin;
        while (distancia <= d_reanudar){         // Esto hace que el sistema no aranque a llenar el tacho hasta que no se vacie lo suficiente por la medida minima o si pasa determinado tiempo (t_reanudado) sin usarse el sistema 
          if (distancia > max_distan){ 
            if (mem_dist == distancia) { 
              mem_buzzer = false;                   // Reinicia el Buzzer
              memoria_time =millis();
              mem_dist = distancia - 2;
              break;
            }
          }
          currentMillis = millis();
          resis_time = millis();                // Resetea el tiempo de temperatura transcurrido
          a_sensores();                         // Actualiza todos los valores de los sensores
          PowerFunction();
          if (power == false) break;            // Sale del While si se se presiono el boton
          if (mem_ini > temp_ini){              // Si la temperatura nueva es menor a la anteen trior imprime en pantalla una flecha para abajo  (A esta altura del programa unicamente la temperatura puede bajar)
            mem_ini = temp_ini;
            lines(150);
          }
          temp_fin = map(analogRead(pote), 0, v_resis, 0, temp_max);
          temp_fin = constrain(temp_fin , temp_ini, temp_max); 
          mem_ini = temp_ini;
          lcd.setCursor(2, 1);
          lcd.print(temp_fin);                                                  // Muestra la temperatura ingresada
          if (temp_fin > mem_fin + 2){                                          // Si la temperatura seteada aumenta mas de 2 grados, vuelve arrancar todo el proceso
            mem_buzzer = false;                                               // Reinicia el Buzzer          
            break;
          }
          if (temp_ini <= mem_fin - 3){                                         // Cuando caiga 5 grados vuelva a calentar
            while (temp_fin >= temp_ini && distancia <= max_distan){
              currentMillis = millis();
              a_sensores();                                                 // Atualiza todos los valores de los sensores
              PowerFunction();                                              // Checkea el pulsador
              if (power == false) break;                                    // Sale del While
              if(mem_ini < temp_ini){
                mem_ini = temp_ini;
                lines(100);                                                   // FALLA CALENTADOR
                resis_time = millis();                                        // Resetea el tiempo de temperatura transcurrido
              }
              else if (mem_ini > temp_ini){
                mem_ini = temp_ini;
                lines(150);
              }
              if (temp_ini >= temp_fin) break;  
                if (digitalRead(resis) != HIGH){
                  if ((unsigned long)(millis() - resis_time) >= t_fallo_templl){       //Tiempo de falla mas corto, porque no esta entrando agua fria
                  a_display(100);                                                    // FALLA CALENTADOR
                  ErrorFunction();
                  if (power == false) break;                                         //Sale del While
                  }
                }
              temp_fin = map(analogRead(pote), 0, v_resis, 0, temp_max);
              temp_fin = constrain(temp_fin , temp_ini, temp_max);
              lcd.setCursor(2, 1);
              lcd.print(temp_fin);                                          // Muestra la temperatura ingresada
              a_display(40);                                                // Display Calentando
              colors('y');
              if (temp_fin > mem_fin + 2){                                  // Si la temperatura seteada aumenta mas de 2 grados, vuelve arrancar todo el proceso
                mem_buzzer = false;                                       // Reinicia el Buzzer          
                break;
              }
              digitalWrite(resis, LOW);
            }
          }else{
            digitalWrite(resis, HIGH);
            a_display(50);                     // Display Finalizado
            colors('g');
          }
        }
      }
    }
  } 
}
