
void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
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
