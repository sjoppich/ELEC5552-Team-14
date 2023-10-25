
long analogPin = A3; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
long val = 0;  // variable to store the value read
float scaledval = 0; // scaled voltage read

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {

//voltage divider using R1=4.7K, R2=200
  val = analogRead(analogPin);  // read the input pin
  scaledval = val*(4900/200)*(5.0/1024);
  //Serial.println(val);          // debug value
  Serial.println(scaledval);          // debug value
  delay(50);
}