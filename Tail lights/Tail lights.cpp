/*
Lab Week 8 Vehicle Tail Lights
Exercise
Design and implement a vehicle taillight control based on real-time interrupts. Four buttons (PA2 for
left turn, PA0 for right turn, PA1 for brake, PA6 (centre of the rotary encoder on the lab board) for
emergency lights) trigger different modes:
1. Left/Right Turn: 8 LEDs on PORTC will sequentially turn off in a flowing pattern (leftward
from PC0 to PC7, rightward from PC7 to PC0) at 200ms intervals for 5 seconds, as in:
11111110 -> 11111101 -> 11111011 -> 11110111 -> …
2. Brake: All LEDs immediately turn on. If button released all LEDs should turn off.
3. Emergency Lights: PC0 and PC7 flash every second until button is pressed again.
10000001 -> 00000000 -> 10000001 -> …
Initialization Settings:
1. Set Timer0 in CTC mode, 1ms interrupt period.
2. No delay should be use, unless for buttons debounce handling.
3. All time counting should be handled by RTI.
*/


/*
* GccApplication1.c
*
* Created: 8/05/2025 11:53:17 am
* Author : 
*/
#define
F_CPU 8000000UL // 8MHz clock speed
#include <avr/io.h>
#include <avr/interrupt.h>
// Button definitions
/*
*Defines button pins on PORTA.
*Defines mode constants that determine the LED behavior. Using defined constants makes code more
readable and reduces the chances of errors.
*/
#define RIGHT_TURN_BTN 0 // PA0
#define BRAKE_BTN 1 // PA1
#define LEFT_TURN_BTN 2 // PA2
#define EMERGENCY_BTN 6 // PA6
// Mode definitions
#define MODE_OFF 0
#define MODE_LEFT_TURN 1
#define MODE_RIGHT_TURN 2
#define MODE_BRAKE 3
#define MODE_EMERGENCY 4
// Global variables
/*
*Stores the current mode of the tail light system.
*Tracks button states with debouncing counters to ensure stable button presses.
*Handles LED patterns for sequential animations.
*/

