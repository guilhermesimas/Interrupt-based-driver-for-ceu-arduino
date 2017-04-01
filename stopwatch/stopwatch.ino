/*
 Find the right spot.
 Generate a "Right spot automatically".
 User has to have its finger on the correct position and
 press the button. The program will avaliate if it was
 successfull and, if not, it will give a hint (far or close).
 Will keep posting on Serial how many seconds the user has and the attempts.
 If correct, will give a happy message

 Will now use a common potentiometer so that it can be easily
 reproduced
 */

const int BUTTON = 2;
const int SENSOR = A0;

const int SEED_GENERATOR = A1;

volatile int position_raw = 0;
volatile bool buttonWasPressed;

int correct_position = 0;
int number_of_attempts = 0;

const int max_time = 30;
int time_elapsed = 0;

bool gameOver = false;

unsigned long previous_time = 0;

const int MAX_NUMBER_ATTEMPTS = 5;
const int TIME_WARNING_INTERVAL = 5;

void setup() {
	pinMode( BUTTON, INPUT );
	pinMode( SENSOR, INPUT );

	pinMode ( SEED_GENERATOR, INPUT );

	attachInterrupt( digitalPinToInterrupt( BUTTON ), buttonPressed, RISING );

	Serial.begin( 9600 );
	Serial.println( "Try to find the correct position! You have 5 attempts and 30 seconds!" );
	randomSeed( analogRead( SEED_GENERATOR ) );
	correct_position = random( 8 );

	previous_time = millis();

}

void loop() {
	if ( !gameOver ) {
		if ( buttonWasPressed ) {
			buttonWasPressed = false;
			int position = treatValue( position_raw );
			int difference = correct_position - position;
			printHintMessage( difference );
			number_of_attempts++;
			if ( number_of_attempts == MAX_NUMBER_ATTEMPTS + 1 ) {
				gameOver = true;
			}
		}
		unsigned long current_time = millis();
		if ( current_time - previous_time >= TIME_WARNING_INTERVAL * 1000 ) {
			previous_time = current_time;
			time_elapsed += TIME_WARNING_INTERVAL;
			if ( time_elapsed == current_time ) {
				gameOver = true;
			}
			Serial.print( max_time - time_elapsed );
			Serial.println( " seconds remaining!" );
		}
	}
}

void printVictoryMessage() {
	Serial.println( "Spot on! You won!" );
	printEndMessage();
}

void printEndMessage( ) {
	Serial.println( "<press reset button to play again>"  );
}

void printTimesUpMessage() {
	Serial.println( "Time's up! You lost!" );
	printEndMessage();

}

void printTooManyAttemptsMessage() {
	Serial.println( "Too many attemps! You lost!" );
	printEndMessage();

}

void printHintMessage( int difference ) {
	if ( difference == 0 ) {
		printVictoryMessage();
		gameOver = true;
		return;
	}
	if ( difference < 0 ) {
		Serial.println( "Too far right!" );
		return;
	}
	Serial.println( "Too far left!" );
	return;
}

void buttonPressed() {
	// Interrupt routine goes here. Debounce button (check state after to make sure it wasnt a false press)
	delay( 30 );
	int buttonState = digitalRead( BUTTON );
	if ( buttonState == LOW ) {
		return;
	}
	position_raw = analogRead( SENSOR );
	buttonWasPressed = true;

}

int treatValue( int raw_value ) {
	return raw_value >> 7;
}


