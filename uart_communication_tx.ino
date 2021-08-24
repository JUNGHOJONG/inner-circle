char message = 'A';

void setup() {
  Serial.begin(9600);
  Serial.println("UART");
}

void loop() {
//  Serial.println(message);
  Serial.write(message);

  message++;

  if (message > 'Z') {
    message = 'A';
  }
  
  delay(1000);
}
