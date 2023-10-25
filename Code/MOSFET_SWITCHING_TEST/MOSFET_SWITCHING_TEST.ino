/*
TESTING CODE FOR MOSFET SWITCHING
DESIGN 2 - 2023 SEM 2

DIGITAL PIN D8 FOR MOSFET SWITCHING OUTPUT
*/

int GateH = 8; // FET connected to Digital Pin 8
int GateL = 7;
unsigned long dutycycle = 0.5; // variable to store switch input
unsigned long frequency = 1; //Hz - adjust to change switching frequency
unsigned long OnDe = long((1000 / frequency) * dutycycle);
unsigned long OfDe = long((1000 / frequency) * (1-dutycycle));

void setup() {
  pinMode(GateH, OUTPUT); //Sets the MOSFET as OUTPUT
  pinMode(GateL, OUTPUT); //Sets the MOSFET as OUTPUT

}

void loop() {
  
  digitalWrite(GateH, HIGH);
  digitalWrite(GateL, LOW);  
  delay(5); // Approximately 10% duty cycle @ 1KHz
  digitalWrite(GateH, LOW);
  digitalWrite(GateL, HIGH);
  delay(5);
  
  //digitalWrite(GateH, HIGH);
  //digitalWrite(GateL, HIGH);  
}