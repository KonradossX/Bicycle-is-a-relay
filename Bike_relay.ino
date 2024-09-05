// Arduino PINs
#define digital  2
#define relayL   7
#define relayN   6
#define ledR     9  // 120 ohm
#define ledG     10 // 60 ohm
#define ledB     11 // 60 ohm
#define highPin  8

// Relay
#define relayOFF HIGH
#define relayON  LOW

// LED
#define ledON    LOW
#define ledOFF   HIGH

int   diameter       = 68;   // cm
float turnRelayOnIf  = 40.0; // km/h
float turnRelayOffIf = 20.0; // km/h

//----------------------------------

float length   = 0; 
float speed    = 0;

bool detected        = false;
bool stopped         = false;
bool serialActive    = true;
bool isOn            = false;

unsigned long timer;
unsigned long delta;
unsigned int waitTime = 2000;

//Funkcja rozruchowa
void setup() {
  pinMode(digital, INPUT_PULLUP); //-------------------------
  //pinMode(ledMain,    OUTPUT);
  pinMode(relayL, OUTPUT);
  pinMode(relayN, OUTPUT);
  
  pinMode(ledR, OUTPUT);
  digitalWrite(ledR, HIGH);
  pinMode(ledG, OUTPUT);
  digitalWrite(ledG, HIGH);
  pinMode(ledB, OUTPUT);
  digitalWrite(ledB, HIGH);
  pinMode(highPin, OUTPUT);
  digitalWrite(highPin, HIGH);
  
  digitalWrite(relayL, relayOFF);
  digitalWrite(relayN, relayOFF);
 
  Serial.begin(9600);

  length = calLength(diameter) * 3600.0;
  //Serial.println(length);
  timer = millis();
  
  while(false){
    digitalWrite(ledR, ledON);
    delay(200);
    digitalWrite(ledR, ledOFF);
    digitalWrite(ledG, ledON);
    delay(200);
    digitalWrite(ledG, ledOFF);
    digitalWrite(ledB, ledON);
    delay(200);
    digitalWrite(ledB, ledOFF);
  }  
}

//Nieskończona pętla główna
void loop() {
  calSpeed();
  if (speed > turnRelayOnIf) {
    turnOn();
  }

  if (speed < turnRelayOffIf) {
    turnOff();
  }
}

//Wyliczanie prędkości roweru
void calSpeed() {
  if (digitalRead(digital) == LOW) {
    if (detected == false) {
      detected = true;
      //digitalWrite(ledMain, HIGH);--------------------------
      
      delta = millis() - timer;
      timer = millis();
      
      //speed =  3.6 * (2.13 / float(delta * 0.001));
      speed = length / float(delta);
      
      if (serialActive) {
        Serial.print("V = ");
        Serial.print(speed);
        Serial.print("\tD = ");
        Serial.print(delta);
        Serial.print(" \t");
        Serial.println(isOn);
      }
    }
    digitalWrite(ledR, ledOFF);
    stopped = false; 
  } else {
    detected = false;
  }

  if (serialActive) {
    if (millis() - timer > waitTime) {
      //if (!stopped) {
        Serial.println("V = 0\tD = null");
        timer = millis();
        speed = 0;
        
        digitalWrite(ledR, ledON);
        digitalWrite(ledG, ledOFF);
        digitalWrite(ledB, ledOFF);

        stopped = true;
      //}
    }
  }
}

//Włączanie przekażników
void turnOn() {
  digitalWrite(relayL, relayON);
  digitalWrite(relayN, relayON);
  digitalWrite(ledG,   ledON);
  digitalWrite(ledB,   ledOFF);

  if (serialActive) {
    if (isOn == false) {
      Serial.println("ON");
      isOn = true;
    }
  }
}

//Wyłączanie przekażników
void turnOff() {
  digitalWrite(relayL, relayOFF);
  digitalWrite(relayN, relayOFF);
  digitalWrite(ledG,   ledOFF);

  if(!stopped)
    digitalWrite(ledB,   ledON);

  if (serialActive) {
    if (isOn == true) {
      Serial.println("OFF");
      isOn = false;
    }
  }
}

//Liczniee obwodu koła ze wzoru L = 2πr = πd
//Wejście to średnica w cm. Wynik to obwód w metrach
float calLength(float D) {
  return PI * D / 100;
  // return 2 * PI * R / 100;
}
