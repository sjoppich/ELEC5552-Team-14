
#include <LiquidCrystal.h> 
#include <Encoder.h>
const int rs =10, en =9, d4 = 8, d5= 7, d6= 6,d7= 5; //pins for LCD
//all tutes say use SPI pins 11 and 12 but no idea how to interface with spi if i do that 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  //set up the LCD connections
const int RotarySW =2; 
Encoder knob(3,4);

const int ledPin = A2; //pin 2
const int Thermal_Protection = A3; //pin controlling the output switch
const int adjustment_switch = 11; //attched to physical switch powering encoder
const byte Thermistor = 0; // pin a0
long DC_voltage = A4; // voltage divider input to mcu

//Meni Reading parameters
int SWNow;
bool refreshLCD = false;
int newEncoder =0;
int oldEncoder =0;
//still need to add the other menu screens
int menu1_Value = 0; //value within menu 1 
int menu2_Value = 0; //value within menu 2
int menu3_Value = 0; //value within menu 3
int menu4_Value = 0; //value within menu 4
int menuCounter = 0;
int newMenuCount = 0;

String reading1_Value = "Test"; //the type of output, depends on output_type
int reading2_Value = 500; //Voltage reading
int reading3_Value = 10; //current reading
int reading4_Value = 30; //Temperature

String Output_type[6] = {"DC-LV", "DC-MV", "DC-HV", "AC-LV", "AC-MV", "AC-HV"}; //selected based on what the physical switches say

// thermal calc+ comparison
int Vo_Temp;
int Temp_Warning = 70;
int Temp_Limit = 80;
float R1 = 10000; // resistor value in voltage divider circuit
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// blinking LED 
int ledState = LOW; 
int protectionState = LOW;
unsigned long prevMills =0;
const long interval = 100; //blinking interval
const long check_interval = 1000; // interval between temp checks 

// DC voltage Calculation
long Vo_DC = 0;               //variable to store the value read 
float scaled_DC_voltage = 0;  // Scaled voltage read 
int R1_DC = 4700;
int R2_DC =200;

//Debouncing
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    // the debounce time; increase if the output flickers
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP); //Button
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(Thermal_Protection, OUTPUT);
  pinMode(adjustment_switch,INPUT);
  pinMode(Thermistor, INPUT); //not n original ref but think i need it 
  lcd.begin(20,4);
  printLCD();
 // attachInterrupt(digitalPinToInterrupt(RotarySW), MenuChange, RISING);

}

void loop() {
  // put your main code here, to run repeatedly:

  //Serial.print(newMenuCount);
  //if(refreshLCD == true){ // this was originally outside as a function but wasnt working haha

  MenuChange(); 
  updateLCD();
  updateCursorPosition();
  ParamaterChange();
  temp_check();
  //}
}

void ParamaterChange() {

  switch(menuCounter) //this checks the value of the counter (0, 1, 2 or 3)
    {
      case 0: //AC Amplitude need to add more precision
        newEncoder = knob.read();
        if(newEncoder > oldEncoder){ //if the input wants to increas
          if(menu1_Value<300){
            menu1_Value++;  
          }
          else{
            menu1_Value = 0;
          }   
        }
        else if(newEncoder < oldEncoder){
          if(menu1_Value>0){
            menu1_Value--;
          }
          else{
            menu1_Value =300;
          }
        }
        else{}
        oldEncoder = newEncoder;
        break;

      //-------------------------------
      case 1: // Current limit
        newEncoder = knob.read();
        if(newEncoder > oldEncoder){ //if the input wants to increas
          if(menu2_Value<10){
            menu2_Value = menu3_Value +2;  
          }
          else{
            menu2_Value = 2;
          }   
        }
        else if(newEncoder < oldEncoder){
          if(menu2_Value>2){
           menu2_Value = menu2_Value -2;
          }
          else{
            menu3_Value =10;
          }
        }
        else{}
        oldEncoder = newEncoder;
        break;

      case 2: //Current limit
        newEncoder = knob.read(); //AC  frequency
        if(newEncoder > oldEncoder){ //if the input wants to increas
          if(menu3_Value<300){
            menu3_Value =menu3_Value +10;  
          }
          else{
            menu3_Value = 50;
          }   
        }
        else if(newEncoder < oldEncoder){
          if(menu3_Value>50){
            menu3_Value = menu3_Value -10;
          }
          else{
            menu3_Value =300;
          }
        }
        else{}
        oldEncoder = newEncoder;
        break;

      //-------------------------------    
      case 3: //Modulated frequency
        newEncoder = knob.read();
        if(newEncoder > oldEncoder){ //if the input wants to increas
          if(menu4_Value<100){
            menu4_Value = menu4_Value +2;  
          }
          else{
            menu3_Value = 2;
          }   
        }
        else if(newEncoder < oldEncoder){
          if(menu4_Value>2){
           menu4_Value = menu4_Value -2;
          }
          else{
            menu4_Value =100;
          }
        }
        else{}
        oldEncoder = newEncoder;
        break;
  refreshLCD = true;

    }
}
//
void MenuChange(){ //changes the value of the menu counter 
  SWNow = digitalRead(RotarySW); //button has been pressed
    if (SWNow == HIGH) {
      newMenuCount++;
      if(newMenuCount <= 3){ //we do not go above 3
        menuCounter++;  
        }
        else{
          newMenuCount = 0;
          menuCounter = 0;
        } 
          }
    else{}
    menuCounter = newMenuCount;
    Serial.print(menuCounter);
  refreshLCD = true;     
}
/* Trying to fix debouncing
  SWNow = digitalRead(RotarySW); //button has been pressed still need to debounce
  if (SWNow != lastButtonState){
    lastDebounceTime = millis();
  }
  if((millis()-lastDebounceTime)> debounceDelay){
    if (SWNow != buttonState){
      buttonState = SWNow;
    if (buttonState == HIGH) {
      newMenuCount++;
      if(newMenuCount <= 3){ //we do not go above 3 in current version
        menuCounter++;  
        }
        else{
          newMenuCount = 0;
          menuCounter = 0;
        } 
          }
    else{}
    menuCounter = newMenuCount;
    lastButtonState = SWNow;      
    }
  }*
  //NDEBOUNCING STILL NOT WORKING, SO CURRENTLY HAS NO DEBOUNCNG. CODE ABOVE TRYING TO FIX (FAILING TO)
*/