volatile
uint8_t current_mode = MODE_OFF;
volatile
uint8_t previous_mode = MODE_OFF;
volatile
uint16_t timer_counter = 0;
volatile
uint8_t led_pattern = 0x00; // All LEDs off initially
volatile
uint8_t pattern_step = 0;
volatile
uint8_t emergency_state = 0;
volatile
uint8_t emergency_active = 0;
volatile
uint8_t turn_active = 0;
volatile
uint16_t debounce_counter[4] = { 0 }; // Counters for debouncing 4 buttons
volatile
uint8_t button_state[4] = { 0 }; // Current state of buttons
// Function prototypes
//Declares functions before they are defined. This is essential for modular programming.//
void init_ports(void);
void init_timer0(void);
void process_buttons(void);
void update_leds(void);
/*
*Initializes ports and timer before entering the infinite loop.
*Enables global interrupts using sei(), ensuring interrupt-driven execution.
*The loop remains empty because the system operates through interrupts.
*/
int main(void) {
	// Initialize hardware
	init_ports();
	init_timer0();
	// Enable global interrupts
	sei();
	// Main loop
	while (1) {
		// Everything is handled in interrupts
		// Main loop can be used for non-time-critical tasks
	}
	Week 8 Lab tail lights Suemon Kwok 14883335
		return 0;
}
/*
*Sets PORTC as an output for controlling LEDs.
*Configures PORTA as an input for buttons with pull-up resistors.
*/
void init_ports(void) {
	// Configure PORTC as output for LEDs
	DDRC = 0xFF; // All pins as output
	PORTC = 0x00; // All LEDs off initially
	// Configure button pins on PORTA as inputs with pull-ups
	DDRA &= ~((1 << RIGHT_TURN_BTN) | (1 << BRAKE_BTN) | (1 << LEFT_TURN_BTN) | (1 <<
		EMERGENCY_BTN));
	PORTA |= ((1 << RIGHT_TURN_BTN) | (1 << BRAKE_BTN) | (1 << LEFT_TURN_BTN) | (1 <<
		EMERGENCY_BTN));
}
void init_timer0(void) {
	// Set Timer0 to CTC mode
	TCCR0A = (1 << WGM01);
	// Set prescaler to 64
	TCCR0B = (1 << CS01) | (1 << CS00);
	// Calculate OCR0A value for 1ms interrupt
	// For 8MHz clock with 64 prescaler:
	// 8,000,000 / 64 = 125,000 ticks per second
	// 125,000 / 1000 = 125 ticks per millisecond
	OCR0A = 124; // 125 - 1 (counting from 0)
	// Enable Timer0 Compare Match A interrupt
	TIMSK0 = (1 << OCIE0A);
}
// Timer0 Compare Match A interrupt service routine
/*
*Configures Timer0 in CTC mode for precise timing.
Week 8 Lab tail lights Suemon Kwok 14883335
*Uses a prescaler of 64 to set up a 1ms interrupt period.
*Enables interrupts to trigger on timer compare match
*/
/*
*Runs every 1ms.
*Handles button inputs and updates LED patterns.
*/
ISR(TIMER0_COMPA_vect) {
	// Increment timer counter
	timer_counter++;
	// Process button debouncing
	process_buttons();
	// Update LEDs based on current mode
	update_leds();
}
/*
*Scans button states every millisecond.
*Uses debouncing (waiting 50ms before confirming a press).
*Assigns mode transitions when a button is pressed.
*/
void process_buttons(void) {
	uint8_t pin_state = PINA;
	// Process each button with debouncing
	for (
		uint8_t i = 0; i < 4; i++) {
		uint8_t pin;
		// Map index to actual pin number
		switch (i) {
		case 0: pin = RIGHT_TURN_BTN; break;
			Week 8 Lab tail lights Suemon Kwok 14883335
		case 1: pin = BRAKE_BTN; break;
		case 2: pin = LEFT_TURN_BTN; break;
		case 3: pin = EMERGENCY_BTN; break;
		}
		// Check if button is pressed (active low since pull-ups are enabled)
		if (!(pin_state & (1 << pin))) {
			// Button is pressed
			if (debounce_counter[i] < 50) { // 50ms debounce time
				debounce_counter[i]++;
				if (debounce_counter[i] == 50) {
					// Button press confirmed
					if (button_state[i] == 0) {
						button_state[i] = 1;
						// Handle button press action
						switch (i) {
						case 0: // Right turn
							// Only start right turn if we're not already in right
							turn mode
								if (current_mode != MODE_RIGHT_TURN) {
									current_mode = MODE_RIGHT_TURN;
									pattern_step = 0;
									led_pattern = 0xFF; // Start with all LEDs
									on
										turn_active = 1;
									timer_counter = 0;
								}
							break;
						case 1: // Brake
							current_mode = MODE_BRAKE;
							led_pattern = 0xFF; // All LEDs on
							break;
						case 2: // Left turn
							// Only start left turn if we're not already in left turn
							mode
								if (current_mode != MODE_LEFT_TURN) {
									Week 8 Lab tail lights Suemon Kwok 14883335
										current_mode = MODE_LEFT_TURN;
									pattern_step = 0;
									led_pattern = 0xFF; // Start with all LEDs
									on
										turn_active = 1;
									timer_counter = 0;
								}
							break;
						case 3: // Emergency
							if (current_mode != MODE_EMERGENCY) {
								// Start emergency mode
								current_mode = MODE_EMERGENCY;
								emergency_state = 1;
								emergency_active = 1;
								led_pattern = 0x81; // 10000001 - PC0 and
								PC7 on
									timer_counter = 0;
							}
							else if (emergency_active) {
								// Turn off emergency mode
								emergency_active = 0;
								current_mode = MODE_OFF;
								led_pattern = 0x00; // All LEDs off
							}
							else {
								// Turn on emergency mode again
								emergency_active = 1;
								led_pattern = 0x81; // 10000001 - PC0 and
								PC7 on
							}
							break;
						}
					}
				}
			}
		}
		else {
			// Button is released
			if (debounce_counter[i] > 0) {
				debounce_counter[i]--;
				if (debounce_counter[i] == 0) {
					Week 8 Lab tail lights Suemon Kwok 14883335
						// Button release confirmed
						button_state[i] = 0;
					// Handle button release action
					if (i == 1 && current_mode == MODE_BRAKE) { // Brake button
						released
							current_mode = MODE_OFF;
						led_pattern = 0x00; // Turn off all LEDs
					}
				}
			}
		}
	}
}
/*
*Left/Right Turn Mode animates LEDs sequentially at 200ms intervals.
*Brake Mode activates all LEDs simultaneously.
*Emergency Mode flashes PC0 & PC7 every 1 second.
*Turn signals deactivate after 5 seconds
*/
void update_leds(void) {
	switch (current_mode) {
	case MODE_LEFT_TURN:
		// Check if it's time to update the pattern (every 200ms)
		if (timer_counter % 200 == 0 && timer_counter > 0) {
			// Shift pattern for left turn (PC0 to PC7)
			if (pattern_step < 8) {
				led_pattern &= ~(1 << pattern_step);
				pattern_step++;
			}
			else {
				// Reset pattern after one complete cycle
				pattern_step = 0;
				led_pattern = 0xFF;
			}
		}
		Week 8 Lab tail lights Suemon Kwok 14883335
			// Check if 5 seconds elapsed
			if (timer_counter >= 5000 && turn_active) {
				turn_active = 0;
				current_mode = MODE_OFF;
				led_pattern = 0x00;
			}
		break;
	case MODE_RIGHT_TURN:
		// Check if it's time to update the pattern (every 200ms)
		if (timer_counter % 200 == 0 && timer_counter > 0) {
			// Shift pattern for right turn (PC7 to PC0)
			if (pattern_step < 8) {
				led_pattern &= ~(1 << (7 - pattern_step));
				pattern_step++;
			}
			else {
				// Reset pattern after one complete cycle
				pattern_step = 0;
				led_pattern = 0xFF;
			}
		}
		// Check if 5 seconds elapsed
		if (timer_counter >= 5000 && turn_active) {
			turn_active = 0;
			current_mode = MODE_OFF;
			led_pattern = 0x00;
		}
		break;
	case MODE_BRAKE:
		// All LEDs remain on - already set in button handler
		break;
	case MODE_EMERGENCY:
		if (emergency_active) {
			// Check if it's time to update the pattern (every 1000ms)
			if (timer_counter % 1000 == 0 && timer_counter > 0) {
				Week 8 Lab tail lights Suemon Kwok 14883335
					if (emergency_state) {
						// Set outer LEDs on (PC0 and PC7)
						led_pattern = 0x81; // 10000001
					}
					else {
						// All LEDs off
						led_pattern = 0x00;
					}
				emergency_state = !emergency_state;
			}
		}
		else {
			// Emergency mode is off
			led_pattern = 0x00;
		}
		break;
	case MODE_OFF:
		// All LEDs off
		led_pattern = 0x00;
		break;
	}
	// Update PORTC with current LED pattern
	PORTC = led_pattern;
}