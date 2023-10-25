/*
 * Waveform_Generator_Manual_Variable_Potentiometer.ino
 * 2018 WLWilliams
 * 
 * This sketch demonstrates the basic use of the AD9833 DDS module library.
 * Using the ApplySignal to generate and/or change the signal.
 * 
 * This program is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version. 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of
 * the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * This example code is in the public domain.
 * 
 * Library code found at: https://github.com/Billwilliams1952/AD9833-Library-Arduino
 * 
 */

// The original code only includes the #include,#define, AD9833 gen.int rate, void gen.ApplySignal, gen.EnableOutput statements. 
// All other code is original work
#include <AD9833.h>     // Include the library

#define FNC_PIN 13       

AD9833 gen(FNC_PIN);       // Defaults to 25MHz internal reference frequency

long rate = 50000;
int potentiometer_value = 0;
int waveform_voltage   = 0;

void setup() {
    // This MUST be the first command after declaring the AD9833 object
    gen.Begin();              

    gen.ApplySignal(SINE_WAVE,REG0,rate);
   
    gen.EnableOutput(true);   // Turn ON the output - it defaults to OFF

    pinMode(A0,INPUT);
    pinMode(A5,INPUT);

    Serial.begin(9600);
}

void loop() {
    potentiometer_value = analogRead(A0);
    int waveform_value   = analogRead(A5);
    
    float potentiometer_voltage_mV = (potentiometer_value / 1023.0) * 5000;
    float waveform_voltage_mV = (waveform_value / 1023.0) * 5000; 
     
    //Frequency demarcations (50kHz to 300kHz in increments of 10kHz)
    int segments = 30;  // There are 26 states (including 0)
    int segmentSize = 1024 / segments;  // Use 1024 for accurate division, considering 0-1023 range
    long baseValue = 50000;
    long increment = 10000;
    int segment = potentiometer_value / segmentSize;
    rate = baseValue + segment * increment;

    if (rate > 300000){ 
        rate = 300000;
    }
    if (rate < 10000){ 
        rate = 10000;
    }
    float frequency_kHz = rate / 1000;   
    gen.ApplySignal(SINE_WAVE,REG0,rate);

    Serial.print("Frequency (kHz): ");
    Serial.println(frequency_kHz);
    
    Serial.print("Potentiometer Value: ");
    Serial.println(potentiometer_value);  

    Serial.print("Potentiometer Voltage (mV): ");
    Serial.println(potentiometer_voltage_mV, 3);  // 3 decimal places for accuracy
    
    Serial.print("Waveform Raw Value: ");
    Serial.println(waveform_value);

    Serial.print("Waveform Voltage (mV): ");
    Serial.println(waveform_voltage_mV, 3); // 3 decimal places for accuracy

    delay(2000);  // Optional delay for easier reading of the serial monitor
}
