#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int numInputs = 2;
const int inpPins[numInputs] = {6, 7};

int screenState = 0;
int prevScreenState = 0;
char noteVal = 'a';
int tempoVal = 1;
int inpCount = 0;

bool takeInp = false;
bool zeroVal = false;

int inpState[numInputs];
int prevState[numInputs] = {LOW,LOW};
bool inpRead[numInputs] = {LOW,LOW};

int cursorX = 0;
int cursorY = 0;
int blinkX = cursorX;
int blinkY = cursorY;

unsigned long lastDebounceTime[numInputs] = {0,0};
unsigned long debounceDelay = 50;

//elements 0-9 for holding notes and their associated tempos
char notes[10] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
int tempo[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int compNotes[10];
int compTempo[10];

int stepNum = 6;

void setup() {
  for(int i = 0; i < numInputs; i++) {
    pinMode(inpPins[i], INPUT);
    digitalWrite(inpPins[i], HIGH);
  }
  Serial.begin(9600);
  lcd.begin(16, 2);
}


void compose(){ //note values taken from lab 4
  for(int i = 0; i < 10; i++){
    if(notes[i] == 'a'){
      compNotes[i] = 440;
    }else if(notes[i] == 'b'){
      compNotes[i] = 466;
    }else if(notes[i] == 'c'){
      compNotes[i] = 261;
    }else if(notes[i] == 'd'){
      compNotes[i] = 294;
    }else if(notes[i] == 'e'){
      compNotes[i] = 329;
    }else if(notes[i] == 'f'){
      compNotes[i] = 349;
    }else if(notes[i] == 'g'){
      compNotes[i] = 391;
    }

    if(tempo[i] == 1){
      compTempo[i] = 1000;
    }else if(tempo[i] == 2){
      compTempo[i] = 500;
    }else if(tempo[i] == 4){
      compTempo[i] = 250;
    }else if(tempo[i] == 8){
      compTempo[i] = 125;
    }
  }
}

void rehearse(){
  for(int i = 0; i < 10; i++){
   tone(13, compNotes[i], compTempo[i]);
   delay(compTempo[i]);
  }
}


void loop() {
  getInpRead();
  resolveinpRead();
}

void getInpRead() {
  for(int i = 0; i < numInputs; i++) {
    int reading = digitalRead(inpPins[i]);
    if (reading != prevState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != inpState[i]) {
        inpState[i] = reading;
        if (inpState[i] == HIGH) {
          inpRead[i] = HIGH;
        }
      }
    }
    prevState[i] = reading;
  }
}

void resolveinpRead() {
  for(int i = 0; i < numInputs; i++) {
    if(inpRead[i] == HIGH) {
      updateScreenState(i); 
      //printString();
      inpRead[i] = LOW;
    }
  }
}

void nextNote() {

  if(noteVal == 'a') {
    noteVal = 'b';
  }else if(noteVal == 'b') {
    noteVal = 'c';
  }else if(noteVal == 'c') {
    noteVal = 'd';
  }else if(noteVal == 'd') {
    noteVal = 'e';
  }else if(noteVal == 'e') {
    noteVal = 'f';
  }else if(noteVal == 'f') {
    noteVal = 'g';
  }else if(noteVal == 'g') {
    noteVal = 'a';
  }
}

void nextTempo() {
  if(tempoVal == 1){
    tempoVal = 2;
  }else if(tempoVal == 2){
    tempoVal = 4;
  }else if(tempoVal == 4){
    tempoVal = 8;
  }else if(tempoVal == 8){
    tempoVal = 1;
  }
}


void updateScreenState(int input) {
if(input == 0) {
   if(screenState > 2 & screenState < 26) {
     //nextNote();
     updateScreenValues();
     if(stepNum < 16){
     lcd.setCursor(stepNum, 0);
     }else if(stepNum > 15){
      lcd.setCursor((stepNum - 10), 1);
     }
     if(screenState == 23){
      lcd.noCursor();
      rehearse();
     }
     
   }
   else if (screenState == 1) {
     screenState = 2;
     updateScreen();
   }
   else {
    //screenState = 2;
    
   }
 }else if(input == 1) { 
    if(screenState == 0 || screenState == 1 || screenState == 24) {
      screenState = 2;
      updateScreen();
    }else{
      screenState++;
      noteVal = 'a';
      tempoVal = 1;
      updateScreen();
    }
  }
}

void setCursorValues( int cursorX, int cursorY) {
  lcd.setCursor(cursorX, cursorY);
  cursorX = cursorX;
  cursorY = cursorY;
}

