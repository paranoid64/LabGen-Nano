#include <LiquidCrystal.h>

// LCD Pins bleiben wie bisher
LiquidCrystal lcd(8, 7, 2, 3, 4, 5);

// Pin-Anpassung für SD-Kompatibilität und DDS
const int pinBuzzer = A3;
const int pinButton = A2;
const int pinKill   = A4;
const int pinEncA   = A0; 
const int pinEncB   = A1; 
const int pinLEDStatus = A5; // Status LED für Output

unsigned long frequenz = 1000;
unsigned long multiplikator[] = {1, 1000, 1000000}; 
int einheitModus = 0; 
int waveMode = 0; 
bool outputAktiv = false;

int letzterStatusA;
unsigned long letzteAenderung = 0;
bool killGedrueckt = false; // Für Entprellung Switch

bool blinkStatus = true;
unsigned long letzteBlinkZeit = 0;

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW); 
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinKill, INPUT_PULLUP);    // Switch Eingang
  pinMode(pinEncA, INPUT_PULLUP); 
  pinMode(pinEncB, INPUT_PULLUP);
  pinMode(pinLEDStatus, OUTPUT);
  
  lcd.begin(20, 4);
  
  // Splash Screen
  lcd.setCursor(3, 1);
  lcd.print("LabGen-Nano");
  lcd.setCursor(6, 2);
  lcd.print("v1.0 Ready");
  delay(2000); 
  lcd.clear();
  
  letzterStatusA = digitalRead(pinEncA);
  updateDisplay();
}

void loop() {
  // 1. Kill-Switch (Start/Stop) mit Entprellung
  bool killStatus = (digitalRead(pinKill) == LOW); 

  if (killStatus && !killGedrueckt) {
    // Taste wurde gerade gedrückt
    outputAktiv = !outputAktiv; 
    
    digitalWrite(pinBuzzer, HIGH); 
    delay(80); 
    digitalWrite(pinBuzzer, LOW);
    
    updateDisplay();
    
    killGedrueckt = true; 
    letzteAenderung = millis(); // Zeitstempel merken
  } 

  // Nur zurücksetzen, wenn Taste losgelassen UND 50ms vergangen sind
  if (!killStatus && killGedrueckt) {
    if (millis() - letzteAenderung > 80) { 
      killGedrueckt = false;
    }
  }

  // 2. Encoder DREHEN
  int aktuellerStatusA = digitalRead(pinEncA);
  if (letzterStatusA == HIGH && aktuellerStatusA == LOW) {
    if (millis() - letzteAenderung > 50) {
      bool hoch = (digitalRead(pinEncB) == LOW); 
      
      if (einheitModus < 3) { 
        if (hoch) frequenz += multiplikator[einheitModus];
        else if (frequenz >= multiplikator[einheitModus]) frequenz -= multiplikator[einheitModus];
      } 
      else if (einheitModus == 3) { 
        waveMode = (waveMode + (hoch ? 1 : 2)) % 3;
      }

      if (frequenz > 12500000) frequenz = 12500000;
      updateDisplay();
      letzteAenderung = millis();
    }
  }
  letzterStatusA = aktuellerStatusA;

  // 3. Encoder KLICKEN (Menü-Wechsel)
  if (digitalRead(pinButton) == LOW) {
    digitalWrite(pinBuzzer, HIGH); delay(30); digitalWrite(pinBuzzer, LOW);
    einheitModus = (einheitModus + 1) % 5; 
    updateDisplay();
    while(digitalRead(pinButton) == LOW); 
  }

  // 4. Blink-Timer
  if (millis() - letzteBlinkZeit > 500) {
    blinkStatus = !blinkStatus;
    letzteBlinkZeit = millis();
    updateDisplay();
  }

  digitalWrite(pinLEDStatus, outputAktiv ? HIGH : LOW);

}

void updateDisplay() {
  char tempBuffer[10]; 

  // Zeile 1: Frequenz
  lcd.setCursor(0, 0);
  lcd.print("FREQ:"); 
  
  unsigned long mhz = frequenz / 1000000;
  unsigned long khz = (frequenz % 1000000) / 1000;
  unsigned long hz  = frequenz % 1000;

  // MHz
  lcd.setCursor(5, 0);
  if (einheitModus == 2 && !blinkStatus) lcd.print("  "); 
  else { sprintf(tempBuffer, "%02lu", mhz); lcd.print(tempBuffer); }
  lcd.print(".");

  // kHz
  lcd.setCursor(8, 0);
  if (einheitModus == 1 && !blinkStatus) lcd.print("   "); 
  else { sprintf(tempBuffer, "%03lu", khz); lcd.print(tempBuffer); }
  lcd.print(".");

  // Hz
  lcd.setCursor(12, 0);
  if (einheitModus == 0 && !blinkStatus) lcd.print("   "); 
  else { sprintf(tempBuffer, "%03lu", hz); lcd.print(tempBuffer); }
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
  if (einheitModus == 3 && !blinkStatus) lcd.print("        "); 
  else {
    if (waveMode == 0) lcd.print("SINUS   ");
    else if (waveMode == 1) lcd.print("RECHTECK");
    else if (waveMode == 2) lcd.print("DREIECK ");
  }

  // Zeile 4: Status
  lcd.setCursor(0, 3);
  if (outputAktiv) {
    lcd.print(">>> SIGNAL ON  <<<");
  } else {
    lcd.print(">>> SIGNAL OFF <<<");
  }

}
