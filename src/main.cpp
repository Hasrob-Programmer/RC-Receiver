#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RFMessage.h>

#define MAX_FORWARD_THRESH 900
#define MIN_FORWARD_THRESH 780
#define MIN_BACKWARD_THRESH 260
#define MAX_BACKWARD_THRESH 100


#define LOW_SPEED_PWM 100
// #define MEDIUM_SPEED_PWM 180
#define HIGH_SPEED_PWM 250
#define STOP_PWM 0

enum SPEED_MODE
{
  FORWARD, FORWARD_LEFT, FORWARD_RIGHT, ROTATE_LEFT, ROTATE_RIGHT, BACKWARD_LEFT, BACKWARD_RIGHT, BACKWARD, STOP
};


//Pin RF
const int rf_ce = 8;
const int rf_csn = 9;

//Pin Motor
const int motor_en_a = 3;
const int motor_en_b = 5;
const int motor_out_1 = 2;
const int motor_out_2 = 4;
const int motor_out_3 = 6;
const int motor_out_4 = 7;

RF24 radio(rf_ce, rf_csn); // CE, CSN

const byte address[6] = "00001";

long last_print_ms, last_read_ms;
int print_ms, read_ms;

RFMessage rf_msg;

void setSpeed(int mode_)
{
  switch (mode_)
  {
  case SPEED_MODE::FORWARD:
    digitalWrite(motor_out_1, HIGH);
    digitalWrite(motor_out_2, LOW);
    digitalWrite(motor_out_3, HIGH);
    digitalWrite(motor_out_4, LOW);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("FORWARD");
    break;

  case SPEED_MODE::FORWARD_LEFT:
    digitalWrite(motor_out_1, HIGH);
    digitalWrite(motor_out_2, LOW);
    digitalWrite(motor_out_3, HIGH);
    digitalWrite(motor_out_4, LOW);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, LOW_SPEED_PWM);
    Serial.println("FORWARD_LEFT");
    break;

  case SPEED_MODE::FORWARD_RIGHT:
    digitalWrite(motor_out_1, HIGH);
    digitalWrite(motor_out_2, LOW);
    digitalWrite(motor_out_3, HIGH);
    digitalWrite(motor_out_4, LOW);
    analogWrite(motor_en_a, LOW_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("FORWARD_RIGHT");
    break;
  
  case SPEED_MODE::BACKWARD:
    digitalWrite(motor_out_1, LOW);
    digitalWrite(motor_out_2, HIGH);
    digitalWrite(motor_out_3, LOW);
    digitalWrite(motor_out_4, HIGH);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("BACKWARD");
    break;

  case SPEED_MODE::BACKWARD_LEFT:
    digitalWrite(motor_out_1, LOW);
    digitalWrite(motor_out_2, HIGH);
    digitalWrite(motor_out_3, LOW);
    digitalWrite(motor_out_4, HIGH);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, LOW_SPEED_PWM);
    Serial.println("BACKWARD_LEFT");
    break;

  case SPEED_MODE::BACKWARD_RIGHT:
    digitalWrite(motor_out_1, LOW);
    digitalWrite(motor_out_2, HIGH);
    digitalWrite(motor_out_3, LOW);
    digitalWrite(motor_out_4, HIGH);
    analogWrite(motor_en_a, LOW_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("BACKWARD_RIGHT");
    break;

  case SPEED_MODE::ROTATE_LEFT:
    digitalWrite(motor_out_1, HIGH);
    digitalWrite(motor_out_2, LOW);
    digitalWrite(motor_out_3, LOW);
    digitalWrite(motor_out_4, HIGH);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("ROTATE_LEFT");
    break;

  case SPEED_MODE::ROTATE_RIGHT:
    digitalWrite(motor_out_1, LOW);
    digitalWrite(motor_out_2, HIGH);
    digitalWrite(motor_out_3, HIGH);
    digitalWrite(motor_out_4, LOW);
    analogWrite(motor_en_a, HIGH_SPEED_PWM);
    analogWrite(motor_en_b, HIGH_SPEED_PWM);
    Serial.println("ROTATE_RIGHT");
    break;
  
  default:
    digitalWrite(motor_out_1, LOW);
    digitalWrite(motor_out_2, LOW);
    digitalWrite(motor_out_3, LOW);
    digitalWrite(motor_out_4, LOW);
    analogWrite(motor_en_a, STOP_PWM);
    analogWrite(motor_en_b, STOP_PWM);
    Serial.println("STOP");
    break;
  }
}

void setupPinMotor()
{
  pinMode(motor_en_a, OUTPUT);
  pinMode(motor_en_b, OUTPUT);
  pinMode(motor_out_1, OUTPUT);
  pinMode(motor_out_2, OUTPUT);
  pinMode(motor_out_3, OUTPUT);
  pinMode(motor_out_4, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
     Serial.println(F("radio hardware not responding!"));
     while (1) {} 
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  setupPinMotor();
  
  rf_msg.x_axis = 500;
  rf_msg.y_axis = 500;
}

void loop() {
  if (radio.available()) {
    unsigned char text[32] = "";
    radio.read(&text, sizeof(text));
    rf_msg.getValueFromBuffer(text, RFMESSAGE_LEN);
  }

  if(rf_msg.x_axis > MAX_FORWARD_THRESH) //move forward x axis
  {
    //move to right or left y axis
    if(rf_msg.y_axis > MIN_FORWARD_THRESH)
      setSpeed(SPEED_MODE::FORWARD_RIGHT);
    else if(rf_msg.y_axis < MIN_BACKWARD_THRESH)
      setSpeed(SPEED_MODE::FORWARD_LEFT);
    else
      setSpeed(SPEED_MODE::FORWARD);
  }
  else if(rf_msg.x_axis < MAX_BACKWARD_THRESH) //move backward x axis
  {
    //move to right or left y axis
    if(rf_msg.y_axis > MIN_FORWARD_THRESH)
      setSpeed(SPEED_MODE::BACKWARD_RIGHT);     
    else if(rf_msg.y_axis < MIN_BACKWARD_THRESH)
      setSpeed(SPEED_MODE::BACKWARD_LEFT);
    else
      setSpeed(SPEED_MODE::BACKWARD);
  }
  else if(rf_msg.y_axis > MAX_FORWARD_THRESH)
  {
    setSpeed(SPEED_MODE::ROTATE_RIGHT);
  }
  else if(rf_msg.y_axis < MAX_BACKWARD_THRESH)
  {
    setSpeed(SPEED_MODE::ROTATE_LEFT);
  }
  else
  {
    setSpeed(SPEED_MODE::STOP);
  }
}