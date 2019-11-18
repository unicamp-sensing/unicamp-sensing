void setup() {
	Serial.begin(115200);
}

void loop() {
	delay(1000);	
	Serial.println("Starting");
	delay(2000);
	Serial.println("Pass");
}
