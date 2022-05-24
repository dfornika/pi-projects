#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringShift.h>

#define dataPin   3   //DS Pin of 74HC595(Pin14)
#define latchPin  2   //ST_CP Pin of 74HC595(Pin12)
#define clockPin  0   //CH_CP Pin of 74HC595(Pin11)

#define buttonPin 28  //define the buttonPin (north/blue)
//#define buttonPin 27  //define the buttonPin (east/green)
//#define buttonPin 29  //define the buttonPin (south/red)
//#define buttonPin 25  //define the buttonPin (west/yellow)



void _shiftOut(int val){   
  int i;  
  for(i = 0; i < 16; i++){
    digitalWrite(clockPin, LOW);

    digitalWrite(dataPin,((0x8000&(val<<i)) == 0x8000) ? HIGH : LOW);
    delayMicroseconds(1);

    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
  }
}

void outData(unsigned long data){
  digitalWrite(latchPin, LOW);
  _shiftOut(data);
  digitalWrite(latchPin, HIGH);
}


unsigned long selectDigit(unsigned long digit){  
  if (digit == 0x01) {
    return (0x08 << 8);
  } else if (digit == 0x02) {
    return (0x04 << 8);
  } else if (digit == 0x04) {
    return (0x02 << 8);
  } else if (digit == 0x08) {
    return (0x01 << 8);
  } else {
    return (0xf0 << 8);
  }
}

unsigned char num[] = {
  0xc0, // 0
  0xf9, // 1
  0xa4, // 2
  0xb0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xf8, // 7
  0x80, // 8
  0x90, // 9
};

void display(int dec) {  //display function for 7-segment display
  int delays = 1;
  unsigned long  digit;
  outData(0xffff);	
  digit = selectDigit(0x01);          //select the first, and display the single digit
  outData(num[dec%10]|digit);   
  delay(delays);                      //display duration
  
  outData(0xffff);    
  digit=selectDigit(0x02);            //select the second, and display the tens digit
  outData(num[dec%100/10]|digit);
  delay(delays);
  
  outData(0xffff);    
  digit=selectDigit(0x04);            //select the third, and display the hundreds digit
  outData(num[dec%1000/100]|digit);
  delay(delays);
  
  outData(0xffff);    
  digit=selectDigit(0x08);            //select the fourth, and display the thousands digit
  outData(num[dec%10000/1000]|digit);
  delay(delays);
}

void main(void) {
  
  wiringPiSetup();	               //Initialize wiringPi.	

  pinMode(buttonPin, INPUT);           //Set buttonPin to input
  pullUpDnControl(buttonPin, PUD_UP);  //pull up to HIGH level
  
  pinMode(dataPin,OUTPUT); 
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);

  int counter = 0;    //variable counter,the number will be displayed by 7-segment display
  int lock_counter = 0;
  
  digitalWrite(latchPin, LOW);

  while(1) {
    display(counter);
    if (digitalRead(buttonPin) == LOW) { //button is pressed
      if (lock_counter == 0) {
	counter++;
	lock_counter = 1;
        // printf("%d\n", counter);
      }
    } else if (digitalRead(buttonPin) == HIGH) {
      lock_counter = 0; 
    }
  }
}

