// Declare R1/R2 ratio and variables
float R1_R2_ratio = 0.02;   // default R1/R2 ratio of 0.02 for 10mA output current limit

const float Rs = 10;        // resistance of current-sensing resistor
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
	float V_out = analogRead(A0) * 5.0/1023;	    // read op-amp output voltage
  float DC_current = V_out / Rs * R1_R2_ratio;  // convert op-amp output voltage to current
  
  int read_transistor_state = digitalRead(36);  // reads state of pin going to transistor
  if (read_transistor_state == 1) {             // if pin is high
    transistor_state="HIGH";                    // signal to transistor is high 
  } else {transistor_state="LOW";}              // signal to transsitor is low

  if (DC_current != DC_current_old) {           // if previous current value is not equal to new current value
    DC_current_old = DC_current;                // update previous current value
    // print outputs
    Serial.print("Op-amp output voltage: ");
    Serial.print(V_out);
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
  if (V_out >= 5) {	                  // if op-amp output voltage is greater than or equal to 5V
		digitalWrite(36, HIGH);	          // send signal to transistor to activate safety relay
	} else {digitalWrite(36, LOW);      // no signal sent to transistor
	}
  
}

