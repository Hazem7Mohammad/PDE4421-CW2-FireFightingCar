#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>

LiquidCrystal_I2C lcd(0x3E,16,2);

int x;
//Commented the ROS settings
//The ROS part, allows the movement of the servomotor by analyzing the data of the middle flame sensor. 
//If the value is above a threshold the code goes to the extinguishing function and move the servomotor.

ros::NodeHandle  nh;
//Callback function, depending on the number (0 or 1) the code is going to extinguishing funtion, to move the servomotor and blink an LED
void messageCb( const std_msgs::Int32& opengate){
  if (opengate.data == 1){
    extinguishing();
  }
}

//Set up of the topic which the arduino script is subcribed(Type: Integer)
ros::Subscriber<std_msgs::Int32> sub("Print", messageCb );
//Set up the publisher called Button(Type: Integer)
std_msgs::Int32 button_status;
ros::Publisher Button("Button", &button_status);


 

const int btn_pin = 0;
int btn_prev = HIGH;
int counter = 0;

int servo = 10;

int angle;
int angle1;
int angle2;
int pwm;

// ultra right 
const int trigPin = 11;
const int echoPin = 12;
long duration;
int ultra1;

// ultra left 
const int trigPin2 = 2;
const int echoPin2 = 6;
int ultra2;

int left, mid , right;
// Motor A
 
int enA = 9;
int in1 = 8;
int in2 = 7;
 
// Motor B
 
int enB = 3;
int in3 = 5;
int in4 = 4;


void setup() {
  //Serial.begin(57600);
  pinMode(btn_pin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(servo, OUTPUT);

// lcd 
//iniciate the nodes and turn on the Button topic and the subcribe topic
  nh.initNode();
  nh.advertise(Button);
  nh.subscribe(sub);

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

//------------------------------------------------------
  int btn_state;
  btn_state = digitalRead(0);
  if ( (btn_prev == HIGH) && (btn_state == LOW) ) {
    counter++;
    //Serial.println(counter);
  }
//------------------------------------------------------
    //ultra1
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    ultra1= duration*0.034/2;
    //Serial.print("ultra1: ");
    //Serial.println(ultra1);
    delay(10);
//ultra2
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration = pulseIn(echoPin2, HIGH);
    ultra2= duration*0.034/2;
    //Serial.print("ultra2: ");
    //Serial.println(ultra2);
    delay(10);
//-------------------------------------------------------
//-------------------------------------------------------
    left = analogRead(A1);
    mid = analogRead(A2);
    right = analogRead(A3);
  
    //Serial.print("left");
    //Serial.println(left);
    //Serial.print("mid");
    //Serial.println(mid);
    //Serial.print("right");
    //Serial.println(right);

// car flow 
if ((counter % 2 == 0)){
    off();
  }else{
  if ((left>18) || (mid > 18 ) || (right > 18) && (counter % 2 != 0)) {
  
  
      func2();
  
    }else if ((left<18) || (mid < 18 ) || (right < 18) && (counter % 2 != 0)) {
      func1();
    }
  }

  btn_prev = btn_state;  
}


//-------------------Functions-------------------------------




// avoidance function 
void func1(){
  
  searchingfire();
   
  angle2 = 30;
  servoPulse(servo, angle2);
  
  if (ultra1 > 40 and ultra2 > 40){
    forward();
  }  
  
  else if ( ultra1 < 40 or ultra2 < 40 and ultra2 > ultra1 ){
    lefts();
  }

  else if ( ultra1 < 40 or ultra2 < 40  and ultra1 > ultra2 ){
    rights();
  }


}



// fire function 
void func2(){
  firelocated();
  if ((left > mid) && (left > right )) {

    lefts();
  }

  if ((mid > left) && (mid > right )) {

    forward();

  }

  if ((right > mid) && (right > left )) {

    rights();


  }



  if ((mid > 620)) {

    off();
    mid = analogRead(A2);
    button_status.data = mid;
    //publishin the data acquired by the middle flame sensor, sending the information to the python file
    Button.publish( &button_status );
    nh.spinOnce();
    
  }
}







//lcd functions 

void searchingfire() {
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Searching for"); 
  lcd.setCursor(0, 1);
  lcd.print("fire...");
}

void firelocated() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fire located!");
}




// servo function 

void extinguishing(){
  angle1 = 140;
  servoPulse(servo, angle1);
  lcd.setCursor(0, 1);
  lcd.print("Extinguishing");
  
  //delay(1000);

  for ( int x = 1 ; x < 15; x++){
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }

//  
//  angle2 = 30;
//  servoPulse(servo, angle2);
  //delay(1000);
}

void servoPulse (int servo, int angle)
{
 pwm = (angle*11) + 500;      
 digitalWrite(servo, HIGH);
 delayMicroseconds(pwm);
 digitalWrite(servo, LOW);
 delay(50);                   
}







// car movement 
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
  analogWrite(enB, 90);
  analogWrite(enA, 90);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void lefts(){
  analogWrite(enB, 90);
  analogWrite(enA, 90);
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
