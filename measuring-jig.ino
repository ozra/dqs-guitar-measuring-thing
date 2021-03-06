#include <Wire.h>
#include <LiquidCrystal.h>
#include <RotaryEncoder.h>

const long STEPS_PER_REVOLUTION = 20;
const long MICROMETERS_PER_REVOLUTION = 1250;

// Obs! Enbart pin 2 o 3 kan trigga interrupt på Uno
const int ROTARY_PIN_1 = 2;
const int ROTARY_PIN_2 = 3;
const int BUTTON_PIN = 4;

///   //  /
/// Förberäknade konstanter
///   //  /
const float MICROS_TO_MILLIS_RECIPROCAL = (1.0 / 1000.0);
const float LAP_STEPS_RECIPROCAL = (1.0 / STEPS_PER_REVOLUTION);
const float DISTANCE_COMPOUND_RECIPROCAL = LAP_STEPS_RECIPROCAL * MICROS_TO_MILLIS_RECIPROCAL;

///   //  /
/// Globala tillstånd
///   //  /
RotaryEncoder measurer(ROTARY_PIN_1, ROTARY_PIN_2);
// LiquidCrystal lcd(necessary pins here and such, aight!);
long prev_pos = -1;
///   //  /
///   //  /

void setup() {
    // Initiera interna serie-porten och vänta tills den är redo så inte skit
    // flyger i luften
    Serial.begin(9600);
    while (!Serial) {}

    // lcd.begin(16, 2);
    // lcd.setBacklight(1);

    // Se till att pinnarna är inställda som ingångar
    pinMode(BUTTON_PIN, INPUT);
    pinMode(ROTARY_PIN_1, INPUT);
    pinMode(ROTARY_PIN_2, INPUT);

    // Slå på pin-change-interrupt-1 för port C
    PCICR |= (1 << PCIE1);
    // Slå på interrupt for pin 2 och 3 på Port C
    PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);

    measurer.setPosition(0);
}

// Se till att rotary'n läses när interrupt triggats, så inget steg missas
ISR(PCINT1_vect) {
  measurer.tick();
}

void loop() {
    // Läs av knapp, om intryckt: nollställ position
    if (digitalRead(BUTTON_PIN) == LOW) {
        prev_pos = -1;
        measurer.setPosition(0);
        Serial.println("BAAM!");
    }

    // Om nuvarande position skiljer från senaste hanterade, gör massa skit
    long pos = measurer.getPosition();
    if (pos != prev_pos) {
        prev_pos = pos;

        float distance = (pos * MICROMETERS_PER_REVOLUTION) * DISTANCE_COMPOUND_RECIPROCAL;


        Serial.print("Pos: ");
        Serial.print(distance);
        Serial.println("mm");
        // lcd.setCursor(0, 1);
        // lcd.print(pos);
        // lcd.print(" ");
    }
}
