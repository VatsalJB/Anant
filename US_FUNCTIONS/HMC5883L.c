#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define ADDRESS 0x1e

static int file;		//use static keyword to ensure that the scope of this variable is limited to this file.
static float factor;
static __u8 buffer[6];

//register addresses
__u8 config_reg_A = 0x00;
__u8 mode_reg = 0x02;
__u8 gain_reg = 0x01;
__u8 data_X_H = 0x03;
__u8 data_X_L = 0x04;
__u8 data_Y_H = 0x07;
__u8 data_Y_L = 0x08;
__u8 data_Z_H = 0x05;
__u8 data_Z_L = 0x06;



/**	
*	The value of mode must be according to the following table:
*	Value 		Mode
*	0			Continuous
*	1			Single	(Default)
*	2			Idle
*	3			Idle
*
*	After any mode change care must be taken to set it back to continuous mode before reading any values.
**/
void set_magnetometer_mode(int mode)
{
	__u8 value = 0x00;
	value |= mode;
	if(i2c_smbus_write_byte_data(file, mode_reg, value)<0)
		perror("Failed to change magnetometer mode");
}

void get_B()
{
	if(i2c_smbus_read_i2c_block_data(file, data_X_H, 6, buffer)<0)
		perror("Failed to read the block");
}

//[IMPORTANT] Note that	 the following 3 functions will return the field values in milli gauss by reading them from the buffer. So call get_Bx() first!
float get_Bx()
{	
	get_B();
	int16_t temp;
	//concatenate the upper and lower bits
	temp = buffer[0];
	int16_t b_X = (temp<<8) | buffer[1];
	return (float)b_X*factor;
}

float get_By()
{
	int16_t temp;
	//concatenate the upper and lower bits
	temp = buffer[4];
	int16_t b_Y = (temp<<8) | buffer[5];
	return (float)b_Y*factor;
}

float get_Bz()
{
	int16_t temp;
	//concatenate the upper and lower bits
	temp = buffer[2];
	int16_t b_Z = (temp<<8) | buffer[3];
	return (float)b_Z*factor;
}

//Initializes the file used by the userspace calls. [IMPORTANT] Must be run before any other function is called for this device!. This needs to be called only once for each process.
void init_magnetometer()
{
    int adapter_number = 0;     //check this.
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", adapter_number);
    file = open(filename, O_RDWR);
    if(file<0)
    {
        perror("File not opened");
    }
    if(ioctl(file, I2C_SLAVE, ADDRESS)<0)
    {
        perror("ioctl could not open file");
    }
    factor = 0.92;
    set_magnetometer_mode(0);
}

void clear_magnetometer()
{
	close(file);
}

/**	
*	The value of freq must be according to the following table:
*	Value 		Rate (Hz)
*	0			0.75
*	1			1.5
*	2			3
*	3			7.5
*	4			15		(Default)
*	5			30
*	6			75
**/
void set_magnetometer_frequency(int freq)
{
	__u8 value = 0x00;
	value |= freq<<2;
	if(i2c_smbus_write_byte_data(file, config_reg_A, value)<0)
		perror("Failed to change data rate");
}


/**	
*	The value of gain must be according to the following table:
*	Value 		Field Range (+/- Gauss)
*	0			0.88
*	1			1.3		(Default)
*	2			1.9
*	3			2.5
*	4			4.0
*	5			4.7
*	6			5.6
*	7			8.1
*	
*	This function will also set the value of the factor to be multiplied to the raw data.
**/
void set_magnetometer_gain(int gain)
{
	__u8 value = 0x00;
	value |= gain<<5;
	if(i2c_smbus_write_byte_data(file, gain_reg, value)<0)
		perror("Failed to change magnetometer gain");
	else
	{
		switch(gain)
		{
			case 0: factor = 0.73; break;
			case 1: factor = 0.92; break;
			case 2: factor = 1.22; break;
			case 3: factor = 1.52; break;
			case 4: factor = 2.27; break;
			case 5: factor = 2.56; break;
			case 6: factor = 3.03; break;
			case 7: factor = 4.35; break;
		}
	}
}