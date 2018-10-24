int clockPin = 9;
int dataPin = 10;

void setup() {
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  //count up routine
  for (int j = 1; j < 256;) {
    shiftOut(dataPin, clockPin, MSBFIRST, j);
    j = j * 2;
    delay(200);
  }
}
