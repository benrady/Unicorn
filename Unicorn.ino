// vim: ft=cpp

const int PING_PIN = 3;
const int TURN_RIGHT_PIN = 5;
const int TURN_LEFT_PIN = 6;
const int BACKWARD_PIN = 7;
const int FORWARDS_PIN = 8;
const int LED_RED_PIN = 12;
const int LED_GREEN_PIN = 10;
const int LED_BLUE_PIN = 9; 
const int LED_POWER = 11; 

const int RC_GO = 1;
const int RC_RETREAT = 2;
const int RC_PATROL = 3;
const int SAFE_DISTANCE = 18; // Inches
const int ATTACK_DISTANCE = 40;
const int MAX_SAMPLES = 10;

int mode = RC_GO;
int currentSample = 0;
unsigned int samples[MAX_SAMPLES];
int red = 255;
int green = 255;
int blue = 255;

void updateLED() {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

void setup() {
  Serial.begin(9600);
  pinMode(TURN_RIGHT_PIN, OUTPUT); 
  pinMode(TURN_LEFT_PIN, OUTPUT); 
  pinMode(BACKWARD_PIN, OUTPUT); 
  pinMode(FORWARDS_PIN, OUTPUT); 
  digitalWrite(BACKWARD_PIN, LOW);
  digitalWrite(FORWARDS_PIN, LOW);
  digitalWrite(TURN_RIGHT_PIN, LOW);
  digitalWrite(TURN_LEFT_PIN, LOW);
  analogWrite(LED_POWER, 255);
}

void color(int r, int g, int b) {
  red = 255 - r;
  green = 255 - g;
  blue = 255 - b;
}

int sampleDistance() {
  currentSample++;
  if (currentSample >= MAX_SAMPLES) {
    currentSample = 0;
  }

  pinMode(PING_PIN, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(PING_PIN, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(PING_PIN, HIGH);       // Start ranging
  delayMicroseconds(5);              //   with 5 microsecond burst
  digitalWrite(PING_PIN, LOW);        // End ranging
  pinMode(PING_PIN, INPUT);           // Set pin to INPUT
  unsigned int duration = pulseIn(PING_PIN, HIGH); // Read echo pulse
  samples[currentSample] = duration / 74 / 2;        // Convert to inches
}

int minDistance() {
  unsigned int distance = 100000;
  for(int x=0; x < MAX_SAMPLES; x++) {
    distance = min(distance, samples[x]);
  }
  return distance;
}

void loop() {
  sampleDistance();
  int inches = minDistance();
  mode = RC_GO;
  if (inches < ATTACK_DISTANCE) {
    mode = RC_PATROL;
  }
  if (inches < SAFE_DISTANCE) {
    mode = RC_RETREAT;
  }
  switch(mode) {
    case RC_PATROL:
      color(255, 255, 0); // Yellow
      digitalWrite(BACKWARD_PIN, LOW);
      digitalWrite(FORWARDS_PIN, HIGH);
      digitalWrite(TURN_RIGHT_PIN, LOW);
      digitalWrite(TURN_LEFT_PIN, HIGH);
      break;
    case RC_GO:
      color(0, 255, 0); 
      digitalWrite(BACKWARD_PIN, LOW);
      digitalWrite(FORWARDS_PIN, HIGH);
      digitalWrite(TURN_RIGHT_PIN, LOW);
      digitalWrite(TURN_LEFT_PIN, LOW);
      break;
    case RC_RETREAT:
      color(255, 0, 0); 
      digitalWrite(BACKWARD_PIN, HIGH);
      digitalWrite(FORWARDS_PIN, LOW);
      digitalWrite(TURN_RIGHT_PIN, HIGH);
      digitalWrite(TURN_LEFT_PIN, LOW);
      break;
    default:
      Serial.println("ERROR!");
      break;
  }
  Serial.println(inches);
  updateLED();
  delay(50);
}

