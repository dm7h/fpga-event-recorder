
// Midi clock timing test for Event Recorder
// see: https://www.pjrc.com/teensy/td_midi.html

int ledPin = 13;
int ledStatus = 0;
int counter = 0;

// midi clock start handler (0xFA)
void onStart() {
  digitalWrite(0, HIGH);
  delay(1);
  digitalWrite(1, HIGH);
  digitalWrite(ledPin, HIGH);
}

// midi clock tick handler (0xF8) 
void onClock() {
    // disable led blinking?
    ledStatus = ~ledStatus;
    digitalWrite(ledPin, ledStatus);
    digitalWrite(2, HIGH);
    digitalWrite(2, LOW);
}

// midi clock stop handler (0xFC)
void onStop() {
  digitalWrite(1, LOW);
  delay(1);
  digitalWrite(0, LOW);
  digitalWrite(ledPin, LOW);
}

// midi note on handler (0xFB) (used to perform tests)

void onNoteOn(byte channel, byte note, byte velocity) {
  // pin check
  if (note == 0) {
    for (int i = 0; i < 12; i++) {
      digitalWrite(i, HIGH);
      delay(50);
      digitalWrite(i, LOW);     
      ledStatus = ~ledStatus;
      digitalWrite(ledPin, ledStatus);
      delay(50);
    }
  } else if (note == 1) {
    digitalWrite(0, HIGH);
    //digitalWrite(11, HIGH);
  } else if (note == 2) {
    digitalWrite(0, LOW);
    //digitalWrite(11, LOW);
  } else if (note == 3) {
    digitalWrite(1, HIGH);
    //digitalWrite(10, HIGH);
  } else if (note == 4) {
    digitalWrite(1, LOW);
    //digitalWrite(10, LOW);
  } else if (note == 5) {
    digitalWrite(2, HIGH);
    //digitalWrite(9, HIGH);
  } else if (note == 6) {
    digitalWrite(2, LOW);
    //digitalWrite(9, LOW);
  } else if (note == 7) {
    digitalWrite(3, HIGH);
    //digitalWrite(8, HIGH);
  } else if (note == 8) {
    digitalWrite(3, LOW);
    //digitalWrite(8, LOW);
  } else if (note == 12) {
    for (int i = 0; i < 12; i++) {
      digitalWrite(i, LOW);
    }
  }
}


void setup() {
  // pin setup
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < 12; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  // register midi handlers
  usbMIDI.setHandleStart(onStart);
  usbMIDI.setHandleStop(onStop);
  usbMIDI.setHandleClock(onClock);
  usbMIDI.setHandleNoteOn(onNoteOn);
  // give a life sign at startup
  digitalWrite(ledPin, HIGH);
  delay(400);     
  digitalWrite(ledPin, LOW);
}

void loop() {
  usbMIDI.read();
}
