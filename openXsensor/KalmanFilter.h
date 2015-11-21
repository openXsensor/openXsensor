#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_


class KalmanFilter {

public :

KalmanFilter();
//void Configure( float zInitial ); // is not used anymore because all data are initilalise in KalmanFilter.h or with #define
void Update(float z, float a,  float* pZ, float* pV);

private :

// State being tracked
	float z_;  // position
	float v_;  // velocity
	float aBias_;  // acceleration

// 3x3 State Covariance matrix
	float Pzz_ = 1.0f;
	float Pzv_;
	float Pza_;
	float Pvz_;
	float Pvv_ = 1.0f;
	float Pva_;
	float Paz_;
	float Pav_;
	float Paa_ = 100000.0f;

// those float have been replaced by #define to save flash memory
//  float zAccelBiasVariance_; // assumed fixed.
//	float zAccelVariance_;  // dynamic acceleration variance
//	float zVariance_; //  z measurement noise variance fixed

};

#endif
