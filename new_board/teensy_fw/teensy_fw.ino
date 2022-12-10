void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<5; i++){
    pinMode(i, OUTPUT);
  }

  pinMode(14, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<5; i++){
    digitalWrite(i, 1);
  }

//  analogRead(0);
//  digitalWrite(0, 0);
//  delayMicroseconds(1);
  analogRead(0);

  for(int i=0; i<5; i++){
    digitalWrite(i, 0);
  }
  delayMicroseconds(1);
}
