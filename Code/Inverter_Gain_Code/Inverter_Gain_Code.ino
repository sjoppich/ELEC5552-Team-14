// This code was used to validate the low voltage AC on an Arduino Leonardo
#include <AD9833.h>     // Include the library

#define FNC_PIN 13      // 
#define CS_PIN 8        // Chip Select for X9C103
#define UD_PIN 9        // Up/Down control for X9C103
#define INC_PIN 10      // Increment control for X9C103

AD9833 gen(FNC_PIN);     // Defaults to 25MHz internal reference frequency

long rate = 50000;
int potentiometer_value = 0;
int x9cResistance = 10040;  // Initialized to the max value (10k + 40 ohms wiper resistance)

unsigned long lastFreqChange = 0;
const unsigned long freqChangeInterval = 10000; // 10 seconds

void setup() {
    pinMode(CS_PIN, OUTPUT);
    pinMode(UD_PIN, OUTPUT);
    pinMode(INC_PIN, OUTPUT);

    // Set initial pin states
    digitalWrite(CS_PIN, HIGH);  // Deselect chip
    digitalWrite(INC_PIN, LOW);

    // Initialize X9C103 to minimum resistance
    initializePotentiometer();

    gen.Begin();              
    gen.ApplySignal(SINE_WAVE, REG0, rate);
    gen.EnableOutput(true);

    pinMode(A0, INPUT);
    Serial.begin(9600);
}

void loop() {
    potentiometer_value = analogRead(A0);
    adjustPotentiometer(potentiometer_value);

    // Change frequency every 10 seconds between 50kHz and 300kHz to demonstrate performance at minimum and maximum frequency
    if (millis() - lastFreqChange >= freqChangeInterval) {
        rate = (rate == 50000) ? 300000 : 50000; // Toggle between the two frequencies
        gen.ApplySignal(SINE_WAVE, REG0, rate);
        lastFreqChange = millis();
    }

    float frequency_kHz = rate / 1000.0;  
    Serial.print("Frequency (kHz): ");
    Serial.println(frequency_kHz);
    
    float gainEstimate = x9cResistance / 1000.0;
    Serial.print("Estimated Gain: ");
    Serial.println(gainEstimate, 2);

    delay(2000); 
}

void initializePotentiometer() {
    digitalWrite(UD_PIN, LOW);  // Set to decrement
    digitalWrite(CS_PIN, LOW);  // Select the chip

    for (int i = 0; i < 100; i++) {  // Make potentiomter minimum value (100 steps)
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
    }
  
    digitalWrite(CS_PIN, HIGH);  // Deselect and store the new value
    delay(20);  // Wait for tCPH minimum defined by datasheet(CS Deselect Time with STORE)
}

void adjustPotentiometer(int potValue) {
    float rangeFifth = 1023.0 / 5;

    if (potValue < rangeFifth) {
        decrementPotFast();
    } else if (potValue < 2 * rangeFifth) {
        decrementPotSlow();
    } else if (potValue < 3 * rangeFifth) {
        // Middle fifth - do nothing} 
        else if (potValue < 4 * rangeFifth) {
        incrementPotSlow();  } 
        else {
        incrementPotFast(); }}
        
void incrementPotFast() {
    if (x9cResistance < 10040) { // 10k + 40 ohms wiper resistance
        digitalWrite(UD_PIN, HIGH);
        changeWiper(5);  // Adjust the value as needed
        x9cResistance += 100; // Each step is approx 100 ohms for X9C103 }}

void decrementPotFast() {
    if (x9cResistance > 140) {  // 100 + 40 ohms wiper resistance
        digitalWrite(UD_PIN, LOW);
        changeWiper(5);
        x9cResistance -= 100;}}

void incrementPotSlow() {
    if (x9cResistance < 10040) {
        digitalWrite(UD_PIN, HIGH);
        changeWiper(1);
        x9cResistance += 100;}}

void decrementPotSlow() {
    if (x9cResistance > 140) {
        digitalWrite(UD_PIN, LOW);
        changeWiper(1);
        x9cResistance -= 100;
    }
}

void changeWiper(int steps) {
    digitalWrite(CS_PIN, LOW);
    for (int i = 0; i < steps; i++) {
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
    }
    digitalWrite(CS_PIN, HIGH);
    delay(20);  // Wait for tCPH
}
