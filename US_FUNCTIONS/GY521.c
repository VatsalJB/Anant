#include <linux/i2c-dev-user.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>

#define ADDR 0x68

static int file;
static __s32 res;
static __u8 reg;

//set flag to 1 for making it sleep and 0 for awake.
void set_sleep_gy521(int flag)
{
	if(flag==0)
	{
		reg = 0x6B;
		uint8_t val8 = 0x01;     //write 0x00 if you want to set the internal 8MHz oscillator as CLK                                              
		res = i2c_smbus_write_byte_data(file, reg, val8);
		if(res<0) 
			perror("Failed to wake it up\n");
		/*else 
			printf("Device is awake\n");
		*/
	}
	else if(flag == 1)    //set it to sleep
	{
		reg = 0x6B;
		uint8_t val8 = 0x41;   //write 0x40 if you want to set the internal 8MHz oscillator as CLK
		res = i2c_smbus_write_byte_data(file, reg, val8);
		if(res<0) 
			perror("Failed to go to sleep\n");
		/*else 
			printf("In sleep mode\n");
		*/
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

float get_Ax()
{
	int16_t xout = i2c_smbus_read_byte_data(file, 0x3B); int16_t xout1 = i2c_smbus_read_byte_data(file, 0x3C);
	xout = (xout<<8) | xout1;
	return xout/16384.0*9.8;
}
float get_Ay()
{	
	int16_t yout = i2c_smbus_read_byte_data(file, 0x3D); int16_t yout1 = i2c_smbus_read_byte_data(file, 0x3E);
	yout = (yout<<8) | yout1;
	return yout/16384.0*9.8;
}
float get_Az()
{
	int16_t zout = i2c_smbus_read_byte_data(file, 0x3F); int16_t zout1 = i2c_smbus_read_byte_data(file, 0x40);
	zout = (zout<<8) | zout1;
	return zout/16384.0*9.8;
}
float get_temp()
{
	__s16 temp = i2c_smbus_read_byte_data(file, 0x41); __s16 temp1 = i2c_smbus_read_byte_data(file, 0x42);
	temp = (temp<<8) | temp1;
	return (temp/340.0 + 36.53);
}
float get_Wx()
{
	__s16 gyro_x = i2c_smbus_read_byte_data(file, 0x43); __s16 gyro_x1 = i2c_smbus_read_byte_data(file, 0x44);
	gyro_x = (gyro_x<<8) | gyro_x1;	
	return gyro_x/131.0;
}
float get_Wy()
{
	__s16 gyro_y = i2c_smbus_read_byte_data(file, 0x45); __s16 gyro_y1 = i2c_smbus_read_byte_data(file, 0x46);
	gyro_y = (gyro_y<<8) | gyro_y1;
	return gyro_y/131.0;
}
float get_Wz()
{
	__s16 gyro_z = i2c_smbus_read_byte_data(file, 0x47); __s16 gyro_z1 = i2c_smbus_read_byte_data(file, 0x48);
	gyro_z = (gyro_z<<8) | gyro_z1;
	return gyro_z/131.0;
}
