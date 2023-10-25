/*
TESTING CODE FOR RELAY SWITCHING
DESIGN 2 - 2023 SEM 2

DIGITAL PIN D8 FOR RELAY SWITCHING OUTPUT
DIGITAL PIN D7 FOR SWITCHED INPUT
*/

int RelayCoil = 8; // Coil connected to Digital Pin 8
int Switch = 7; // Switch connected to Digital Pin 7

int read = 0; // variable to store switch input

void setup() {
  pinMode(RelayCoil, OUTPUT); //Sets the RelayCoil as OUTPUT
  pinMode(Switch, INPUT);  //Sets the Switch as INPUT
  Serial.begin(9600); //Setup Serial

}

void loop() {
  read = digitalRead(Switch); // reads the switch input
  digitalWrite(RelayCoil, read); // sets the relay coil equal to input
  Serial.println(read);
}