void updateScreenValues() {
  if(stepNum < 16) {
    nextNote();
    lcd.print(noteVal);
    Serial.println("changed noteVal to: ");
    Serial.println(noteVal);
    notes[inpCount] = noteVal;
  }else if(screenState != 23){
    nextTempo();
    lcd.print(tempoVal);
    Serial.println("changed tempoVal to: ");
    Serial.println(tempoVal);
    Serial.print("inpCount = ");
    Serial.print(inpCount);
    tempo[(inpCount - 10)] = tempoVal;
  }
}
void updateScreen() {
  
    if(screenState == 2){ //Screen lets you know its a binary calc.. the best one -- welcomes are lame
        takeInp = false;
        lcd.clear();
        setCursorValues(0, 0);
        lcd.print("Diet Mozart v1");   
        setCursorValues(0, 1);
        lcd.print("Press to compose");
    }
    
    //change notes screen
    if(screenState == 3){
        takeInp = true;
        lcd.clear();
        setCursorValues(0, 0);          
        lcd.print("Notes:aaaaaaaaaa");
        setCursorValues(0, 1);
        lcd.print("Tempo:1111111111");
        setCursorValues(6, 0);
        inpCount = 0;
        lcd.blink();
    }
        //input counter changes with screenState and where the cursor is for maximum reliability
    if(screenState == 4) {
      setCursorValues(7, 0);
     inpCount = 1;
     stepNum = 7;
    }
     if(screenState == 5) {
        setCursorValues(8, 0);
      inpCount = 2;
      stepNum = 8;
    }
    if(screenState == 6) {
        setCursorValues(9, 0);
      inpCount = 3;
      stepNum = 9;
    }
    if(screenState == 7) {
        setCursorValues(10, 0);
     inpCount = 4;
     stepNum = 10;
    }
    if(screenState == 8) {
        setCursorValues(11, 0);
      inpCount = 5;
      stepNum = 11;
    }
    if(screenState == 9) {
        setCursorValues(12, 0);
     inpCount = 6;
     stepNum = 12;
    }
    if(screenState == 10) {
        setCursorValues(13, 0);
     inpCount = 7;  
     stepNum = 13;
    }
    if(screenState == 11) {
        setCursorValues(14, 0);
     inpCount = 8; 
     stepNum = 14; 
    }
    if(screenState == 12) {
        setCursorValues(15, 0);
     inpCount = 9;  
     stepNum = 15;
    }
  
    //enter byte 2
    if(screenState == 13){
        setCursorValues(6, 1);          
        stepNum = 16;
        inpCount = 10;
        Serial.print("notes: ");
        for(int x = 0; x < 10; x++){
        Serial.print(notes[x]);
        }
    }
        
    if(screenState == 14) {
      setCursorValues(7, 1);
      inpCount = 11;
      stepNum = 17;
    }
     if(screenState == 15) {
      setCursorValues(8, 1);
      inpCount = 12;
      stepNum = 18;
    }
    if(screenState == 16) {
        setCursorValues(9, 1);
      inpCount = 13;
      stepNum = 19;
    }
    if(screenState == 17) {
        setCursorValues(10, 1);
      inpCount = 14;
      stepNum = 20;
    }
    if(screenState == 18) {
        setCursorValues(11, 1);
      inpCount = 15;
      stepNum = 21;
    }
    if(screenState == 19) {
        setCursorValues(12, 1);
      inpCount = 16;
      stepNum = 22;
    }
    if(screenState == 20) {
        setCursorValues(13, 1);
      inpCount = 17;
      stepNum = 23;
    }  
    if(screenState == 21) {
        setCursorValues(14, 1);
      inpCount = 18;
      stepNum = 24;
    }  
    if(screenState == 22) {
        setCursorValues(15, 1);
      inpCount = 19;
      stepNum = 25;
        Serial.print("tempo: ");
        for(int y = 0; y < 10; y++){
        Serial.print(tempo[y]);
        }
    }  
    if(screenState == 23){
        lcd.clear();
        compose();          
        lcd.print("Mozart approves!");
        setCursorValues(0, 1); 
        lcd.print("LeftBtn=Replay!");
        lcd.noCursor();
        rehearse();
        }
        
    
     
      if(screenState == 24){
        lcd.clear();
        setCursorValues(0, 0);          
        lcd.print("Thanks for using");
        setCursorValues(0, 1); 
        lcd.print("Diet Mozart!!");
      }

}



