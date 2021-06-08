/**
 * fuzzy_controller.ino
 * 
 * Measuring based on inputs and IR sensor . The output is a servo Motor. The fuzzy rules are:
 * if distance is small and ldr is low then brightness is low

The inputs are an ultrasonic sensor and a LDR. The output is a Motor. We define the variables of distance, ldr and brightness. 

if distance is small and ldr is low then brightness is high if distance is small and ldr is high then brightness is off if distance is mid then brightness is midb if distance is big then brightness is low if distance is verybig then brightness is off
 **/

// eFFL includes
#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>
#include <Servo.h>

// pins
#define IR 0
#define TRIGG 4
#define ECH 5
#define TRIGG1 6
#define ECH1 7
#define SERVO 3
#define SERVO1 9 //pwm pins

int count=0;
// object library
Fuzzy *fuzzy = new Fuzzy();
Fuzzy *fuzzy2 = new Fuzzy();
Servo servo1;
Servo servo2;
void setup() {
  // set console and pins
  Serial.begin(9600);
  pinMode(IR, INPUT);
  pinMode(TRIGG, OUTPUT);
  pinMode(ECH, INPUT);
  pinMode(TRIGG1, OUTPUT);
  pinMode(ECH1, INPUT);
  servo1.attach(SERVO);
  servo2.attach(SERVO1);
  // fuzzy sets 16 big
  //ok first input into fuzzy, distance is measured by ultrasonic
  FuzzyInput *distance = new FuzzyInput(1);
  
  FuzzySet *small = new FuzzySet(0, 5, 5, 9);
  //add the distace to the input (logic)
  distance->addFuzzySet(small);

  FuzzySet *safe = new FuzzySet(7, 10, 10, 12);

  distance->addFuzzySet(safe);
  // Instantiating a FuzzySet object
  FuzzySet *big = new FuzzySet(11, 13, 13, 16);
  // Including the FuzzySet into FuzzyInput
  distance->addFuzzySet(big);
  // Including the FuzzyInput into Fuzzy
  fuzzy->addFuzzyInput(distance);


   // Instantiating a FuzzyOutput objects
   //using delay time 
  FuzzyOutput *speed = new FuzzyOutput(1);
  // Instantiating a FuzzySet object
  FuzzySet *fast = new FuzzySet(0, 2, 2, 4);
  // Including the FuzzySet into FuzzyOutput
  speed->addFuzzySet(fast);
  // Instantiating a FuzzySet object
  FuzzySet *average = new FuzzySet(2, 4, 4, 7);
  // Including the FuzzySet into FuzzyOutput
  speed->addFuzzySet(average);
  // Instantiating a FuzzySet object
  FuzzySet *slow = new FuzzySet(4, 7, 7, 10);
  // Including the FuzzySet into FuzzyOutput
  speed->addFuzzySet(slow);
  // Including the FuzzyOutput into Fuzzy
  fuzzy->addFuzzyOutput(speed);

   // Building FuzzyRule "IF distance = small THEN speed = slow"
  // Instantiating a FuzzyRuleAntecedent objects
  FuzzyRuleAntecedent *ifDistanceSmall = new FuzzyRuleAntecedent();
  // Creating a FuzzyRuleAntecedent with just a single FuzzySet
  ifDistanceSmall->joinSingle(small);
  // Instantiating a FuzzyRuleConsequent objects
  FuzzyRuleConsequent *thenSpeedSlow = new FuzzyRuleConsequent();
  // Including a FuzzySet to this FuzzyRuleConsequent
  thenSpeedSlow->addOutput(slow);
  // Instantiating a FuzzyRule objects
  FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifDistanceSmall, thenSpeedSlow);
  // Including the FuzzyRule into Fuzzy
  fuzzy->addFuzzyRule(fuzzyRule01);

  // Building FuzzyRule "IF distance = safe THEN speed = average"
  // Instantiating a FuzzyRuleAntecedent objects
  FuzzyRuleAntecedent *ifDistanceSafe = new FuzzyRuleAntecedent();
  // Creating a FuzzyRuleAntecedent with just a single FuzzySet
  ifDistanceSafe->joinSingle(safe);
  // Instantiating a FuzzyRuleConsequent objects
  FuzzyRuleConsequent *thenSpeedAverage = new FuzzyRuleConsequent();
  // Including a FuzzySet to this FuzzyRuleConsequent
  thenSpeedAverage->addOutput(average);
  // Instantiating a FuzzyRule objects
  FuzzyRule *fuzzyRule02 = new FuzzyRule(2, ifDistanceSafe, thenSpeedAverage);
  // Including the FuzzyRule into Fuzzy
  fuzzy->addFuzzyRule(fuzzyRule02);

  // Building FuzzyRule "IF distance = big THEN speed = high"
  // Instantiating a FuzzyRuleAntecedent objects
  FuzzyRuleAntecedent *ifDistanceBig = new FuzzyRuleAntecedent();
  // Creating a FuzzyRuleAntecedent with just a single FuzzySet
  ifDistanceBig->joinSingle(big);
  // Instantiating a FuzzyRuleConsequent objects
  FuzzyRuleConsequent *thenSpeedFast = new FuzzyRuleConsequent();
  // Including a FuzzySet to this FuzzyRuleConsequent
  thenSpeedFast->addOutput(fast);
  // Instantiating a FuzzyRule objects
  FuzzyRule *fuzzyRule03 = new FuzzyRule(3, ifDistanceBig, thenSpeedFast);
  // Including the FuzzyRule into Fuzzy
  fuzzy->addFuzzyRule(fuzzyRule03);

  
}
int distance(int TRIGGER , int ECHO) {
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  long pulse = pulseIn(ECHO, HIGH) / 2;
  return pulse * 10 / 292;
}

// returns prsence
int presence() {
  return digitalRead(IR);
}

void loop() {
  // put your main code here, to run repeatedly:


//sideway motion 
  for (int i =0; i<180; i++){

    int dist1 = distance(TRIGG, ECH);
    if (dist1 < 0 || dist1 > 16 ) return;
     fuzzy->setInput(1, dist1);
  // Running the Fuzzification
     fuzzy->fuzzify();
     float output = fuzzy->defuzzify(1);
  // Printing something
 // Serial.println("Result: ");
  Serial.print("\t\t\tSpeed: ");
  Serial.println(output);
  // wait 12 seconds
  
  servo2.write(count);
 delay(output);
    
    
  }


//delay for a while before dropping down
  //downward motion 

  for (int i = 0; i<180; i++){
    int dist2 = distance (TRIGG1, ECH1);

    //removes noise from ultrasonic
    if (dist1 < 0 || dist1 > 16 ) return;
    fuzzy2->setInput(1, dist2);
  // Running the Fuzzification
     fuzzy2->fuzzify();
     float output = fuzzy2->defuzzify(1);
  // Printing something
   // Serial.println("Result: ");
    Serial.print("\t\t\tSpeed: ");
    Serial.println(output);
    // wait 12 seconds
    
    servo2.write(count);
   delay(output);
      
  }

  
}
