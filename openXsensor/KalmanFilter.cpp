#include "Arduino.h"
// #include "common.h"
#include <math.h>
#include "KalmanFilter.h"

// Tracks the position z and velocity v of an object moving in a straight line,
// (here assumed to be vertical) that is perturbed by random accelerations.
// sensor measurement of z is assumed to have constant measurement noise 
// variance zVariance,
// This can be calculated offline for the specific sensor, and is supplied 
// as an initialization parameter.


KalmanFilter::KalmanFilter() { // constructor initialize 3 parameters
  Pzz_ = Pvv_ = 1.0f ;
  Paa_ = 100000.0f;
}

/* // model of a function to extract data from Kalman filter
float KalmanFilter::getPaa_() {
  return Paa_ ;
}
*/
/*
//void KalmanFilter::Configure(float zVariance, float zAccelVariance, float zAccelBiasVariance, float zInitial, float vInitial, float aBiasInitial) {
void KalmanFilter::Configure( float zInitial) { // is not used anymore because all data are initilalise in KalmanFilter.h or with #define
//	zAccelVariance_ = zAccelVariance;
//  zAccelBiasVariance_ = zAccelBiasVariance;
//	zVariance_ = zVariance;

//	z_ = zInitial;
//	v_ =  0.0f; // is automatically initialise to 0 ; this save some flash memory
//  aBias_ = 0.0f;

//	Pzz_ = 1.0f;
//	Pzv_ = 0.0f;
//	Pza_ = 0.0f;
	
//	Pvz_ = 0.0f;
//	Pvv_ = 1.0f;
//	Pva_ = 0.0f;
	
//	Paz_ = 0.0f;
//	Pav_ = 0.0;
//	Paa_ = 100000.0f;
 
	}
*/

// Updates state given a sensor measurement of z, acceleration a, 
// and the time in seconds dt since the last measurement. 
// 19uS on Navspark @81.84MHz	
//void KalmanFilter::Update(float z, float a, float dt, float* pZ, float* pV) {
void KalmanFilter::Update(float z, float a,  float* pZ, float* pV) {

#define Z_VARIANCE        50.0f // initially 500
#define ZACCEL_VARIANCE     0.1f  // initially 1
#define ZACCELBIAS_VARIANCE 1.0f

#define DT 0.02f                          // time interval = 20 msec
#define DT2DIV2  (DT * DT / 2.0f)
#define DT3DIV2  (DT2DIV2 * DT)
#define DT4DIV4  (DT2DIV2 * DT2DIV2)

	// Predict state
    float accel = a - aBias_;
	  v_ += accel * DT;
	  z_ += v_ * DT;

//test by ms leaving zAccelVariance_ = 1
/*
    zAccelVariance_ = fabs(accel)/50.0f;
    if (zAccelVariance_ > 50.0f)  {
        zAccelVariance_ = 50.0f;
    } else if (zAccelVariance_ < 1.0f) {
        zAccelVariance_ = 1.0f ;
    }
//    CLAMP(zAccelVariance_, 1.0f, 50.0f);
*/
    // Predict State Covariance matrix
	float t00;
	float t01,t02;
    float t10,t11,t12;
    float t20,t21,t22;
	
//    float dt2div2 = dt*dt/2.0f;
//    float dt3div2 = dt2div2*dt;
//    float dt4div4 = dt2div2*dt2div2;
	
	t00 = Pzz_ + DT*Pvz_ - DT2DIV2*Paz_;
	t01 = Pzv_ + DT*Pvv_ - DT2DIV2*Pav_;
	t02 = Pza_ + DT*Pva_ - DT2DIV2*Paa_;

	t10 = Pvz_ - DT*Paz_;
	t11 = Pvv_ - DT*Pav_;
	t12 = Pva_ - DT*Paa_;

	t20 = Paz_;
	t21 = Pav_;
	t22 = Paa_;
	
	Pzz_ = t00 + DT*t01 - DT2DIV2*t02;
//  Pzz_ +=  dt*Pvz_ - dt2div2*Paz_ + dt*t01 - dt2div2*t02;
	Pzv_ = t01 - DT*t02;
	Pza_ = t02;
	
	Pvz_ = t10 + DT*t11 - DT2DIV2*t12;
//  Pvz_ +=  - dt*Paz_ + dt*t11 - dt2div2*t12;
	Pvv_ = t11 - DT*t12;
	Pva_ = t12;
	
	Paz_ = t20 + DT*t21 - DT2DIV2*t22;
//  Paz_ +=  dt*t21 - dt2div2*t22;
	Pav_ = t21 - DT*t22;
	Paa_ = t22;

    Pzz_ += DT4DIV4*ZACCEL_VARIANCE;
    Pzv_ += DT3DIV2*ZACCEL_VARIANCE;

    Pvz_ += DT3DIV2*ZACCEL_VARIANCE;
    Pvv_ += DT*DT*ZACCEL_VARIANCE;

    Paa_ += ZACCELBIAS_VARIANCE;

	// Error
	float innov = z - z_; 
	float sInv = 1.0f / (Pzz_ + Z_VARIANCE);  

    // Kalman gains
	float kz = Pzz_ * sInv;  
	float kv = Pvz_ * sInv;
	float ka = Paz_ * sInv;

	// Update state 
	z_ += kz * innov;
	v_ += kv * innov;
	aBias_ += ka * innov;
	
	*pZ = z_;
	*pV = v_;

	// Update state covariance matrix
	Pzz_ -= kz * Pzz_;
	Pzv_ -= kz * Pzv_;
	Pza_ -= kz * Pza_;
	
	Pvz_ -= kv * Pzz_;
	Pvv_ -= kv * Pzv_;
	Pva_ -= kv * Pza_;
	
	Paz_ -= ka * Pzz_;
	Pav_ -= ka * Pzv_;
	Paa_ -= ka * Pza_;
	}
