void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    Serial.println("=== receive ===");
    Serial.println(Serial.read());
    Serial.println("=== ===");
  }
}
