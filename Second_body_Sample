
//definitions for jumpwire.io
#include <ESP8266WiFi.h>
#include "JWIO_ESP8266_ArduinoIDE.h"
jumpwireIo jwio("wifiSSID","wifipassward",
  "yourtoken","A");

#include <Servo.h>

Servo servo_pin_14;
Servo servo_pin_12;
int _ABVAR_1_P = 0 ;
int _ABVAR_2_Q = 0 ;

unsigned long timer;

void motorA_fw();
void motorB_stop();
void motorA_stop();
void motorB_fw();
void motorB_bw();
void motorA_bw();
void Pservo();
void Qservo();


void setup() {
  jwio.setup(); //always put this code in setup()
  //put your code here
  //pinMode(16, OUTPUT);

  pinMode( 4 , OUTPUT);
  pinMode( 5 , OUTPUT);
  pinMode( 0 , OUTPUT);
  servo_pin_14.attach(14);
  servo_pin_12.attach(12);
  pinMode( 2 , OUTPUT);
  
  timer = millis(); // reset timer
}

void loop() {
  delay(100);
  jwio.loop();
  //put your code
 
}

void Catch(char key, float value){  //Jwio values are changed
  //put your code
  
  
  if (key == 'A') {
    if(value == 0){          //if value of key A is 0
      jwio.Throw('C',0);
      motorA_stop();
      motorB_stop();

    }
    else if(value == 1){     //if value of key A is 1
      jwio.Throw('C',1);
      motorA_fw();
      motorB_fw();
    }
    else if(value == 2){     //if value of key A is 1
      jwio.Throw('C',2);
      motorA_fw();
      motorB_bw();
    }
    else if(value == 3){     //if value of key A is 1
      jwio.Throw('C',3);
      motorA_bw();
      motorB_fw();
    }
    else if(value == 4){     //if value of key A is 1
      jwio.Throw('C',4);
      motorA_bw();
      motorB_bw();
    }
  } //key A end

  if (key == 'P'){
     _ABVAR_1_P = value ;
     Pservo();
     jwio.Throw('J', _ABVAR_1_P );  
    }

   if (key == 'Q'){
     _ABVAR_2_Q = value ;
     Qservo();    
     jwio.Throw('K', _ABVAR_2_Q );
   }
  
}


//myfunction

void motorB_fw()
{
  analogWrite(4 , 1023);
  digitalWrite( 2 , HIGH );
}

void motorB_bw()
{
  analogWrite(4 , 1023);
  digitalWrite( 2 , LOW );
}

void motorB_stop()
{
  analogWrite(4 , 0);
  digitalWrite( 2 , LOW );
}

void motorA_bw()
{
  analogWrite(5 , 1023);
  digitalWrite( 0 , LOW );
}

void motorA_stop()
{
  analogWrite(5 , 0);
  digitalWrite( 0 , LOW );
}

void motorA_fw()
{
  analogWrite(5 , 1023);
  digitalWrite( 0 , HIGH );
}

void Pservo()
{
  servo_pin_14.write( _ABVAR_1_P );
}

void Qservo()
{
  servo_pin_12.write( _ABVAR_2_Q );
}


