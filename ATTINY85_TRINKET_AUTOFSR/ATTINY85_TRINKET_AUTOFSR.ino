/*

  Auto Tuning FSR Trigger for ATtiny85 and AdaFruit Trinket

  Author: Wing Tang Wong
  GitHub: https://github.com/WingTangWong/AutoTuningFSRTrigger/ATTINY85_TRINKET_FSR

  Description:

  Stripped down sketch to improve response time. No functions other than setup() and loop().

  Calibrates on start, determines trigger and recovery levels. 

  Main loop just reads from ADC on one pin and triggers/recovers as the value dictates.

  Should be able to flip output on triggering event in under 1ms.

*/

/*
  Output pins
  ==================================================

  Note: 
    For NC(Normally Connected) hookup, wire DIO 0 out to the endstop signal.
    For NO(Normally Open) hookup, wire DIO 1 out to the enstop signal. 
    
    The two are basically reverses of each other. A bi-color LED placed between DIO
    pins 0 and 1 will toggle colors. 2 reversed LED(s) will achieve the same effect.
*/

int OUT = 0; // OUTPUT pin
int LED = 1; // LED pin

/*
  FSR's analog pin number and the digital pin number
  ==================================================
*/

int FSR  = 1; // FSR ADC pin
int FSRD = 2; // FSR ADC pin's Digital pin number

/*
  Misc variables for indexs and dump values
  ==================================================
*/

int value, reading, idx, noise;


/*
  Calibration and other values
  ==================================================
*/

unsigned int samples=20;

long fsrReading, fsrValue;
float fsrAverage , fsrTriggerLevel, fsrRecoveryLevel, fsrNoise;

// Default adjust levels. Tune as required.
float fsrRecoveryAdjust = 8.5;
float fsrTriggerAdjust  = 25;

void setup() {

  // Setup Output Pin
  pinMode( OUT , OUTPUT );
  digitalWrite( OUT, HIGH );

  // Setup LED pin
  pinMode( LED, OUTPUT );
  digitalWrite( LED, LOW );

  // Setup FSR pin
  pinMode( FSRD, INPUT );
  digitalWrite( FSRD, LOW );

  // Perform the first FSR read. This one takes longer than the rest.
  value = analogRead( FSR );

  // Calibration

  // Prime with a value
  fsrAverage = analogRead( FSR );

  for( idx=0; idx < samples ; idx++ ) {
    reading = analogRead( FSR );

    if ( reading > fsrAverage ) {
      noise = reading - fsrAverage;
    } else {
      noise = fsrAverage - reading;
    };

    if ( noise > fsrNoise ) {
      fsrNoise = ( fsrNoise + noise ) / 2.00 ;
    } else {
      fsrNoise = ( ( fsrNoise * 3 ) + noise ) / 2.00;
    };
   
    fsrAverage = ( fsrAverage + reading )  / 2.00;
  };

  // Triggering should be above the average and noise level, along with the trigger adjust. 
  fsrTriggerLevel = fsrAverage + fsrNoise + fsrTriggerAdjust;

  // Recovery should be at a point between the trigger level and the average reading level.
  fsrRecoveryLevel = ( fsrTriggerLevel - fsrAverage ) / 2.00;

};


// the loop routine runs over and over again forever:
void loop() {
  // Take a reading!
  fsrValue = analogRead( FSR );

  // Did we trigger?
  if ( fsrValue > fsrTriggerLevel ) {
    digitalWrite( OUT, LOW );
    digitalWrite( LED, HIGH );
  };

  // Did we recover?
  if ( fsrValue < fsrRecoveryLevel ) {
    digitalWrite( LED, LOW );
    digitalWrite( OUT, HIGH );
  };
};

