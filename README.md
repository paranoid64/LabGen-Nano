# LabGen-Nano

LabGen Nano ist ein kompakter, präziser Funktionsgenerator auf Arduino-Basis, der professionelle Bedienlogik mit bewährter Hardware vereint. Als Herzstück dient das klassische YB2004D V3.0 Display (bekannt vom Prusa i3), welches Buzzer, Encoder, SD-Slot und LCD in einer Einheit vereint. Der Generator liefert Frequenzen von 0 Hz bis 12,5 MHz in den Wellenformen Sinus, Rechteck und Dreieck. Dank der dokumentierten Pinbelegung lässt sich das System flexibel anpassen: Alle Komponenten können auch einzeln verdrahtet oder gegen alternative Displays ausgetauscht werden. Mit getrennten AC- und DC-BNC-Ausgängen ist der LabGen Nano ein vielseitiges Werkzeug.

## KOMPONENTEN:
- 1x Arduino Nano (oder Uno)
- 1x AD9833 DDS Modul
- 1x YB2004D V3.0 LCD Panel (20x4 Zeichen)
- 1x Widerstand 200 Ohm (Schutzwiderstand)
- 1x Folienkondensator (für AC-Kopplung)
- 2x BNC-Einbaubuchsen



STECKER-LAYOUT (Draufsicht auf die Wannen):
```
____________    ____________  

| (01) (03) (05) (07) (09) |  <- OBERE REIHE (Ungerade)
| (02) (04) (06) (08) (10) |  <- UNTERE REIHE (Gerade)
____________________________  

```

### EXP1 (Display & Sound):

```
Pin 01 (Oben L) : Buzzer          -> Nano A3
Pin 02 (Unten L): Encoder Button  -> Nano A2
Pin 03          : LCD_E (Enable)  -> Nano D7
Pin 04          : LCD_RS          -> Nano D8
Pin 05          : LCD_D4          -> Nano D2
Pin 06          : LCD_D5          -> Nano D3
Pin 07          : LCD_D6          -> Nano D4
Pin 08          : LCD_D7          -> Nano D5
Pin 09          : GND             -> Nano GND
Pin 10 (Unten R): VCC (+5V)       -> Nano 5V

```

### EXP2 (SD-Karte & Encoder):

```
Pin 01 (Oben L) : MISO (SD)       -> Nano D12
Pin 02 (Unten L): SCK (SD)        -> Nano D13
Pin 03          : Encoder A       -> Nano A0
Pin 04          : SD_CS           -> Nano D10
Pin 05          : Encoder B       -> Nano A1
Pin 06          : MOSI (SD)       -> Nano D11
Pin 07          : SD_DET          -> ---
Pin 08          : RESET 
Pin 09          : GND             -> Nano GND
Pin 10 (Unten R): KILL (Button)   -> Nano A4  (Der Taster muss angelötet werden)

```

Oder Taster an A4 GND

### VERKABELUNG AD9833 (DDS):

```
VCC          -> Nano 5V
GND          -> Nano GND
SDAT (MOSI)  -> Nano D11 (Parallel zu EXP2-06)
SCLK (SCK)   -> Nano D13 (Parallel zu EXP2-02)
FNC (CS)     -> Nano D9  (Eigener Pin)

```

### SIGNAL-AUSGANG:
```
VOUT (AD9833) --- [200 Ohm] ---+---> BNC "DC"
                               |
                               +--- [Kondensator] ---> BNC "AC"
GND (AD9833) ----------------------------------------> BNC Masse
```
