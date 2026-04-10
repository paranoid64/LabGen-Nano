
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 7, 2, 3, 4, 5);

const int pinBuzzer = 9;   
const int pinButton = 10;  
const int pinEncA = A0; 
const int pinEncB = A1; 

unsigned long frequenz = 1000;
unsigned long multiplikator[] = {1, 1000, 1000000}; 
int einheitModus = 0; // 0=Hz, 1=kHz, 2=MHz, 3=Wave, 4=Output
int waveMode = 0; 
bool outputAktiv = false;

int letzterStatusA;
unsigned long letzteAenderung = 0;

bool blinkStatus = true;
unsigned long letzteBlinkZeit = 0;

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW); 
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinEncA, INPUT_PULLUP); 
  pinMode(pinEncB, INPUT_PULLUP);
  
  lcd.begin(20, 4);
  
  // Splash Screen
  lcd.setCursor(3, 1);
  lcd.print("LabGen-Nano");
  lcd.setCursor(6, 2);
  lcd.print("v1.0 Ready");
  delay(2000); // 2 Sekunden Pause
  lcd.clear();
  
  letzterStatusA = digitalRead(pinEncA);
  updateDisplay();
}

void loop() {
  // 1. Encoder DREHEN
  int aktuellerStatusA = digitalRead(pinEncA);
  if (letzterStatusA == HIGH && aktuellerStatusA == LOW) {
    if (millis() - letzteAenderung > 50) {
      bool hoch = (digitalRead(pinEncB) == LOW); 
      
      if (einheitModus < 3) { // Hz, kHz, MHz Bereich
        if (hoch) frequenz += multiplikator[einheitModus];
        else if (frequenz >= multiplikator[einheitModus]) frequenz -= multiplikator[einheitModus];
      } 
      else if (einheitModus == 3) { // Wellenform
        waveMode = (waveMode + (hoch ? 1 : 2)) % 3;
      }
      else if (einheitModus == 4) { // Output Start/Stop
        outputAktiv = !outputAktiv;
      }

      if (frequenz > 12500000) frequenz = 12500000;
      updateDisplay();
      letzteAenderung = millis();
    }
  }
  letzterStatusA = aktuellerStatusA;

  // 2. Encoder KLICKEN
  if (digitalRead(pinButton) == LOW) {
    digitalWrite(pinBuzzer, HIGH); delay(30); digitalWrite(pinBuzzer, LOW);
    
    einheitModus = (einheitModus + 1) % 5; // 5 Positionen
    updateDisplay();
    while(digitalRead(pinButton) == LOW); 
  }

  // Blink-Timer: Alle 500ms den Status umkehren
  if (millis() - letzteBlinkZeit > 500) {
    blinkStatus = !blinkStatus;
    letzteBlinkZeit = millis();
    updateDisplay(); // Display aktualisieren
  }

}

void updateDisplay() {
  char tempBuffer[10]; // Sicherer Puffer für kleine Zahlenblöcke

  // Zeile 1: Frequenz
  lcd.setCursor(0, 0);
  lcd.print("FREQ:"); 
  
  unsigned long mhz = frequenz / 1000000;
  unsigned long khz = (frequenz % 1000000) / 1000;
  unsigned long hz  = frequenz % 1000;

  // MHz Block
  lcd.setCursor(5, 0);
  if (einheitModus == 2 && !blinkStatus) {
    lcd.print("  "); 
  } else {
    sprintf(tempBuffer, "%02lu", mhz);
    lcd.print(tempBuffer);
  }
  lcd.print(".");

  // kHz Block
  lcd.setCursor(8, 0);
  if (einheitModus == 1 && !blinkStatus) {
    lcd.print("   "); 
  } else {
    sprintf(tempBuffer, "%03lu", khz);
    lcd.print(tempBuffer);
  }
  lcd.print(".");

  // Hz Block
  lcd.setCursor(12, 0);
  if (einheitModus == 0 && !blinkStatus) {
    lcd.print("   "); 
  } else {
    sprintf(tempBuffer, "%03lu", hz);
    lcd.print(tempBuffer);
  }
  lcd.print(" Hz");

  // Zeile 2: Pfeil
  lcd.setCursor(0, 1);
  lcd.print("                    "); 
  if (einheitModus < 3) {
    int pfeilPos = (einheitModus == 2) ? 5 : (einheitModus == 1 ? 9 : 13);
    lcd.setCursor(pfeilPos, 1);
    lcd.print("^");
  }

  // Zeile 3: Wellenform
  lcd.setCursor(0, 2);
  lcd.print("WAVE: "); 
  lcd.setCursor(6, 2);
  if (einheitModus == 3 && !blinkStatus) {
    lcd.print("        "); 
  } else {
    if (waveMode == 0) lcd.print("SINUS   ");
    if (waveMode == 1) lcd.print("RECHTECK");
    if (waveMode == 2) lcd.print("DREIECK ");
  }

  // Zeile 4: Output
  lcd.setCursor(0, 3);
  lcd.print("OUT : "); 
  lcd.setCursor(6, 3);
  if (einheitModus == 4 && !blinkStatus) {
    lcd.print("        "); 
  } else {
    lcd.print(outputAktiv ? "RUNNING " : "STOPPED ");
  }
}
