#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3E,16,2); 


//-----------------------button pin -----------------
const int btn_pin = 0;
int btn_prev = HIGH;
int counter = 0;


// ----------------------servo ------------------
int servo = 10;     // servo pin

int angle;
int angle1;
int angle2;
int pwm;

// -------------------------ultra right connection pins ----------------
const int trigPin = 11;
const int echoPin = 12;
long duration;
int ultra1;

// ultra left connection pins 
const int trigPin2 = 2;
const int echoPin2 = 6;
int ultra2;

int left, mid , right;


// -------------- H-bridge connection pins -------- 
// Motor A
 
int enA = 9;
int in1 = 8;
int in2 = 7;
 
// Motor B
 
int enB = 3;
int in3 = 5;
int in4 = 4;


void setup() {
  
  Serial.begin(9600);
  pinMode(btn_pin, INPUT_PULLUP);  // button pin
  pinMode(13, OUTPUT);             // LED pin
  pinMode(servo, OUTPUT);          // servo pin 

// lcd  pins connections. 

  lcd.init(); 
  lcd.backlight();
  

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);


  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
}

void loop() {
//--------------------------Push Button ----------------------------
  int btn_state;
  btn_state = digitalRead(btn_pin);      // reading if the button is pressed and saving the push in variable
  if ( (btn_prev == HIGH) && (btn_state == LOW) ) {
    /* with every press of the button the press is saved to button state variable
    then compared to previous button value set to high. 
    if they are diffrent values then that means the button is pressed
    and the counter should count one */
    counter++;
    Serial.println(counter);         // printing the value on serial monitor
  }
//--------------------------ultrasonic sensors ----------------------------
    // the right ultrasonic sensor echoing the sound and getting distance back
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    ultra1= duration*0.034/2;
    Serial.print("ultra1: ");        // printing the value on serial monitor
    Serial.println(ultra1);
    delay(10);
    // the left ultrasonic sensor echoing the sound and getting distance back
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration = pulseIn(echoPin2, HIGH);
    ultra2= duration*0.034/2;
    Serial.print("ultra2: ");       // printing the value on serial monitor
    Serial.println(ultra2);
    delay(10);
//-------------------------------------------------------
//------------------------falme sensor-------------------------------
    // reading the input values of the flame sensor
    left = analogRead(A1);
    mid = analogRead(A2);
    right = analogRead(A3);
    // printing the values on the serial monitor 
    Serial.print("left");
    Serial.println(left);
    Serial.print("mid");
    Serial.println(mid);
    Serial.print("right");
    Serial.println(right);


//-------------------ROBOT MAIN FUNCTION-------------------------------
if ((counter % 2 == 0)){        // checking button status using a counter value
    off();                      // if counter is even turn off motors
    
  }else{                        // if counter is odd check the flame sensor values
    
  if ((left>18) || (mid > 18 ) || (right > 18) && (counter % 2 != 0)) {
  // if the flame values are more than 18 start the fire located function
      func2();
    }else if ((left<18) || (mid < 18 ) || (right < 18) && (counter % 2 != 0)) {
  // if the flame values are less than 18 start the obstacles avoidance function. 
      func1();
    }
  }

  btn_prev = btn_state; 
  // make previous button status as the current status  
}


//-------------------Functions-------------------------------


/* obstacles avoidance function */
void func1(){
  // first show the searching for fire message on the LCD screen 
  searchingfire();
  if (ultra1 > 40 and ultra2 > 40){   // if there are no obsacles on the left side and right side go forward. 
    forward();
  }  
  else if ( ultra1 < 40 or ultra2 < 40 and ultra2 > ultra1 ){ 
    // if there is obstacles on the right turn left 
    lefts();
  }
  else if ( ultra1 < 40 or ultra2 < 40  and ultra1 > ultra2 ){
    // if there is obstacles on the left turn right 
    rights();
  }
}



/* fire located function 
this function compares between the three values of the flame sensor
finds the largest value to turn the motors in the direction towards the fire
when the fire is located*/
void func2(){
  firelocated();
  if ((left > mid) && (left > right )) {
    lefts();  // car goes left 
  }
  if ((mid > left) && (mid > right )) {
    forward(); // car goes forward
  }
  if ((right > mid) && (right > left )) {

    rights(); // car goes right
  }
/* if the intencity of the fire is more than 620 stop the car and start the 
extinguishing process. */
  if ((mid > 620)) {
    off();
    extinguishing();
  }
}





//-------------------LCD screen for each state-------------------------------

//lcd functions 

void searchingfire() {     // shown when the car is avoiding obstacles
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Searching for"); 
  lcd.setCursor(0, 1);
  lcd.print("fire...");
}

void firelocated() {       // shown when the fire is located 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fire located!");
}

//-------------------servo and LED -------------------------------

// servo control function. 
void servoPulse (int servo, int angle)
// inputs as servo name, and angle values 
{
 pwm = (angle*11) + 500;   // calulating the time to go to the right angle.
 digitalWrite(servo, HIGH);
 delayMicroseconds(pwm);
 digitalWrite(servo, LOW);
 delay(50);                   
}


/* extinguising function using servo function to simulate opening hose
 in the direction of the fire */
void extinguishing(){
  angle1 = 140;                    // default servo angle
  servoPulse(servo, angle1);
  delay(1000);
  
  for ( int x = 1 ; x < 51; x++){  // LED on and off for 10 seconds
    digitalWrite(13, HIGH);        // simulating water flow. 
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }
  angle2 = 30;                     // angle when fire is found to point at it. 
  servoPulse(servo, angle2);
  delay(1000);
}


//-------------------H-bridge-------------------------------
// car movement functions: enabling required motors with appropriate speeds
void backward(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 90);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 90);
}
void forward(){
  analogWrite(enB, 90);
  analogWrite(enA, 90);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW); 
}
void rights(){
  analogWrite(enB, 100);
  analogWrite(enA, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void lefts(){
  analogWrite(enB, 100);
  analogWrite(enA, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}
void off(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
