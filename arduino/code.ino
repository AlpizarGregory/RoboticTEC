#include <Servo.h>

// 3 servos conectados a pines 9, 8 y 7
Servo servo1;  // Servo principal (pin 9)
Servo servo2;  // Servo de golpecito (pin 8)
Servo servo3;  // Servo de alineación inicial (pin 7)

// Ángulos iniciales para cada servo
int angulo1 = 40;
int angulo2 = 50;
int angulo3 = 130;
int delay_base = 8000;

void setup() {
  Serial.begin(9600);
  servo1.attach(9);                  // Conexión física de servos
  servo2.attach(8);
  servo3.attach(7);

  resetPosiciones();                // Posición inicial de todos los servos

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  

  Serial.println("Listening...");   // Esperando caracteres por Serial
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Elimina espacios extra o saltos de línea

    for (int i = 0; i < command.length(); i++) {
      resetPosiciones();       // Vuelve a posición inicial antes de cada letra
      delay(1000);
      type_key(command[i]);    // Ejecuta acción según la letra recibida
    }
  }
}

// Ejecuta una secuencia específica dependiendo del carácter recibido
void type_key(char c) {
  if (c == 't') {
    Serial.println("Letra t encontrada");

    angulo3 = 135;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 55;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 45;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == 'h') {
    Serial.println("Letra h encontrada");

    angulo3 = 120;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 50;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 40;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == 'e') {
    Serial.println("Letra e encontrada");

    angulo3 = 150;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 50;
    servo1.write(angulo1);
    delay(1000);

    angulo1 = 60;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 45;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == '2') {
    Serial.println("Número 2 encontrado");

    angulo3 = 160;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 50;
    servo1.write(angulo1);
    delay(1000);

    angulo1 = 70;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 55;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == '4') {
    Serial.println("Número 4 encontrado");

    angulo3 = 145;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 55;
    servo1.write(angulo1);
    delay(1000);

    angulo1 = 65;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 55;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == '3') {
    Serial.println("Número 3 encontrado");

    angulo3 = 150;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 55;
    servo1.write(angulo1);
    delay(1000);

    angulo1 = 65;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 55;
    servo2.write(angulo2);
    delay(1000);
  }
  else if (c == '5') {
    Serial.println("Número 5 encontrado");

    angulo3 = 135;
    servo3.write(angulo3);
    delay(delay_base);

    angulo1 = 50;
    servo1.write(angulo1);
    delay(1000);

    angulo1 = 65;
    servo1.write(angulo1);
    delay(1000);

    angulo2 = 50;
    servo2.write(angulo2);
    delay(1000);
  }

  // Golpe a la tecla con el servo2
  int golpe = min(angulo2 + 45, 180);
  servo2.write(golpe);
  delay(150);
  servo2.write(angulo2);
  delay(1000);

  // Volver a la posición base
  resetPosiciones();
  delay(5000);

  //LED se enciende 2 segundo
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
}

// Restablece las posiciones iniciales de todos los servos
void resetPosiciones() {
  angulo1 = 40;
  angulo2 = 50;
  angulo3 = 130;

  servo1.write(angulo1);
  servo2.write(angulo2);
  servo3.write(angulo3);
}