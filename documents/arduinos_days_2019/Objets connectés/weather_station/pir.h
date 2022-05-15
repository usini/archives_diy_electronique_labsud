void startPIR() {
  pinMode(PIN_PIR, INPUT);
}

void getPIR(bool &pirState) {
  pirState = digitalRead(PIN_PIR);
}
