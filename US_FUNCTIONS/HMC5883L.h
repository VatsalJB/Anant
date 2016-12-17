#ifndef H5883L.h
#define H5883L.h


float get_Bx();
float get_By();
float get_Bz();
void init_magnetometer();
void set_magnetometer_freq(int);
void set_magnetometer_gain(int);
void set_magnetometer_mode(int);