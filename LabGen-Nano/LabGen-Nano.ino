#include <LiquidCrystal.h>
#include "AD9833.h"

// --- KONFIGURATION (Festwerte) ---
#define ENCODER_SPEED 50
#define DEBOUNCE_TIME 150
#define MAX_FREQ 12500000
#define ENCODER_REVERSED 0  // 0 Off , 1 On

// --- PIN-DEFINITIONEN ---
#define pinBuzzer    A3
#define pinButton    A2
#define pinKill      A4
#define pinEncA      A0
#define pinEncB      A1
#define pinLEDStatus A5
#define ddsFSYNC     10

LiquidCrystal lcd(8, 7, 2, 3, 4, 5);
//LiquidCrystal lcd(8, 7, 5, 2, 3, 4);
AD9833 dds(10);

unsigned long frequenz = 1000;
// Multiplikator liegt jetzt nur im Flash (spart RAM)
const unsigned long multiplikator[] PROGMEM = {1, 1000, 1000000}; 

uint8_t einheitModus = 0; 
uint8_t waveMode     = 0; 
bool outputAktiv = false;

bool letzterStatusA;
unsigned long letzteAenderung = 0;
bool killGedrueckt = false; 

bool blinkStatus = true;
unsigned long letzteBlinkZeit = 0;

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW); 
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinKill, INPUT_PULLUP);    
  pinMode(pinEncA, INPUT_PULLUP); 
  pinMode(pinEncB, INPUT_PULLUP);
  pinMode(pinLEDStatus, OUTPUT);
  
  lcd.begin(20, 4);
  
  lcd.setCursor(3, 1);
  lcd.print(F("LabGen-Nano"));
  lcd.setCursor(6, 2);
  lcd.print(F("v1.0"));
  delay(2000); 
  lcd.clear();
  
  letzterStatusA = digitalRead(pinEncA);
  updateDisplay();

  SPI.begin(); 
  dds.begin();
  dds.setFrequency(1000, 0); 
  dds.setWave(AD9833_OFF);   
}

void loop() {
  // 1. Kill-Switch
  bool killStatus = (digitalRead(pinKill) == LOW); 
  if (killStatus && !killGedrueckt) {
    outputAktiv = !outputAktiv; 
    applyDDS();
    digitalWrite(pinBuzzer, HIGH); delay(100); digitalWrite(pinBuzzer, LOW);
    updateDisplay();
    killGedrueckt = true; 
    letzteAenderung = millis(); 
  } 

  if (!killStatus && killGedrueckt) {
    if (millis() - letzteAenderung > DEBOUNCE_TIME) { 
      killGedrueckt = false;
    }
  }

  // 2. Encoder DREHEN
  int aktuellerStatusA = digitalRead(pinEncA);
  if (letzterStatusA == HIGH && aktuellerStatusA == LOW) {
    if (millis() - letzteAenderung > ENCODER_SPEED) {
      
      // Bestimmt die Richtung basierend auf dem Schalter
      #if ENCODER_REVERSED
        bool hoch = (digitalRead(pinEncB) == HIGH); 
      #else
        bool hoch = (digitalRead(pinEncB) == LOW);
      #endif
      
      if (outputAktiv) { 
        outputAktiv = false;
        applyDDS(); 
      }

      if (einheitModus < 3) { 
        unsigned long m = pgm_read_dword(&(multiplikator[einheitModus]));
        if (hoch) {
          frequenz += m;
        } else if (frequenz >= m) {
          frequenz -= m;
        }
      } 
      else if (einheitModus == 3) { 
        waveMode = (waveMode + (hoch ? 1 : 2)) % 3;
      }

      if (frequenz > MAX_FREQ) frequenz = MAX_FREQ;  
      updateDisplay();
      letzteAenderung = millis();
    }
  }
  letzterStatusA = aktuellerStatusA;


  // 3. Encoder KLICKEN
  if (digitalRead(pinButton) == LOW) {
    digitalWrite(pinBuzzer, HIGH); delay(30); digitalWrite(pinBuzzer, LOW);
    // Hier auf % 4 geändert, da wir 4 Modi haben (0,1,2,3)
    einheitModus = (einheitModus + 1) % 4; 
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

  lcd.setCursor(0, 0);
  lcd.print(F("FREQ:")); 
  
  unsigned long mhz = frequenz / 1000000;
  unsigned long khz = (frequenz % 1000000) / 1000;
  unsigned long hz  = frequenz % 1000;

  lcd.setCursor(5, 0);
  if (einheitModus == 2 && !blinkStatus) lcd.print(F("  ")); 
  else { sprintf(tempBuffer, "%02lu", mhz); lcd.print(tempBuffer); }
  lcd.print(F("."));

  lcd.setCursor(8, 0);
  if (einheitModus == 1 && !blinkStatus) lcd.print(F("   ")); 
  else { sprintf(tempBuffer, "%03lu", khz); lcd.print(tempBuffer); }
  lcd.print(F("."));

  lcd.setCursor(12, 0);
  if (einheitModus == 0 && !blinkStatus) lcd.print(F("   ")); 
  else { sprintf(tempBuffer, "%03lu", hz); lcd.print(tempBuffer); }
  lcd.print(F(" Hz"));

  lcd.setCursor(0, 1);
  lcd.print(F("                "));
  if (einheitModus < 3) {
    int pfeilPos = (einheitModus == 2) ? 5 : (einheitModus == 1 ? 9 : 13);
    lcd.setCursor(pfeilPos, 1);
    lcd.print(F("^"));
  }

  lcd.setCursor(0, 2);
  lcd.print(F("WAVE: ")); 
  lcd.setCursor(6, 2);
  if (einheitModus == 3 && !blinkStatus) lcd.print(F("        ")); 
  else {
    if (waveMode == 0) lcd.print(F("SINUS   "));
    else if (waveMode == 1) lcd.print(F("RECHTECK"));
    else if (waveMode == 2) lcd.print(F("DREIECK "));
  }

  lcd.setCursor(0, 3); 
  if (outputAktiv) lcd.print(F(">>> SIGNAL ON  <<<"));
  else lcd.print(F(">>> SIGNAL OFF <<<"));
}

void applyDDS() {
  if (!outputAktiv) {
    dds.setWave(AD9833_OFF);
    dds.setPowerMode(AD9833_PWR_DISABLE_ALL);
  } else {
    dds.setPowerMode(AD9833_PWR_ON);
    if (waveMode == 0) dds.setWave(AD9833_SINE);
    else if (waveMode == 1) dds.setWave(AD9833_SQUARE1); 
    else dds.setWave(AD9833_TRIANGLE);
    dds.setFrequency(frequenz, 0);
  }
}
