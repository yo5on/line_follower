#include <PID_v1.h>

// ================= PIN MAPPING =================
const int PWMA = 4, AIN1 = 17, AIN2 = 16;   
const int PWMB = 21, BIN1 = 18, BIN2 = 19;  
const int STBY = 5;

const int S0 = 27, S1 = 26, S2 = 25;        
const int muxSignal = 33;                   

const int btnStart = 13;                    
const int btnCalib = 14;                    
const int ledStatus = 2;                    

// ================= SPEED & PID =================
int baseSpeed = 190;
int maxCorrection = 100;
int turnSpeed = 150; 

// PID Constants
double Kp = 105.0; 
double Ki = 0.02;  
double Kd = 2.5;   

double setpoint = 3500;   // Middle of 0 - 7000 range
double input = 3500; 
double output = 0;

// Set to REVERSE so that:
// If Line is at Sensor 8 (Weight 0), Error (3500-0) is positive.
// REVERSE PID makes output NEGATIVE. 
// Left Speed (base + neg) slows down, Right Speed (base - neg) speeds up -> Turns LEFT.
// WAIT - to turn RIGHT when line is at 8, we use DIRECT mode.
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

int sensorMin[8], sensorMax[8];
bool isRunning = false;
bool whiteLine = false;

#define CH_LEFT  0
#define CH_RIGHT 1

void setup() {
  Serial.begin(115200);
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT); pinMode(S2, OUTPUT);
  pinMode(ledStatus, OUTPUT);
  pinMode(btnStart, INPUT_PULLUP);
  pinMode(btnCalib, INPUT_PULLUP);

  digitalWrite(STBY, HIGH);

  ledcSetup(CH_LEFT, 5000, 8); 
  ledcSetup(CH_RIGHT, 5000, 8);
  ledcAttachPin(PWMA, CH_LEFT);
  ledcAttachPin(PWMB, CH_RIGHT);

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-maxCorrection, maxCorrection); 
  myPID.SetSampleTime(5);
}

void loop() {
  checkButtons();

  if (!isRunning) {
    drive(0, 0);
    return;
  }

  detectLineColor();
  int detected = 0;
  input = getLinePosition(detected);

  // ===== LINE LOST RECOVERY =====
  if (detected == 0) {
    // If last seen on the Left side (near 8), turn RIGHT to find center
    if (input < 3500) drive(turnSpeed, -turnSpeed); 
    // If last seen on the Right side (near 1), turn LEFT to find center
    else drive(-turnSpeed, turnSpeed);
    return;
  }

  myPID.Compute();

  // Applying logic: 
  // Line at 8 (input 0) -> PID Output Positive -> Left speed UP, Right speed DOWN -> Turns RIGHT
  int leftOut  = baseSpeed + output;
  int rightOut = baseSpeed - output;

  drive(leftOut, rightOut);
}

// ================= SENSOR POSITIONING (8 to 1) =================
double getLinePosition(int &detected) {
  long avg = 0;
  long sum = 0;
  detected = 0;

  for (int i = 0; i < 8; i++) {
    int raw = readMux(i); 
    int n = map(raw, sensorMin[i], sensorMax[i], 0, 1000);
    n = constrain(n, 0, 1000);

    if (whiteLine) n = 1000 - n;
    if (n > 250) detected++;

    // Mux 0 = Sensor 8 (Left)  -> Weight 0
    // Mux 7 = Sensor 1 (Right) -> Weight 7000
    int weight = i * 1000;
    avg += (long)n * weight;
    sum += n;
  }

  if (sum < 50) return input; 
  return (double)avg / sum;
}

// ================= HELPERS =================

int readMux(int ch) {
  digitalWrite(S0, ch & 1);
  digitalWrite(S1, (ch >> 1) & 1);
  digitalWrite(S2, (ch >> 2) & 1);
  delayMicroseconds(15);
  return analogRead(muxSignal);
}

void detectLineColor() {
  long total = 0;
  for (int i = 0; i < 8; i++) total += readMux(i);
  int currentAvg = total / 8;
  int calibMid = 0;
  for (int i = 0; i < 8; i++) calibMid += (sensorMin[i] + sensorMax[i]) / 2;
  calibMid /= 8;

  if (currentAvg < calibMid - 200) whiteLine = false; 
  else if (currentAvg > calibMid + 200) whiteLine = true;
}

void drive(int left, int right) {
  digitalWrite(AIN1, left >= 0);
  digitalWrite(AIN2, left < 0);
  ledcWrite(CH_LEFT, constrain(abs(left), 0, 255));

  digitalWrite(BIN1, right >= 0);
  digitalWrite(BIN2, right < 0);
  ledcWrite(CH_RIGHT, constrain(abs(right), 0, 255));
}

void calibrate() {
  for (int i = 0; i < 8; i++) { sensorMin[i] = 4095; sensorMax[i] = 0; }
  unsigned long t = millis();
  while (millis() - t < 5000) {
    for (int i = 0; i < 8; i++) {
      int v = readMux(i);
      if (v < sensorMin[i]) sensorMin[i] = v;
      if (v > sensorMax[i]) sensorMax[i] = v;
    }
    digitalWrite(ledStatus, (millis() / 100) % 2);
  }
}

void checkButtons() {
  if (digitalRead(btnCalib) == LOW && !isRunning) { calibrate(); delay(500); }
  static bool lastState = HIGH;
  bool now = digitalRead(btnStart);
  if (lastState == HIGH && now == LOW) { isRunning = !isRunning; delay(300); }
  lastState = now;
}
