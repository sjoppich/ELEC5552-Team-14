// Set output current limit and declare variables
float current_limit = 1e-2;	// set current limit to 10mA
float DC_current_old;       // previous DC current value
String transistor_state;    // state of signal going to transistor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   // set data rate to 9600 bps
  pinMode(36, OUTPUT);  // set digital pin 36 as output (controls transistor)
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // DC current measurement
	float V1_scaled = analogRead(A2) * 5.0/1023;	// read scaled voltage before current-sensing resistor
	float V2_scaled = analogRead(A3) * 5.0/1023;	// read scaled voltage after current-sensing resistor
  float voltage_drop = V1_scaled - V2_scaled;   // difference between scaled voltages
  float DC_current = voltage_drop * 48 / 10;	  // convert voltage drop to current by scaling it up to original voltage and dividing by 10 Ohm current-sensing resistor
  
  int read_transistor_state = digitalRead(36);  // read state of pin going to transistor
  if (read_transistor_state == 1) {             // if pin is high
    transistor_state="HIGH";                    // signal to transistor is high 
  } else {transistor_state="LOW";}              // signal to transsitor is low

  if (DC_current != DC_current_old) {           // if previous current value is not equal to new current value
    DC_current_old = DC_current;                // update previous current value
    // print outputs
    Serial.print("Scaled Voltages: ");
    Serial.print(V1_scaled); 
    Serial.print("V");
    Serial.print(", ");
    Serial.print(V2_scaled);
    Serial.print("V ");
    Serial.print("--> Voltage drop: ");
    Serial.print(voltage_drop);
    Serial.print("V");
    Serial.println();
    Serial.print("DC Current: ");
    Serial.print(DC_current);
    Serial.print("A");
    Serial.println(); 
    Serial.print("Transistor signal: ");
    Serial.print(transistor_state);
    Serial.println();
    Serial.println();
    }  else {};

  // Protection cutoff
  if (DC_current >= current_limit) {	// if current is greater than/equal to current limit
		digitalWrite(36, HIGH);	          // send signal to transistor to activate safety relay
	} else {digitalWrite(36, LOW);      // no signal sent to transistor
	}
  
}

