#define POTENTIOMETER_PIN A2
int valueOld = 0, value = 0;
byte sensitivity = 10;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    value = analogRead(POTENTIOMETER_PIN);

    if (abs(valueOld - value) >= sensitivity) {
        valueOld = value;
        Serial.println(value);
    }
}