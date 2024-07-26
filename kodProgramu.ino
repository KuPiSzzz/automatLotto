const int motorPin = 3;        // Pin sterujący silnikiem
const int buttonPin = 6;       // Pin podłączony do przycisku bistabilnego
const int trigPin = 4;         // Pin TRIG czujnika ultradźwiękowego
const int echoPin = 5;         // Pin ECHO czujnika ultradźwiękowego
const int ledPins[] = {8, 9, 10, 11, 12, 13}; // Piny diod LED
const int numLeds = 6;         // Liczba diod LED

unsigned long previousMillis = 0;
const long interval = 100; // Częstotliwość standardowych pomiarów
const long delayAfterDetection = 1500; // Opóźnienie po wykryciu (1.5 sekundy)
int distanceCount = 0;
bool motorEnabled = false;
bool buttonState = LOW;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
    pinMode(motorPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
    
    for (int i = 0; i < numLeds; i++) {
        pinMode(ledPins[i], OUTPUT);
    }

    analogWrite(motorPin, 0);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        long duration = pulseIn(echoPin, HIGH);
        float distance = duration * 0.034 / 2;

        Serial.print("Zmierzona odleglosc: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance < 12.8) {
            distanceCount++;
            Serial.print("Odległość mniejsza niż 12 cm! Zliczono: ");
            Serial.println(distanceCount);

            // Zapal odpowiednie diody LED
            for (int i = 0; i < numLeds; i++) {
                if (i < distanceCount) {
                    digitalWrite(ledPins[i], HIGH);
                } else {
                    digitalWrite(ledPins[i], LOW);
                }
            }

            if (distanceCount >= 6) {
                motorEnabled = false;
                analogWrite(motorPin, 0);
                Serial.println("Silnik wyłączony - osiągnięto 6 razy odległość mniejszą niż 12 cm.");
            }

            // Bezpośrednie opóźnienie
            delay(delayAfterDetection);
            previousMillis = millis(); // Resetowanie timera
        } else {
            previousMillis = currentMillis;
        }

        if (motorEnabled) {
            analogWrite(motorPin, 110);
        }
    }

    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == LOW) {
                motorEnabled = !motorEnabled;
                if (motorEnabled) {
                    distanceCount = 0;
                }
            }
        }
    }

    lastButtonState = reading;

    if (motorEnabled) {
        analogWrite(motorPin, 110);
    } else {
        analogWrite(motorPin, 0);
    }

    for (int i = 0; i < numLeds; i++) {
        if (i < distanceCount) {
            digitalWrite(ledPins[i], HIGH);
        } else {
            digitalWrite(ledPins[i], LOW);
        }
    }
}
