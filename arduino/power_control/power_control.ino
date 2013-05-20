/**
 * Control the power state of a Canon SX260HS
 * 
 * Decode r/c receiver PPM servo signal and turn the camera on or off.
 * Since the camera is using a digital button for on/off, PIN_CAM_PWR_CHK 
 * is used to check if the circuit board of the camera is powered up or 
 * down for state control.
 *
 * This program is made to be used with ATtiny45/85
 *
 * $Id$
 */

// pin setup
#define PIN_PPM  0        // ATTIny pin 0
#define PIN_CAM_PWR_BTN 4 // ATTiny pin 3
#define PIN_CAM_PWR_CHK 3 // ATTiny pin 2

unsigned long duration, lastgood = 0;
int ppm_in, cam_state, cam_shutdown = 0;

void setup() {
	pinMode(PIN_PPM, INPUT);
	pinMode(PIN_CAM_PWR_CHK, INPUT);
	pinMode(PIN_CAM_PWR_BTN, OUTPUT);
	digitalWrite(PIN_CAM_PWR_BTN, LOW);
	delay(100); // make sure the pin is low
}

void loop() {
  
	/**
	 * check ppm signal
	 * the length of the pulse (in microseconds) or 0 if no pulse 
	 * started before the timeout (unsigned long)
	 *
	 * ppm_in == 0 means the camera should be off or turned off
	 * ppm_in == 1 means the camera should be on or turned on
	 */
	duration = pulseIn(PIN_PPM, HIGH, 20000); 
	if (duration == 0)
		duration = lastgood;
	else
		lastgood = duration;
	ppm_in = map(lastgood, 1000, 2000, 0, 1);
	
	/**
	 * measure the camera's circuit voltage. If it is not close 
	 * to zero, the camera is powered on. The camera will report 
	 * a positive voltage during power down for upt or more than 10 secs
	 */
	cam_state = (analogRead(PIN_CAM_PWR_CHK) > 100) ? 1 : 0;

	/** 
	 * PIN_CAM_PWR_CHK reports immediately when the camera is turned on
	 * when the camera is shutting down, PIN_CAM_PWR_CHK may report the new
	 * state with a delay of 5-10 seconds (need to test a bit more)
	 */
	
	// do not trigger any action while the camera is in shutdown state
	if (cam_shutdown && cam_state) {
		return;
	}
	
	if (cam_shutdown && !cam_state) {
		cam_shutdown = 0;
	}
	
	// if the camera state is high and ppm_in is low, turn it off
	if (!ppm_in && cam_state) {
		digitalWrite(PIN_CAM_PWR_BTN, HIGH);
		delay(500);
		digitalWrite(PIN_CAM_PWR_BTN, LOW);
		cam_shutdown = 1;
	}
	
	// if ppm_in is high and cam_state is low, turn the camera on
	if (ppm_in && !cam_state) {
		digitalWrite(PIN_CAM_PWR_BTN, HIGH);
		delay(500);
		digitalWrite(PIN_CAM_PWR_BTN, LOW);
		delay(500);
	}
	
}

