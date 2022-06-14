#include<lpc21xx.h>
#include<string.h>
 char password[5];
 char userentry[5];
void delay(unsigned int x);
void newpassword(void);


           // lcd functions
void data(unsigned int a);     //to display on keyboard
void lcd_init(void);           // initializing of lcd display
void cmd(unsigned int a);      // writing commands to lcd command register
void show( char[] );           // display a string on lcd

       // dc motor functions
void clockwise(void);         // rotate the motor in clockwise direction(opening of door)
void anti_clockwise(void);    // rotate the motor in anticlockwise direction(closing of door)


        //keypad functions
#define c1 (1<<20)          //column 1 connected to P1.20
#define c2 (1<<21)
#define c3 (1<<22)
#define r1 (1<<16)					//row1 is connected to P1.16
#define r2 (1<<17)
#define r3 (1<<18)
#define r4 (1<<19)


 char keypad(void);        // function to get key 
 char flag=0;

int main(void)
{
	
	
	int count,done=0;
	PINSEL0=0x00000000;     // make all pins as GPIO
	IO0DIR=0xFFFFFFFF;      //make all pins as output pins

	lcd_init();
	
	while(done==0) 
	{	
		cmd(0x01);
    cmd(0x80);              //command 0x80 forces the cursor to beginning of first line
	  show("press # to");
	  cmd(0xc0);      					// 0xc0 forces the cursor to  second line of cursor
	  show("change password");
	  flag=keypad();
    delay(1000);		
	if(flag=='#')            
		{
			newpassword();      //this function takes the new password
			done=1;
		}
	else
	{
		cmd(0x01);
		cmd(0x80);
		show("Wrong key pressed");
		delay(10000);
		}
	}
	  
		while(1)
		{
		cmd(0x01);
		cmd(0x80);
		show("Enter password");
	
		cmd(0xc7);
		for(count=0;count<4;count++)		//take the password from user
		{
			userentry[count]=keypad();
			data('*');
			//data(userentry[count]);
		}
		userentry[count]=0;
		if(strcmp(password,userentry)!=0)		//check if the password is correct
		{
			
			
			cmd(0xC0);
			show("WRONG PASSWORD"); 					//if not print appropriate message
			cmd(0xC0);
			
		}
		else
		{
			cmd(0x01);
			cmd(0xC0);
			show("Door opening...");			//opening of door
			clockwise();
			delay(855000);
			cmd(0x01);
			cmd(0xC0);
			show("Door closing...");			
			anti_clockwise();				//closing of door
			delay(9500000);
		}
		cmd(0x01);
	}
			
}

void delay(unsigned int x)			
{
	unsigned int b;
	for(b=0;b<x;b++);
}

void data(unsigned int a)
{
	IO0PIN&=0x00;							//set all ports to 0
	IO0SET=0x00000010;				//RS =1 for data 
	IO0CLR=0x00000020;        //R/W =0
	IO0PIN=( IO0PIN | (a & 0xFF) << 8);
	IO0SET=1<<6;							//giving a high to low pulse for enable pin
	delay(10000);
	IO0CLR=0x00000040;
}

void cmd(unsigned int a)
{
	IO0PIN&=0x00;					//set all pins to 0
	IO0CLR=0x00000010;		//RS=0 for command 
	IO0CLR=0x00000020;
	IO0PIN=( (a & 0xFF) << 8);  //high to low pulse for enable pin
	IO0SET=0x00000060;
	delay(10000);
	IO0CLR=1<<6;
}

void lcd_init()
{
	cmd(0x38);  //choose 5x7 matrix and 16x2 mode
	cmd(0x0e);	//curosr blinking
	cmd(0x01);		//clear screen
	cmd(0x06);		//increment cursor
	cmd(0x87);		//to the center of screen
}

void show( char *s)				//function to dsiplay a string 
{													// takes a character point as argument
    
	     while(*s)
	{
		data(*s++);
		delay(10000);
		
	}
}

void clockwise()				// function to rotate motor in clockwise rotation
												//PO.20 is connected to input 1
{ 
	
	IO0CLR=0x00300000;                      //P0.21 is connected to input 2
	delay(400000);
	IO0SET=0x00100000;
	delay(300000);
	IO0CLR=0x00300000;   
}

void anti_clockwise()					//function to rotate the motor in anticlockwise direction
{
	
	IO0CLR=0x00300000;
	//delay(300000);
	IO0SET=0x00200000;
	delay(300000);
	IO0CLR=0x00300000;
	
}

void newpassword(void)			//this function takes a new password and stores it into password character array
{
	unsigned int con;
	cmd(0x01);
			cmd(0x80);
			show("New password");
			cmd(0xc7);									//0xc7 forces the cursor to middle of second line
			for(con=0;con<4;con++)      //save the new password
			{
				password[con]=keypad();
				data('*');
			}
			password[con]=0;
		}

char keypad(void)       				//function to get a key from the keyboard
{
 IO1DIR=0x0f<<16;								//make port pins p1.16 to p1.19 as output pins (i.e, row pins as output pins)
  IO1CLR|=0xFF<<16;							//clear the row and colums pins to 0
 IO1SET|=0xFF<<16;							//set the row and column pins (p1.16 to p1.22) to 1
 while(1)												//this loop scans for the key pressed by sequentially pulling each row low and reading the coreesponding columns
																//after all the columns of a row are read its again made high and the next row is made low and so on
 {															
   IO1CLR|=r1;										//make row 1 as low	
  IO1SET|=0x0E<16;								//other rows should be high
   if((IO1PIN&c1)==0)							//read each column
   {
     while((IO1PIN&c1)==0);
  return '1';
   }
   else if((IO1PIN&c2)==0)				// after making any of the columns low and a key of that row is pressed then the column coressponding column becomes 
																	//low due to short circuit
   {
     while((IO1PIN&c2)==0);				//wait till the key is released  
  return '2';
   }
   else if((IO1PIN&c3)==0)
   {
     while((IO1PIN&c3)==0);
  return '3';
   }
    IO1CLR|=r2;								//make r1 as low
  IO1SET|=0x0D<<16;           //other rows should be high
    if((IO1PIN&c1)==0)
   {
     while((IO1PIN&c1)==0);
  return '4';
   }
  else if((IO1PIN&c2)==0)
   {
     while((IO1PIN&c2)==0);
  return '5';
   }
  else if((IO1PIN&c3)==0)
   {
     while((IO1PIN&c3)==0);
  return '6';
   }
   IO1CLR|=r3;
  IO1SET|=0X0B<<16;
    if((IO1PIN&c1)==0)
   {
     while((IO1PIN&c1)==0);
  return '7';
   }
  else if((IO1PIN&c2)==0)
   {
     while((IO1PIN&c2)==0);
  return '8';
   }
  else if((IO1PIN&c3)==0)
   {
     while((IO1PIN&c3)==0);
  return '9';
   }
  IO1CLR|=r4;
   IO1SET|=0X07<<16;
       if((IO1PIN&c1)==0)
   {
     while((IO1PIN&c1)==0);
  return '*';
   }
  else if((IO1PIN&c2)==0)
   {
     while((IO1PIN&c2)==0);
  return '0';
   }
  else if((IO1PIN&c3)==0)
   {
     while((IO1PIN&c3)==0);
  return '#';
   }
 }
}