void printLCD() { //static information thats not movind 
  lcd.setCursor(0,0); //1st line, 2nd block
  lcd.print("O:"); //text
  //----------------------
  lcd.setCursor(0,1); //2nd line, 2nd block
  lcd.print("V:"); //text
  //----------------------
  lcd.setCursor(0,2); //3rd line, 2nd block
  lcd.print("I:  mA"); //text
  //----------------------
  lcd.setCursor(0,3); //4th line, 2nd block
  lcd.print("T:"); //text

  //---------------------- 
  lcd.setCursor(10,0); //1st line, 14th block
  lcd.print("VAC:"); //  
    //---------------------- 
  lcd.setCursor(10,1); //1st line, 14th block
  lcd.print("CURLIM:"); //
  //----------------------
  lcd.setCursor(11,2); //1st line, 14th block
  lcd.print("ACHZ:"); //
    //----------------------
  lcd.setCursor(10,3); //1st line, 14th block
  lcd.print("MODHZ:"); //
}

void updateCursorPosition() {
  //Clear display's ">" parts 
  lcd.setCursor(9,0); //1st line, 1st block
  lcd.print(" "); //erase by printing a space
  lcd.setCursor(9,1); //1st line, 1st block
  lcd.print(" "); //erase by printing a space
  lcd.setCursor(9,2);
  lcd.print(" "); 
  lcd.setCursor(9,3);
  lcd.print(" "); 
     
  //Place cursor to the new position
  switch(menuCounter) //this checks the value of the counter (0, 1, 2 or 3)
  {
    case 0:
    lcd.setCursor(9,0); //4th line, 1st block
    lcd.print(">"); 
    //-------------------------------
    case 1:
    lcd.setCursor(9,1); //1st line, 1st block
    lcd.print(">"); 
    break;
    //-------------------------------
    case 2:
    lcd.setCursor(9,2); //2nd line, 1st block
    lcd.print(">"); 
    break;
    //-------------------------------    
    case 3:
    lcd.setCursor(9,3); //3rd line, 1st block
    lcd.print(">"); 
    break;
    //-------------------------------    

  }
}

void updateLCD() {  //Refreshes the LCD with new parameters and readings
  //Parameters 
  lcd.setCursor(14,0); //1st line, 10th block
  lcd.print("   "); //erase the content by printing space over it
  lcd.setCursor(14,0); //1st line, 10th block
  lcd.print(menu1_Value); //AC amplitude

  lcd.setCursor(17,1);
  lcd.print("   ");
  lcd.setCursor(17,1);
  lcd.print(menu2_Value); // Current limit
  
  lcd.setCursor(16,2);
  lcd.print("   ");
  lcd.setCursor(16,2);
  lcd.print(menu3_Value); // AC frequency

  lcd.setCursor(17,3);
  lcd.print("   ");
  lcd.setCursor(17,3);
  lcd.print(menu4_Value);   // Modulation frequency

  //Measurements
  lcd.setCursor(2,0); 
  lcd.print("     "); 
  lcd.setCursor(2,0); 
  lcd.print(reading1_Value); 

  lcd.setCursor(2,1);
  lcd.print("      ");
  lcd.setCursor(2,1);
  lcd.print(reading2_Value); // Current limit
  
  lcd.setCursor(2,2);
  lcd.print("  ");
  lcd.setCursor(2,2);
  lcd.print(reading3_Value); // AC frequency

  lcd.setCursor(2,3);
  lcd.print("  ");
  lcd.setCursor(2,3);
  lcd.print(reading4_Value); //temperature
  }

void temp_check() {
  // reads the thermistor, calculates the temp
  Vo_Temp = analogRead(Thermistor); //Reads voltage 
  R2 = R1*(1023.0 / (float)Vo_Temp - 1.0); 
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;         //Calculates Temperature
  reading4_Value = T;
  Serial.print("Temp:  "); //for fault finding
  Serial.print(T); 
  digitalWrite(ledPin,ledState); ///visual indication
  Serial.print("  ");
  Serial.print(ledState);
  digitalWrite(Thermal_Protection,protectionState); 
                                // to output switch
  unsigned long currentMills = millis();
  // flashes LED if getting too hot, solid on 
  //if the overheating protection enabled
  if(T>= Temp_Limit) {          //Temp_Limit =80
    ledState = HIGH;            //LED on
    protectionState = HIGH;     //shuts off output 
  } else if(T>=Temp_Warning) {  //Temp_Warning =70s 
      protectionState = LOW;    
        ledState = HIGH;
      }
  

}