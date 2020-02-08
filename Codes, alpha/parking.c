/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : parking
Version : 
Date    : 27/01/2020
Author  : Mohamad
Company : 
Comments: 


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8/000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/
#include <stdlib.h>

#include <mega32.h>

#include <delay.h>

#include <stdio.h>

#include <alcd.h>


unsigned char key = 20;
int acknowledge = 0;
int stay_in = 1;
int zarft[5] = { 10, 8, 12, 4, 8 };
int zarfemp[5] = { 10, 8, 12, 4, 8 };
int point_starter[5] ={ 0, 10, 18, 30, 34 };
int availability[42];
int delay[8] = {100, 60, 40, 30, 20, 15, 7, 3};
unsigned char c[16];
int key_history[4];


//Aesthetic function
void blink(char* s){
    int i=0;
    for(i=0; i<=6; i++){ 
        lcd_clear();
        lcd_gotoxy(2,0);
        lcd_puts(s);
        delay_ms(delay[i]);
        lcd_clear();  
        delay_ms(delay[i+1]);
    }         
    delay_ms(delay[1]);      
    lcd_clear();
}


//interfaces keyboard
void keyboard(void) {

  key = 20; //default value

  //---- COL1 ----
  PORTD .4 = 0;
  delay_ms(2);

  if (PIND .0 == 0) key = 7;
  if (PIND .1 == 0) key = 4;
  if (PIND .2 == 0) key = 1;
  if (PIND .3 == 0) stay_in = 0; 
  PORTD .4 = 1;

  //---- COL2 ----
  PORTD .5 = 0;
  delay_ms(2);

  if (PIND .0 == 0) key = 8;
  if (PIND .1 == 0) key = 5;
  if (PIND .2 == 0) key = 2;
  if (PIND .3 == 0) key = 0; 
  PORTD .5 = 1;

  //---- COL3 ----
  PORTD .6 = 0;
  delay_ms(2);

  if (PIND .0 == 0) key = 9;
  if (PIND .1 == 0) key = 6;
  if (PIND .2 == 0) key = 3;
  if (PIND .3 == 0) acknowledge = 1; //acknowledges the keyboards output. '=' character on keypad.
  PORTD .6 = 1;

  //---- COL4 ----
  PORTD .7 = 0;
  delay_ms(2);

  if (PIND .0 == 0) {
    lcd_putchar('/');
    delay_ms(30);
  }
  if (PIND .1 == 0) {
    lcd_putchar('*');
    delay_ms(30);
  }
  if (PIND .2 == 0) stay_in = 0; //raises the stay_in flag. helps to identify when we should exit loops. 0 key in keypad.
 
  if (PIND .3 == 0) {
    lcd_putchar('+');
    delay_ms(30);
  }
  PORTD .7 = 1;

}


int floor_in(int y){
    int f=5 ;
    int i = 0;
    for(i=0; i<4; i++){
    
        if(y < point_starter[i+1] && y >= point_starter[i]) f = i+1 ;
        
    
    }
    return f;

}


int finder(int x){

    int i = 0;
    int min_floor_dist = 6;
    int min_unit_dist = 13; 
    int best_i = 0;
    
    for(i=0; i<42 ; i++){
    
        int floor_dist = abs(floor_in(i)-key_history[0]);
        int unit_dist = abs(i-x);
        if(availability[i] == 0 && floor_dist < min_floor_dist){
        
            min_floor_dist = floor_dist;
            min_unit_dist = unit_dist;
            best_i = i;  
        
        } 
       
        else if(availability[i] == 0 && floor_dist == min_floor_dist && unit_dist < min_unit_dist){
        
            min_floor_dist = floor_dist;
            min_unit_dist = unit_dist;
            best_i = i;                 
        
        }
    
    }
    
    return best_i;

}


