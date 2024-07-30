#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 4, 32, 30, 28, 26);
int threshold = 60;
int green_LED = 6;
int yellow_LED = 7;
int red_LED = 8;
int triggerPin =9;
int echoPin = 10;
int buzzer = 11;
int buttonPin = 3;
int switchPin = 2;
float duration = 0;
float distance = 0;
int initial = 0;
int forward = 0;
int LDR_pin = A0;
volatile bool manualMode = false;
volatile int mode = 0; // 0: Night mode, 1: Pedestrian mode, 2: Regular mode
void buttonPressed();
bool nightMode();
bool pedestrianMode();
void pedestrianResponse();


void setup() {
  Serial.begin(9600);
  lcd.begin(20, 2);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(green_LED, OUTPUT);
  pinMode(yellow_LED, OUTPUT);
  pinMode(red_LED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LDR_pin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);

  // Set up interrupt for the button
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);
}

void loop() {
    bool switchState = digitalRead(switchPin);

      if (switchState == HIGH) {
  bool ng = nightMode();
  bool ped = pedestrianMode();

  if (manualMode) {
    // In manual mode
    switch (mode) {
      case 0: // Night mode
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Manual (Night):"); 
        
        if (ped) {
          lcd.setCursor(0, 1); 
          lcd.print("Pedestrian Cross"); 
          digitalWrite(green_LED, HIGH);
          digitalWrite(red_LED, LOW);
        } else {
          lcd.setCursor(0, 1); 
          lcd.print("No Pedestrian"); 
          digitalWrite(red_LED, HIGH);
          digitalWrite(green_LED, LOW);
        }
        break;
      case 1: // Pedestrian mode
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Manual (Day):"); 
        lcd.setCursor(0, 1); 
        lcd.print("Pedestrian");        
        pedestrianResponse();
        break;
      case 2: // Regular mode
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Manual (Day):"); 
        lcd.setCursor(0, 1); 
        lcd.print("Regular Mode"); 
        if (initial == 0) {
          lcd.clear();
          digitalWrite(red_LED, HIGH);
          lcd.print("Stop!");
          delay(2000);
          digitalWrite(red_LED, LOW);
          initial = 1;
          forward = 1;
        } else if (initial == 1 && forward == 1) {
          lcd.clear();
          digitalWrite(yellow_LED, HIGH);
          lcd.print("Caution..");
          delay(1000);
          digitalWrite(yellow_LED, LOW);
          initial = 2;
        } else if (initial == 1 && forward == 0) {
          lcd.clear();
          digitalWrite(yellow_LED, HIGH);
          lcd.print("Caution..");        
          delay(1000);
          digitalWrite(yellow_LED, LOW);
          initial = 0;
        } else if (initial == 2) {
          lcd.clear();
          digitalWrite(green_LED, HIGH);
          lcd.print("Go!");
          delay(2000);
          digitalWrite(green_LED, LOW);
          initial = 1;
          forward = 0;
        }
        break;
    }
  } else {
    // In automatic mode
    if (!ped && !ng) {
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Auto (Day):"); 
        lcd.setCursor(0, 1); 
        lcd.print("Regular Mode"); 
        if(initial==0){
     lcd.clear();
     digitalWrite(red_LED, HIGH);
     lcd.print("Stop!");
     delay(2000);
    digitalWrite(red_LED,LOW);
    initial=1;
    forward=1;
    }
    
    else if(initial==1 && forward==1){
          lcd.clear();
          digitalWrite(yellow_LED, HIGH);
          lcd.print("Caution..");
     delay(1000);
     digitalWrite(yellow_LED,LOW);
     initial=2;
    }
    else if(initial==1 && forward==0){
          lcd.clear();
          digitalWrite(yellow_LED, HIGH);
          lcd.print("Caution..");
     delay(1000);
     digitalWrite(yellow_LED,LOW);
    initial=0;
    }
    
    else if(initial==2){
          lcd.clear();
          digitalWrite(green_LED, HIGH);
          lcd.print("Go!");
    delay(2000);
    digitalWrite(green_LED,LOW);
    initial=1;
    forward=0;
    }
  }   
  else if (ng && ped) {
      lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Auto (Night):"); 
        lcd.setCursor(0, 1); 
        lcd.print("Pedestrian Cross");
        digitalWrite(green_LED, HIGH);
        digitalWrite(red_LED, LOW);
    } else if (ng && !ped) {
      lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Auto (Night):"); 
        lcd.setCursor(0, 1); 
        lcd.print("No Pedestrian");
        digitalWrite(red_LED, HIGH);
        digitalWrite(green_LED, LOW);
    } else if (ped && !ng) {
      lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Auto (Day):"); 
        lcd.setCursor(0, 1); 
        lcd.print("Pedestrian");  
        pedestrianResponse();
    }
  }
}
else{
  lcd.clear();
}
}

void buttonPressed() {
  manualMode = !manualMode; // Toggle manual mode
  Serial.println(mode);
  mode = (mode + 1) % 3;
  Serial.println(mode);
}

void pedestrianResponse() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(yellow_LED, HIGH);
    delay(100);
    digitalWrite(yellow_LED, LOW);
    delay(100);
  }
  digitalWrite(buzzer, LOW);
}

bool nightMode() {
  int LDR = analogRead(LDR_pin);
  return LDR < threshold;
}

bool pedestrianMode() {
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;
  return distance < 10;
}
