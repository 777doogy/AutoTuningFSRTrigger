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

int OUT = 12; // OUTPUT pin
int LED = 13; // LED pin

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
unsigned long ticks=0;
long fsrReading, fsrValue;
float fsrAverage , fsrTriggerLevel, fsrRecoveryLevel, fsrNoise;

// Default adjust levels. Tune as required.
float fsrRecoveryAdjust = 10;    // adjustment
float fsrTriggerAdjust  = 1.1;  // Multiplication factor
  bool lastState=false;

void setup() {
  Serial.begin(9600);
  
  fsrNoise=0;
  fsrRecoveryLevel=0;
  fsrTriggerLevel=0;
  fsrAverage=0;
  
  // Setup Output Pin
  pinMode( OUT , OUTPUT );
  digitalWrite( OUT, LOW );

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
  fsrAverage = analogRead( FSR );
  
  
  
  while ( ( fsrTriggerLevel < fsrAverage ) || ( ( fsrTriggerLevel - fsrAverage ) > 150 ) || ( ( fsrRecoveryLevel - fsrAverage) > 150 ) ) {
    delay(250);
    fsrAverage=0;
    fsrRecoveryLevel=0;
    fsrTriggerLevel=0;
    fsrNoise=0;
    
  for( idx=0; idx < samples ; idx++ ) {
    reading = analogRead( FSR );
   
    fsrAverage = ( fsrAverage + reading )  / 2.00;
  };

  // Triggering should be above the average and noise level, along with the trigger adjust. 
  fsrTriggerLevel = ( fsrAverage + fsrRecoveryAdjust )  * fsrTriggerAdjust ;
    
  // Recovery should be at a point between the trigger level and the average reading level.
  fsrRecoveryLevel = fsrAverage + ( ( fsrTriggerLevel - fsrAverage ) / 3.0) + fsrRecoveryAdjust;

  Serial.print("FSRavg:");
  Serial.println(fsrAverage);
  Serial.print("  FSR Trigger Level:");
  Serial.println(fsrTriggerLevel);
  Serial.print("  FSR Recovery Level:");
  Serial.println(fsrRecoveryLevel);

  };

  // All ready? Okay, set the output pin high
  digitalWrite( OUT, HIGH );
  
};


// the loop routine runs over and over again forever:
void loop() {
  // Take a reading!
  fsrValue = analogRead( FSR );
  
  // Did we trigger?
  if ( fsrValue > fsrTriggerLevel ) {
    if ( lastState != true ) {
      lastState = true;
      digitalWrite( OUT, LOW );
      digitalWrite( LED, HIGH );
      Serial.print("TRIGGERED(");
      Serial.print(fsrTriggerLevel);
      Serial.print("): ");    
      Serial.println(fsrValue);
    };
  };

  // Did we recover?
  if ( fsrValue < fsrRecoveryLevel ) {
    if ( lastState != false ) {
      lastState = false;
    digitalWrite( LED, LOW );
    digitalWrite( OUT, HIGH );
    Serial.print("RECOVERED(");
    Serial.print(fsrRecoveryLevel);
    Serial.print("): ");
    Serial.println(fsrValue);
    Serial.println(" ");
    };
  };
  if ( ticks > 1000 ) {
    Serial.print("FSR:");
    Serial.println(fsrValue);
    ticks=0;
  };
  ticks++;
};


