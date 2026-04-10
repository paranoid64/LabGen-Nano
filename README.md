# LabGen-Nano

LabGen Nano ist ein kompakter, präziser Funktionsgenerator auf Arduino-Basis, der professionelle Bedienlogik mit bewährter Hardware vereint. Als Herzstück dient das klassische YB2004D V3.0 Display (bekannt vom Prusa i3), welches Buzzer, Encoder, SD-Slot und LCD in einer Einheit vereint. Der Generator liefert Frequenzen von 0 Hz bis 12,5 MHz in den Wellenformen Sinus, Rechteck und Dreieck. Dank der dokumentierten Pinbelegung lässt sich das System flexibel anpassen: Alle Komponenten können auch einzeln verdrahtet oder gegen alternative Displays ausgetauscht werden. Mit getrennten AC- und DC-BNC-Ausgängen ist der LabGen Nano ein vielseitiges Werkzeug.

EXP1 und 2 Pins:
```
____________    ____________  
| (01) (03) (05) (07) (09) |  
| (02) (04) (06) (08) (19) |  
____________________________  
```

| EXP1     | Pin            | Nano/Uno |
|----------|----------------|----------|
| 01       | VSS (GND)      | GND      |
| 02       | VDD (VCC)      |	5V       |
| 03       | DB6            |	D4       |
| 04       | DB7            |	D5       |
| 05       | DB4            |	D2       |
| 06       | DB5            |	D3       |
| 07       | E (Enable)     |	D7       |
| 08       | RS	            | D8       |
| 09       | Buzzer         |	D9       |
| 10       | Encoder Button	| D10      |

| EXP2     | Pin	          | Nano/Uno |
|----------|----------------|----------|
| 05       | Encoder A      | A0       |
| 07       | Encoder B      | A1       |

Die Verkabelung (Nano zu AD9833)
| AD9833 Pin  | Nano Pin | Funktion             |
|-------------|----------|----------------------|
| VCC         | 5V       | Stromversorgung      |
| GND	        | GND      | Masse                |
| SDAT/DAT    | D11	     | SPI Daten (MOSI)     |
| SCLK/CLK    | D13	     | SPI Takt (SCK)       |
| FNC (FS/CS)	| D6	     | Chip Select (F-Sync) |
