void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Listening...");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil("\n");
    command.trim();

    if (command == "LED_ON") {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Turned on...");
    } else if (command == "LED_OFF") {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Turned off...");
    } else if (command == "MR") {
      Serial.println("Moving right");
    } else if (command == "ML") {
      Serial.println("Moving left");
    } else if (command == "MU") {
      Serial.println("Moving up");
    } else if (command == "MD") {
      Serial.println("Moving down");
    } else if (command == "PK") {
      Serial.println("Pressing key");
    } 
  }
}
