const int SENSOR = A0;
const int LED = 5;

void setup() {
	pinMode( LED, OUTPUT );
	pinMode( SENSOR, INPUT );

	digitalWrite( LED, HIGH );
	delay( 500 );
	digitalWrite( LED, LOW );
	delay( 500 );
	digitalWrite( LED, HIGH );
	delay( 500 );
	digitalWrite( LED, LOW );
}

void loop() {
	int value = analogRead( SENSOR );
	if ( value > 500 ) {
		digitalWrite( LED, HIGH );
	} else {
		digitalWrite( LED, LOW );
	}
}
