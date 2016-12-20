#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>

#define addr 0x68

static int file;
static __s32 res;
static __u8 reg;
static __u8 values[14];   //array to hold all the register values

void set_sleep_gy521(int flag)
{
	if(flag==0)    //wake up the device
	{
		//Accessing reg 107
		reg = 0x6B;
		uint8_t val8 = 0x01;     //write 0x00 if you want to set the internal 8MHz oscillator as CLK                                              
		res = i2c_smbus_write_byte_data(file, reg, val8);
		if(res<0) 
			perror("Failed to wake it up\n");
		else 
			printf("Device is awake\n");
	}
	else      //set it to sleep
	{
		reg = 0x6B;
		uint8_t val8 = 0x41;   //write 0x40 if you want to set the internal 8MHz oscillator as CLK                                             //
		res = i2c_smbus_write_byte_data(file, reg, val8);
		if(res<0) 
			perror("Failed to go to sleep\n");
		else 
			printf("In sleep mode\n");
	}
}

void init_gy521()
{
	char filename[20];
	int adapter_no = 0;
	snprintf(filename, 19, "/dev/i2c-%d", adapter_no);
	file = open(filename, O_RDWR);
	if(file<0)
	{
		perror("File not opened");	
		exit(1);
	}
	if(ioctl(file, I2C_SLAVE, addr)<0)
	{
		perror(":( Not able to access the device\n");
		exit(EXIT_FAILURE);
	}

	//setting the sensitivity of the gyroscope and accelerometer

	res = i2c_smbus_write_byte_data(file, 0x1B, 0x00);
	if(res<0)
		perror("Failed to set gyro range\n");
	res = i2c_smbus_write_byte_data(file, 0x1C, 0x00);
	if(res<0)
		perror("Failed to set the accelerometer range\n");

	set_sleep_gy521(0);  //this also sets the clock source to X-axis gyro reference which is slightly better than the internal 8MHz oscillator
}

//get_values() stores all the register measurements in the array values

int get_values()
{
	//reading all the values needed at once in a block
	res = i2c_smbus_read_i2c_block_data(file, 0x3B, 14, (__u8*)values);     
	if(res<0)
		perror("Failed to read using Block");
	return res;
}

float get_Ax()
{
	int c = get_values();      //calls get_values() to get all values at a time instant
	int16_t xout;
	if(c>0)
		xout = (((int16_t)values[0])<<8) | values[1];
	else
	{	
		perror("Can't get the values");
		exit(EXIT_FAILURE); 
	}
	return xout/16384.0*9.8;
}

float get_Ay()
{	
	//concatenate the higher byte and the lower byte
	int16_t yout = (((int16_t)values[2])<<8) | values[3];
	return yout/16384.0*9.8;
}

float get_Az()
{
	int16_t zout = (((int16_t)values[4])<<8) | values[5];
	return zout/16384.0*9.8;
}

float get_temp_gy521()
{
	__s16 temp = (((int16_t)values[6])<<8) | values[7];
	return (temp/340.0 + 36.53);	
}

float get_Wx()
{
	__s16 xgyro = (((int16_t)values[8])<<8) | values[9]; 
	return xgyro/131.0;
}
float get_Wy()
{
	__s16 ygyro = (((int16_t)values[10])<<8) | values[11]; 
	return ygyro/131.0;
}

float get_Wz()
{
	__s16 zgyro = (((int16_t)values[12])<<8) | values[13]; 
	return zgyro/131.0;
}

void clear_gy521()
{
	close(file);
}

