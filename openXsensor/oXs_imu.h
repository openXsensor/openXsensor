#ifndef OXS_IMU_h
#define OXS_IMU_h


#include "Arduino.h"
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "I2C.h"
#include "helper_3dmath.h"


boolean initialize_mpu() ;
void getEuler(float *data, Quaternion *q) ;
void getGravity(struct FloatVectorStruct *v, Quaternion *q) ;
void dmpGetYawPitchRoll(float *data, Quaternion *q, struct FloatVectorStruct *gravity) ;
void enable_mpu() ;
void disable_mpu() ;
void setupImu() ;
void gyro_data_ready_cb(void) ;
unsigned short inv_row_2_scale(const signed char *row) ;
unsigned short inv_orientation_matrix_to_scalar( const signed char *mtx) ;
boolean run_mpu_self_test(boolean& gyro_ok, boolean& accel_ok) ;
bool read6050 () ;   // return a flag that is true when a new acceleration is available
void dump_all() ;
#endif // OXS_IMU_h




