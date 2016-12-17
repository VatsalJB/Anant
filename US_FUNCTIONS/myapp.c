//#include <linux/i2c.h>
/*
*	This is the actual code.
*
*/
#include <linux/i2c-dev-user.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define ADDRESS 0x1e

void self_test(int file)
{
    //register addresses
	__u8 mode_reg = 0x02;
	__u8 gain_reg = 0x01;
	__u8 data_X_H = 0x03;
	__u8 data_X_L = 0x04;
	__u8 data_Y_H = 0x07;
	__u8 data_Y_L = 0x08;
	__u8 data_Z_H = 0x05;
	__u8 data_Z_L = 0x06;

	//read all 6 registers individually
	int16_t b_X_H = i2c_smbus_read_byte_data(file, data_X_H);
	int16_t b_X_L = i2c_smbus_read_byte_data(file, data_X_L);
	int16_t b_Y_H = i2c_smbus_read_byte_data(file, data_Y_H);
	int16_t b_Y_L = i2c_smbus_read_byte_data(file, data_Y_L);
	int16_t b_Z_H = i2c_smbus_read_byte_data(file, data_Z_H);
	int16_t b_Z_L = i2c_smbus_read_byte_data(file, data_Z_L);

	//concatenate the upper and lower bits
	int16_t b_X = (b_X_H<<8) | b_X_L;
	int16_t b_Y = (b_Y_H<<8) | b_Y_L;
	int16_t b_Z = (b_Z_H<<8) | b_Z_L;

	printf("raw values %hd %hd %hd\n", b_X, b_Y, b_Z);
	printf("actual values %f %f %f\n", b_X*2.56, b_Y*2.56, b_Z*2.56);

}

void measure(int file)
{
    //register addresses
	__u8 mode_reg = 0x02;
	__u8 gain_reg = 0x01;
	__u8 data_X_H = 0x03;
	__u8 data_X_L = 0x04;
	__u8 data_Y_H = 0x07;
	__u8 data_Y_L = 0x08;
	__u8 data_Z_H = 0x05;
	__u8 data_Z_L = 0x06;

	//read all 6 registers individually
	int16_t b_X_H = i2c_smbus_read_byte_data(file, data_X_H);
	int16_t b_X_L = i2c_smbus_read_byte_data(file, data_X_L);
	int16_t b_Y_H = i2c_smbus_read_byte_data(file, data_Y_H);
	int16_t b_Y_L = i2c_smbus_read_byte_data(file, data_Y_L);
	int16_t b_Z_H = i2c_smbus_read_byte_data(file, data_Z_H);
	int16_t b_Z_L = i2c_smbus_read_byte_data(file, data_Z_L);

	//concatenate the upper and lower bits
	int16_t b_X = (b_X_H<<8) | b_X_L;
	int16_t b_Y = (b_Y_H<<8) | b_Y_L;
	int16_t b_Z = (b_Z_H<<8) | b_Z_L;

	printf("raw values %hd %hd %hd\n", b_X, b_Y, b_Z);
	printf("actual values %f %f %f\n", b_X*0.92, b_Y*0.92, b_Z*0.92);
}

int main()
{
    /* initialization of userspace function*/
    int adapter_number = 0;     //check this.
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", adapter_number);
    int file;
    file = open(filename, O_RDWR);
    if(file<0)
    {
        perror("File not opened");
    }
    if(ioctl(file, I2C_SLAVE, ADDRESS)<0)
    {
        perror("ioctl could not open file");
    }

	if(i2c_smbus_write_byte_data(file, 0x02, 0x00)<0)	//continuous measurement mode.
		perror("Mode change failed");
	if(i2c_smbus_write_byte_data(file, 0x00, 0x71)<0)	//set config register A for positive bias, 8-average, 15Hz self test.
		perror("config error");
	if(i2c_smbus_write_byte_data(file, 0x01, 0xA0)<0)	//Set Gain to 5.
		perror("Gain change failed");

    while(1)
    {
    	printf("Enter 1 for testing or 2 for measurement\n");
    	int input;
    	scanf("%d", &input);
    	if(input==1)
    		self_test(file);
    	else
    		measure(file);
    	/*if(i2c_smbus_write_byte_data(file, 0x02, 0x02)<0)	//places the device on idle mode.
    		perror("Idle failed");*/
    }
}
