//declares all digital pins used on the board
//declares status LED Array
const int STATUS_ARR[] = {11, 12, 13};
//declares valve array (index 0 is valve 1)
const int VALVE_ARR[] = {7, 6, 5, 4, 3, 8, 3, 1, 0};
//declares pump array (index 0 is pump 1)
const int PUMP_ARR[] = {10, 9};
//declares button
const int BUTTON = 2;
//declares all analogue pins used on the board
//declares transducer array (index 0 is tranducer 1)
const int TRANSDUCER_ARR[] = {A0, A1};
//array of possible pressure
const int PRESSURE_ARR[3] = {123, 128, 133};
//current pressure setting;
int pressureSetting = 1; //switch back to 0 for sentinel

// the setup function runs once when you press reset or power the board
void setup() {
  //disable interrupts for setup
  noInterrupts();
  // initialize serial communication at 9600 bits per second: NOTE THIS IS SOLELY TO DEBUG THE SYSTEM
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Serial connection established");
  //initialize all pins and directions
  initializeLed();
  initializePump();
  initializeValve();
  initializeButton();
  //allow interrupts from the button
  interrupts();
}

// the loop function runs over and over again forever
void loop() {
  //sentinel loop waiting for user to press button to begin sequence. skipping for debugging purposes
  //awaitInput();
  //set current pressure setting
  int currentSetting = pressureSetting;
  //set status LED
  setStatus(currentSetting);
  int count;
  int transducerValue;
  bool upCount = true;
  int CUR_PRESSURE = PRESSURE_ARR[pressureSetting];
  //enter the pressure sequence sentinel loop. Sequencing will begin so long as pressure setting does not change
  while (currentSetting == pressureSetting) {
    //open the appropriate valve to pump
    openValve(count);
    //start the pump until the tranducer reaches the correct value
    startPump(count % 2, 255);
    while (transducerValue <= CUR_PRESSURE) {
      //read tranducer value (percent error about 2%)
      transducerValue = readTransducer(count % 2);
    }
    Serial.println("Pressure reached");
    //pressure has reached the appropriate pressure
    stopPump(count % 2);
    //keeps the pressure inflated in that bag until desired time is hit
    delay(0);
    //allows valve to remit
    closeValve(count);
    //determines if cuff is sequencing up or down
    if (upCount) {
      count++;
    }
    else {
      count--;
    }
    //conditions for change in sequence direction
    if (count == 8)
    {
      count = 6;
      upCount = false;
    }
    if (count == -1) {
      count = 1;
      upCount = true;
    }
    //slow sequence down
    Serial.println("Waiting");
    transducerValue = 0;
    delay(5000);
  }
}

void initializeLed() {
  Serial.println("Initializing LEDs");
  pinMode(STATUS_ARR[0], OUTPUT);
  pinMode(STATUS_ARR[1], OUTPUT);
  pinMode(STATUS_ARR[2], OUTPUT);
}


void initializePump() {
  Serial.println("Initializing pumps");
  pinMode(PUMP_ARR[0], OUTPUT);
  pinMode(PUMP_ARR[1], OUTPUT);
}

void initializeValve() {
  Serial.println("Initializing valves");
  pinMode(VALVE_ARR[0], OUTPUT);
  pinMode(VALVE_ARR[1], OUTPUT);
  pinMode(VALVE_ARR[2], OUTPUT);
  pinMode(VALVE_ARR[3], OUTPUT);
  pinMode(VALVE_ARR[4], OUTPUT);
  pinMode(VALVE_ARR[5], OUTPUT);
  pinMode(VALVE_ARR[6], OUTPUT);
  pinMode(VALVE_ARR[7], OUTPUT);
}

void startPump(int val, int pwm) {
  Serial.println("Starting pump #" + val);
  //pwm value is the speed from 0 to 255
  analogWrite(PUMP_ARR[val], pwm);
}

void stopPump(int val) {
  Serial.println("Stoping pump #" + val);
  digitalWrite(PUMP_ARR[val], LOW);
}

int readTransducer(int val) {
  Serial.println("Reading transducer #" + val);
  return analogRead(TRANSDUCER_ARR[val]);
}

void initializeButton() {
  Serial.println("Initializing button");
  //allows button to stop other processes from occuring in the device
  //TODO: Debounce Button??
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), changePressure, RISING);
}

void setStatus(int val) {
  Serial.println("Setting LED to setting #" + val);
  //status values determine what lights to turn on/off
  if (val == 1) {
    Serial.println("LED 1 on");
    digitalWrite(STATUS_ARR[0], HIGH);
    digitalWrite(STATUS_ARR[1], LOW);
    digitalWrite(STATUS_ARR[2], LOW);
  }
  else if (val == 2) {
    Serial.println("LED 2 on");
    digitalWrite(STATUS_ARR[0], LOW);
    digitalWrite(STATUS_ARR[1], HIGH);
    digitalWrite(STATUS_ARR[2], LOW);
  }
  else {
    Serial.println("LED 3 on");
    digitalWrite(STATUS_ARR[0], LOW);
    digitalWrite(STATUS_ARR[1], LOW);
    digitalWrite(STATUS_ARR[2], HIGH);
  }
}

void awaitInput() {
  //blinks a sequence of lights to indicate waiting on user input
  Serial.println("Awaiting input");
  while (pressureSetting == 0)
  {
    digitalWrite(STATUS_ARR[0], HIGH);
    digitalWrite(STATUS_ARR[1], LOW);
    digitalWrite(STATUS_ARR[2], LOW);
    delay(500);
    digitalWrite(STATUS_ARR[0], LOW);
    digitalWrite(STATUS_ARR[1], HIGH);
    digitalWrite(STATUS_ARR[2], LOW);
    delay(500);
    digitalWrite(STATUS_ARR[0], LOW);
    digitalWrite(STATUS_ARR[1], LOW);
    digitalWrite(STATUS_ARR[2], HIGH);
    delay(500);
  }
}

void changePressure() {
  //increments the pressure if button pressed
  Serial.println("Button interupt triggered");
  pressureSetting++;
  if (pressureSetting >= 4) {
    pressureSetting = 1;
  }
}

void openValve(int valve) {
  Serial.println("Opening valve #" + valve);
  digitalWrite(VALVE_ARR[valve], HIGH);
}

void closeValve(int valve) {
  Serial.println("Closing valve #" + valve);
  digitalWrite(VALVE_ARR[valve], LOW);
}
