#include <linux/i2c-dev.h>      //Changed to i2c-dev-user.h when checking on ZedBoard
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define ADDR 0x77     //i2c adress for read for temp sensor

static int file;
static int16_t ac5, ac6, mc, md;   //Calibration constants

//Generic wrapper for the smbus functions to read and concatenate 2 register values. Will not be used externally.
int16_t get_cal(int msb, int lsb)
{
  	int16_t val1, val2, val;
  
  	val1 = i2c_smbus_read_byte_data(file, msb);
  	if(val1<0)
	{
	  perror("\nI2C transaction 1 failed");
	}
 
  	val2 = i2c_smbus_read_byte_data(file, lsb);
  	if(val2<0)
	{
	  perror("\nI2C transaction 2 failed");
	}
  
  	val = (val1 << 8) | val2;
  	return val;
}

void init_gy80()
{
  	int adapter_nr = 0;	//Dynamically determined
	char filename[20];
	
	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);	
	file = open(filename, O_RDWR);
	
	if(file<0)
	{
		perror("\nFile not found");
		exit(1);
	}

  	if(ioctl(file, I2C_SLAVE, ADDR)<0)
  	{
    	perror("\n Slave not found");
    	exit(1);
  	}
    
  	ac5 = get_cal(0xB2, 0xB3);		
	ac6 = get_cal(0xB4, 0xB5);
	mc  = get_cal(0xBC, 0xBD);
	md  = get_cal(0xBE, 0xBF);
}

float get_temp()
{
  	int16_t val1, val2, temp_raw;
		
	__u8 msb = 0xF6;		//Temp sensor reg
	__u8 lsb = 0xF7;
	__s32 res;
		
	int writeto = 0xF4;
	int writeval = i2c_smbus_write_byte_data(file, writeto, 0x2E);    //This requests a temp reading
	if(writeval < 0)
	{
	  perror("\nWriting of \"0x2E\" to reg 0xF4 failed");
	} 
	
	usleep(4500);
	
	temp_raw = get_cal(msb, lsb);
	
	/**************Calculation of true temp****************/
	float x1 = ((int)(temp_raw - ac6))*((int)ac5);
  	int i;
  
  	for(i = 0; i < 15; i++)       //For lack of math.h
  	{
    	x1 = x1/2;
  	}
  
  	float x2 = mc/(x1+md);
  	for(i = 0; i < 11; i++)
  	{
    	x2 = x2*2;
  	}	
  
  	float b5 = x1+x2;
  
  	float temp = (b5+8);
  	for(i = 0; i < 4; i++)
  	{
    	temp = temp/2;
  	}
  
  	temp=temp/10;   //Temp calculated before this is in 0.1 degree C
  	return temp;
}

void clear_gy80()
{
	close(file);
}
