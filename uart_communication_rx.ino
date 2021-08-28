void setup() {
  Serial.begin(9600);
  Serial.println("Start Receive");
}

void loop() {
  if (Serial.available()) {
    Serial.println("=== receive ===");
    char receivedData = Serial.read();
    Serial.println(receivedData);
    Serial.println("=== ===");
  }
}
