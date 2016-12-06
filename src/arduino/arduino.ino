
void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
}
 
void loop() {
    if(Serial.available() > 0) {
        char c = Serial.read();
        if (c == '1') {
            digitalWrite(13, HIGH);
            delay(250);
            digitalWrite(13, LOW);
        } else if (c == '0') {
            digitalWrite(12, HIGH);
            delay(250);
            digitalWrite(12, LOW);
        }
    }
}