void deliver(int p){
    

    if(availability[p] == 0){
    
        availability[p] = 1;
        zarfemp[key_history[0]-1] -= 1;
        
        lcd_clear();
        sprintf(c,"    U Got It");
        lcd_puts(c);
        lcd_gotoxy(0,1);  
        lcd_puts("Press C To Exit");
        
        while(stay_in){
          keyboard();
        }
    
    } 
    
    else if(availability[p] == 1){  
        int empty_point = finder(p);
        int fop = floor_in(empty_point);
        int uop = empty_point - point_starter[fop-1]+1;
        availability[empty_point] = 1; 
        zarfemp[floor_in(empty_point)-1] -= 1;
        
        sprintf(c,"Take %d%d Instead", fop, uop); 
        lcd_clear();
        lcd_puts(c);
        lcd_gotoxy(0,1);
        lcd_puts("Press C To Exit");
        
        while(stay_in){
          keyboard();
        }
    
    }

}


void stats(void){
    
    int key_his = 20;
    acknowledge = 0;
    
    lcd_clear();
    lcd_puts("Which Floor?"); 
    delay_ms(30);
    lcd_gotoxy(0,1);
    
    while(stay_in){
        
        keyboard();
        if(key!=20 && acknowledge == 0 && stay_in == 1){
        
            sprintf(c,"%d",key);
            lcd_puts(c);
            delay_ms(30); 
            key_his = key;
        
        }  
        
        if(acknowledge == 1){
        
            lcd_clear();
            sprintf(c, "Total: %d", zarft[key_his-1]); 
            lcd_puts(c);              
            lcd_gotoxy(0,1);
            sprintf(c, "Available: %d", zarfemp[key_his-1]);  
            lcd_puts(c);
            
            acknowledge = 0;
            delay_ms(30);
        
        }
        
    
    }

}


void ticket(void){
    int point = 0;
    int i = 0;
    acknowledge = 0;
    
    lcd_clear();
    lcd_puts("Reserve?"); 
    delay_ms(30);
    lcd_gotoxy(0,1);  
    lcd_puts("---");  
    lcd_gotoxy(0,1);  
    
    while(stay_in){
        
        keyboard();
        if(key!=20 && acknowledge == 0 && stay_in == 1){
        
            sprintf(c,"%d",key);
            lcd_puts(c);
            delay_ms(30); 
            key_history[i] = key;
            i++;
            
            if(i == 4){
                lcd_clear();
                lcd_puts("reselect");
                lcd_gotoxy(0,1);
                lcd_puts("---");
                lcd_gotoxy(0,1);
                i = 0;          
                delay_ms(30);
            } 
            
        }  
        
        if(acknowledge == 1){
        
            key_history[i] = 98;
            
            if(key_history[2] == 98){
            
                point = point_starter[key_history[0]-1] + key_history[1]-1 ;  //points to the availability array, to see it's correct try the 11 and 58 user entry.
                deliver(point);
            }
            
            else if(key_history[3] == 98){
            
                point =  point_starter[key_history[0]-1] + key_history[2]-1 + 10 ;   
                deliver(point);
            
            }
            
            acknowledge = 0;
        
        }
    
    }
}


void home_panel(void){
    int key_his = 20;
    acknowledge = 0;
    stay_in = 1;
    
    lcd_clear();
    lcd_puts("1.Stats 2.Ticket");
    delay_ms(30);
    lcd_gotoxy(0,1);  
    
    while(stay_in){
        
        keyboard();
        if(key!=20 && acknowledge == 0 && stay_in == 1){
        
            sprintf(c,"%d",key);
            lcd_puts(c); 
            key_his = key;
            delay_ms(30);
        
        }              
        
        if(acknowledge == 1){
                
            switch(key_his){
                
                case 1:
                    stats();
                    acknowledge = 0;
                    break;
                case 2:
                    ticket();
                    acknowledge = 0;
                    break;
            } 
            delay_ms(30);
                   
        } 
    
    }

}



void main(void) {

  PORTA = 0x00;
  DDRA = 0x00;
  PORTB = 0x00;
  DDRB = 0x00;
  PORTC = 0x00;
  DDRC = 0xF7;
  PORTD = 0xFF;
  DDRD = 0xF0; 
  
  lcd_init(16);
  blink("W E L C O M E");
  
  while (1) {
   home_panel();
  }
}