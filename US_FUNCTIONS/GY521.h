#ifndef GY521.h
#define GY521.h

void init_gy521();
void set_sleep_gy521(int);
float get_Ax();
float get_Ay();
float get_Az();
float get_temp_gy521();
float get_Wx();
float get_Wy();
float get_Wz();
void clear_gy521();

#endif
