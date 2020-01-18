#ifndef OXS_HMC5883_h
#define OXS_HMC5883_h


#include "Arduino.h"
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"
#include "I2C.h"



#define HMC5883L_ADDRESS              (0x1E)
#define HMC5883L_REG_CONFIG_A         (0x00)
#define HMC5883L_REG_CONFIG_B         (0x01)
#define HMC5883L_REG_MODE             (0x02)
#define HMC5883L_REG_OUT_X_M          (0x03)
#define HMC5883L_REG_OUT_X_L          (0x04)
#define HMC5883L_REG_OUT_Z_M          (0x05)
#define HMC5883L_REG_OUT_Z_L          (0x06)
#define HMC5883L_REG_OUT_Y_M          (0x07)
#define HMC5883L_REG_OUT_Y_L          (0x08)
#define HMC5883L_REG_STATUS           (0x09)
#define HMC5883L_REG_IDENT_A          (0x0A)
#define HMC5883L_REG_IDENT_B          (0x0B)
#define HMC5883L_REG_IDENT_C          (0x0C)

// number of samples for averages
#define HMC5883L_SAMPLES_8     0b11
#define HMC5883L_SAMPLES_4     0b10
#define HMC5883L_SAMPLES_2     0b01
#define HMC5883L_SAMPLES_1     0b00


// Data rate
#define HMC5883L_DATARATE_75HZ       0b110
#define HMC5883L_DATARATE_30HZ       0b101
#define HMC5883L_DATARATE_15HZ       0b100
#define HMC5883L_DATARATE_7_5HZ      0b011
#define HMC5883L_DATARATE_3HZ        0b010
#define HMC5883L_DATARATE_1_5HZ      0b001
#define HMC5883L_DATARATE_0_75_HZ    0b000



// Range
#define HMC5883L_RANGE_8_1GA     0b111
#define HMC5883L_RANGE_5_6GA     0b110
#define HMC5883L_RANGE_4_7GA     0b101
#define HMC5883L_RANGE_4GA       0b100
#define HMC5883L_RANGE_2_5GA     0b011
#define HMC5883L_RANGE_1_9GA     0b010
#define HMC5883L_RANGE_1_3GA     0b001
#define HMC5883L_RANGE_0_88GA    0b000


// mode
#define HMC5883L_IDLE           0b10
#define HMC5883L_SINGLE         0b01
#define HMC5883L_CONTINOUS      0b00

boolean setup_hmc5883() ;

boolean read_hmc5883() ;   // return a flag that is true when a new heading is available



#endif // OXS_HMC5883_h




