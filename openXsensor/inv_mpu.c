/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 *
 *
 *  @addtogroup  DRIVERS Sensor Driver Layer
 *  @brief       Hardware drivers to communicate with sensors via I2C.
 *
 *  @{
 *      @file       inv_mpu.c
 *      @brief      An I2C-based driver for Invensense gyroscopes.
 *      @details    This driver currently works for the following devices:
 *                  MPU6050 (support for other devices have been deleted in order to save flash memory)
 */
 
#define EMPL_TARGET_ATMEGA328 // initially the library supported several mcu, some code has been removed; this define is kept for saferty
#define MPU6050 // initially the library supported several mpu, some code has been removed; this define is kept for saferty

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include <string.h>
//#include <math.h>
#include "inv_mpu.h"

//#include "I2C.h"
/* The following functions must be defined for this platform:
 * i2c_write(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char const *data)
 * i2c_read(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char *data)
 * reg_int_cb(void (*cb)(void), unsigned char port, unsigned char pin)
 * labs(long x)
 * fabsf(float x)
 */
#include "arduino_shim.h"

//#define DEBUG_READ_FIFO    // this function has been added to create a function get_fifo_count_debug to print it in oXs_imu if required
//#define i2c_write(a,b,c,d) shim_i2c_write(a,b,c,d)
//#define i2c_write(a,b,c,d)  I2c.write( (uint8_t) a , (uint8_t) b ,  (uint8_t) c  , (uint8_t *) d ) ; // return 0 on success  
#define i2c_writeByte(b,d) shim_i2c_writeByte(b,d)
#define i2c_read(a,b,c,d) shim_i2c_read(a,b,c,d)
#define i2c_writeBlock(b,c,d) shim_i2c_writeBlock(b,c,d)

/*
static inline int reg_int_cb(struct int_param_s *int_param)
{
	attachInterrupt(int_param->pin, int_param->cb, RISING);
	return 0;
}
*/

#define fabs(x)     (((x)>0)?(x):-(x))


struct chip_cfg_s {   // keep trace of mpu6050 config. 
    /* Matches gyro_cfg >> 3 & 0x03 */
//    unsigned char gyro_fsr;
    /* Matches accel_cfg >> 3 & 0x03 */
//    unsigned char accel_fsr;
    /* Enabled sensors. Uses same masks as fifo_en, NOT pwr_mgmt_2. */
//    unsigned char sensors;
    /* Matches config register. */
//    unsigned char lpf;
//    unsigned char clk_src;
    /* Sample rate, NOT rate divider. */
//    unsigned short sample_rate;
    /* Matches fifo_en register. */
//    unsigned char fifo_enable;
    /* Matches int enable register. */
//    unsigned char int_enable;
    /* 1 if devices on auxiliary I2C bus appear on the primary. */
//    unsigned char bypass_mode;
    /* 1 if half-sensitivity.
     * NOTE: This doesn't belong here, but everything else in hw_s is const,
     * and this allows us to save some precious RAM.
     */
    unsigned char accel_half;
    /* 1 if device in low-power accel-only mode. */
//    unsigned char lp_accel_mode;
    /* 1 if interrupts are only triggered on motion events. */
//    unsigned char int_motion_only;
//    struct motion_int_cache_s cache;
    /* 1 for active low interrupts. */
//    unsigned char active_low_int;
    /* 1 for latched interrupts. */
//    unsigned char latched_int;
    /* 1 if DMP is enabled. */
//    unsigned char dmp_on;
    /* Ensures that DMP will only be loaded once. */
//    unsigned char dmp_loaded;
    /* Sampling rate used when DMP is enabled. */
//    unsigned short dmp_sample_rate;
};


/* Gyro driver state variables. */
//struct gyro_state_s {
//    struct chip_cfg_s chip_cfg;
//};


    struct chip_cfg_s chip_cfg;



/* Filter configurations. */
enum lpf_e {
    INV_FILTER_256HZ_NOLPF2 = 0,
    INV_FILTER_188HZ,
    INV_FILTER_98HZ,
    INV_FILTER_42HZ,
    INV_FILTER_20HZ,
    INV_FILTER_10HZ,
    INV_FILTER_5HZ,
    INV_FILTER_2100HZ_NOLPF,
    NUM_FILTER
};

/* Full scale ranges. */
enum gyro_fsr_e {
    INV_FSR_250DPS = 0,
    INV_FSR_500DPS,
    INV_FSR_1000DPS,
    INV_FSR_2000DPS,
    NUM_GYRO_FSR
};

/* Full scale ranges. */
enum accel_fsr_e {
    INV_FSR_2G = 0,
    INV_FSR_4G,
    INV_FSR_8G,
    INV_FSR_16G,
    NUM_ACCEL_FSR
};

/* Clock sources. */
enum clock_sel_e {
    INV_CLK_INTERNAL = 0,
    INV_CLK_PLL,
    NUM_CLK
};

/* Low-power accel wakeup rates. */ 
enum lp_accel_rate_e {
    INV_LPA_1_25HZ,
    INV_LPA_5HZ,
    INV_LPA_20HZ,
    INV_LPA_40HZ
};

#define BIT_I2C_MST_VDDIO   (0x80)
#define BIT_FIFO_EN         (0x40)
#define BIT_DMP_EN          (0x80)
#define BIT_FIFO_RST        (0x04)
#define BIT_DMP_RST         (0x08)
#define BIT_FIFO_OVERFLOW   (0x10)
#define BIT_DATA_RDY_EN     (0x01)
#define BIT_DMP_INT_EN      (0x02)
#define BIT_MOT_INT_EN      (0x40)
#define BITS_FSR            (0x18)
#define BITS_LPF            (0x07)
#define BITS_HPF            (0x07)
#define BITS_CLK            (0x07)
#define BIT_FIFO_SIZE_1024  (0x40)
#define BIT_FIFO_SIZE_2048  (0x80)
#define BIT_FIFO_SIZE_4096  (0xC0)
#define BIT_RESET           (0x80)
#define BIT_SLEEP           (0x40)
#define BIT_S0_DELAY_EN     (0x01)
#define BIT_S2_DELAY_EN     (0x04)
#define BITS_SLAVE_LENGTH   (0x0F)
#define BIT_SLAVE_BYTE_SW   (0x40)
#define BIT_SLAVE_GROUP     (0x10)
#define BIT_SLAVE_EN        (0x80)
#define BIT_I2C_READ        (0x80)
#define BITS_I2C_MASTER_DLY (0x1F)
#define BIT_AUX_IF_EN       (0x20)
#define BIT_ACTL            (0x80)
#define BIT_LATCH_EN        (0x20)
#define BIT_ANY_RD_CLR      (0x10)
#define BIT_BYPASS_EN       (0x02)
#define BITS_WOM_EN         (0xC0)
#define BIT_LPA_CYCLE       (0x20)
#define BIT_STBY_XA         (0x20)
#define BIT_STBY_YA         (0x10)
#define BIT_STBY_ZA         (0x08)
#define BIT_STBY_XG         (0x04)
#define BIT_STBY_YG         (0x02)
#define BIT_STBY_ZG         (0x01)
#define BIT_STBY_XYZA       (BIT_STBY_XA | BIT_STBY_YA | BIT_STBY_ZA)
#define BIT_STBY_XYZG       (BIT_STBY_XG | BIT_STBY_YG | BIT_STBY_ZG)


/* Hardware registers needed by driver. */
#define GYRO_REG_who_am_i         0x75
#define GYRO_REG_rate_div         0x19
#define GYRO_REG_lpf              0x1A
#define GYRO_REG_prod_id          0x0C
#define GYRO_REG_user_ctrl        0x6A
#define GYRO_REG_fifo_en          0x23
#define GYRO_REG_gyro_cfg         0x1B
#define GYRO_REG_accel_cfg        0x1C
#define GYRO_REG_motion_thr       0x1F
#define GYRO_REG_motion_dur       0x20
#define GYRO_REG_fifo_count_h     0x72
#define GYRO_REG_fifo_r_w         0x74
#define GYRO_REG_raw_gyro         0x43
#define GYRO_REG_raw_accel        0x3B
#define GYRO_REG_temp             0x41
#define GYRO_REG_int_enable       0x38
#define GYRO_REG_dmp_int_status   0x39
#define GYRO_REG_int_status       0x3A
#define GYRO_REG_pwr_mgmt_1       0x6B
#define GYRO_REG_pwr_mgmt_2       0x6C
#define GYRO_REG_int_pin_cfg      0x37
#define GYRO_REG_mem_r_w          0x6F
#define GYRO_REG_accel_offs       0x06
#define GYRO_REG_i2c_mst          0x24
#define GYRO_REG_bank_sel         0x6D
#define GYRO_REG_mem_start_addr   0x6E
#define GYRO_REG_prgm_start_h     0x70

// Information specific to a particular device. //
#define INV6050_addr              0x68
#define INV6050_max_fifo          1024
#define INV6050_num_reg           118
#define INV6050_temp_sens         340
#define INV6050_temp_offset       -521
#define INV6050_bank_size         256

// Information for self test I think //
#define TEST_gyro_sens        32768/250
#define TEST_accel_sens       32768/16
#define TEST_reg_rate_div     0    /* 1kHz. */
#define TEST_reg_lpf          1    /* 188Hz. */
#define TEST_reg_gyro_fsr     0    /* 250dps. */
#define TEST_reg_accel_fsr    0x18 /* 16g. */
#define TEST_wait_ms          50
#define TEST_packet_thresh    5    /* 5% */
#define TEST_min_dps          10.f
#define TEST_max_dps          105.f
#define TEST_max_gyro_var     0.14f
#define TEST_min_g            0.3f
#define TEST_max_g            0.95f
#define TEST_max_accel_var    0.14f


#define MAX_PACKET_LENGTH (12)


//static struct gyro_state_s st ; // store the status and config of MPU6050


/**
 *  @brief      Enable/disable data ready interrupt.                                // saving : if dmp is always used this code could be simplified
 *  If the DMP is on, the DMP interrupt is enabled. Otherwise, the data ready
 *  interrupt is used.
 *  @param[in]  enable      1 to enable interrupt.
 *  @return     0 if successful.
 */
/*
static int set_int_enable(unsigned char enable)
{
    unsigned char tmp;
    if (chip_cfg.dmp_on) {
        if (enable)
            tmp = BIT_DMP_INT_EN;
        else
            tmp = 0x00;
    //        if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, &tmp))
          if (i2c_writeByte( GYRO_REG_int_enable,  tmp))
            return -1;
        chip_cfg.int_enable = tmp;
    } else {
            if (!chip_cfg.sensors)
                return -1;
        if (enable && chip_cfg.int_enable)
            return 0;
        if (enable)
            tmp = BIT_DATA_RDY_EN;
        else
            tmp = 0x00;
    //        if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, &tmp))
          if (i2c_writeByte(  GYRO_REG_int_enable,  tmp))
            return -1;
        chip_cfg.int_enable = tmp;
    }
    return 0;
}
*/
/*
static int set_int_enablexx(unsigned char enable)
{
       return  i2c_writeByte( GYRO_REG_int_enable,  BIT_DMP_INT_EN ) ;     // it is 0x02 ; it is not documented in the datashee
}
*/

/**
 *  @brief      Register dump for testing.
 *  @return     0 if successful.
 */
/*                                            // this does not work if all registers are defined with #define instead of a structure; so this code is commented
int mpu_reg_dump(void)
{
    unsigned char ii;
    unsigned char data;

    for (ii = 0; ii < INV6050_num_reg; ii++) {
        if (ii == GYRO_REG_fifo_r_w || ii == GYRO_REG_mem_r_w)
            continue;
        if (i2c_read( INV6050_addr, ii, 1, &data))
            return -1;
        log_i("%#5x: %#5x\r\n", ii, data);
    }
    return 0;
}
*/

/**
 *  @brief      Read from a single register.                                    // saving : this code could be reduced if we remove the checks
 *  NOTE: The memory and FIFO read/write registers cannot be accessed.          // this code is not used by oXs
 *  @param[in]  reg     Register address.
 *  @param[out] data    Register data.
 *  @return     0 if successful.
 */
int mpu_read_reg(unsigned char reg, unsigned char *data)
{
    if (reg == GYRO_REG_fifo_r_w || reg == GYRO_REG_mem_r_w)
        return -1;
    if (reg >= INV6050_num_reg)
        return -1;
    return i2c_read( INV6050_addr, reg, 1, data);
}

/*
void mpu_force_reset()                                         // this code is currently not used; so it does not take flash memory
{
    unsigned char data[6];
	data[0] = BIT_RESET;
	int i;
    for ( i = 0; i < 100; i++ )
	{
//		if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 1, data) == 0) 
    if (i2c_writeByte( GYRO_REG_pwr_mgmt_1,  data[0] ) == 0) 
		{
			delay(100);

			                      // Wake up chip. 
			data[0] = 0x00;
//			if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 1, data)==0)
      if (i2c_writeBlock( GYRO_REG_pwr_mgmt_1, 1, data)==0)
			{
				break;
			}
			else
			{
				delay(10);
			}
		}
		else
		{
			delay(10);
		}
	}
}
*/

/**
 *  @brief      Initialize hardware.
 *  Initial configuration:\n
 *  Gyro FSR: +/- 2000DPS\n
 *  Accel FSR +/- 2G\n
 *  DLPF: 42Hz\n
 *  FIFO rate: 50Hz\n
 *  Clock source: Gyro PLL\n
 *  FIFO: Disabled.\n
 *  Data ready interrupt: Disabled, active low, unlatched.
 *  @param[in]  int_param   Platform-specific parameters to interrupt API.
 *  @return     0 if successful.
 */
int mpu_init()
{
    unsigned char data[6], rev;
    // Reset device. 
    if (i2c_writeByte(GYRO_REG_pwr_mgmt_1, (uint8_t) BIT_RESET ))  // reset the mpu6050
        return -1;
    delay(100);                                                    // wait that reset is done

    // Wake up chip.                                               // filling 0 is really required (tested) to get good acceleration; this is strange because it is normally cleared automatically when reset is done
    if (i2c_writeByte( GYRO_REG_pwr_mgmt_1, (uint8_t) 0x00 ))      
        return -1;
                                                 
    /* Check product revision. */                     // saving : this code could omitted if accel_half is not used. To check ; My mpu return 0
    if (i2c_read( INV6050_addr, GYRO_REG_accel_offs, 6, data))
        return -1;
    rev = ((data[5] & 0x01) << 2) | ((data[3] & 0x01) << 1) | (data[1] & 0x01);
    if (rev) {
        /* Congrats, these parts are better. */
        if (rev == 1)             chip_cfg.accel_half = 1;
        else if ((rev == 2) || (rev ==4))        chip_cfg.accel_half = 0;
        else {
            log_e("Unsupported software product rev %d.\n", rev);
            return -1;
        }
    } else {
        if (i2c_read( INV6050_addr, GYRO_REG_prod_id, 1, data)) 
            return -1;
        rev = data[0] & 0x0F;
        if (!rev) {
            log_e("Product ID read as 0 indicates device is either "
                "incompatible or an MPU3050.\n");
            return -1;
        } else if (rev == 4) {
            log_i("Half sensitivity part found.\n");
            chip_cfg.accel_half = 1;
        } else
            chip_cfg.accel_half = 0;
    }

    /* Set to invalid values to ensure no I2C writes are skipped. */
///    chip_cfg.sensors = 0xFF;
//    chip_cfg.gyro_fsr = 0xFF;            // can be omitted because filled by mpu_set_gyro_fsr()
//    chip_cfg.accel_fsr = 0xFF;           // can be omitted because filled by mpu_set_accel_fsr()
//    chip_cfg.lpf = 0xFF;                  // can be omitted because filled by mpu_set_lpf()
//    chip_cfg.sample_rate = 0xFFFF;        // can be omitted because filled by mpu_set_sample_rate()
//    chip_cfg.fifo_enable = 0xFF;
//    chip_cfg.bypass_mode = 0xFF;
    /* mpu_set_sensors always preserves this setting. */
//    chip_cfg.clk_src = INV_CLK_PLL;
    /* Handled in next call to mpu_set_bypass. */
//    chip_cfg.active_low_int = 1;
//    chip_cfg.latched_int = 0;
//    chip_cfg.int_motion_only = 0;
//    chip_cfg.lp_accel_mode = 0;
//    memset(&chip_cfg.cache, 0, sizeof(chip_cfg.cache));
//    chip_cfg.dmp_on = 0;
//    chip_cfg.dmp_loaded = 0;
//    chip_cfg.dmp_sample_rate = 0;

 //   if (mpu_set_gyro_fsr(2000))
 //       return -1;
    if (i2c_writeByte(  GYRO_REG_gyro_cfg, (INV_FSR_2000DPS << 3 ))) return -1;  // set gyro_fsr to 2000 degree/sec 
    
//    if (mpu_set_accel_fsr(2))  // 2 = 2g at full scale
//        return -1;
    if (i2c_writeByte( GYRO_REG_accel_cfg, (INV_FSR_2G << 3) )) return -1;   // set accel_fsr to 2g
      
//    if (mpu_set_lpf(42))                    // it is fixed automatically to 200/2 in another function (hardcode)
//        return -1;
//    if (mpu_set_sample_rate(50))            // to be checked if it can be ommited ; it is overwritten when DMP is initialized
//        return -1;
//    if (mpu_configure_fifo(0))            // not needed ; it seems that fifo is configured by DMP feature
//        return -1;


//        reg_int_cb(int_param);               // set the call back function and the interrupt number (0) used by 6050

    /* Already disabled by setup_compass. */
//    if (mpu_set_bypass(0))               // to be checked if it can be ommited
//        return -1;

//    mpu_set_sensors(0); // set mpu in standby ; could be replaced by next lines if really requested.
      if( i2c_writeByte( GYRO_REG_pwr_mgmt_1, BIT_SLEEP )) return -1 ; // set mpu in sleep mode
      if( i2c_writeByte( GYRO_REG_pwr_mgmt_2, 0 ) ) return -1 ;    
    return 0;
}

/*
int getDataFromMpu(){
  return 0 ;
}
*/

/**
 *  @brief      Enter low-power accel-only mode.
 *  In low-power accel mode, the chip goes to sleep and only wakes up to sample
 *  the accelerometer at one of the following frequencies:
 *  \n MPU6050: 1.25Hz, 5Hz, 20Hz, 40Hz
  *  \n If the requested rate is not one listed above, the device will be set to
 *  the next highest rate. Requesting a rate above the maximum supported
 *  frequency will result in an error.
 *  \n To select a fractional wake-up frequency, round down the value passed to
 *  @e rate.
 *  @param[in]  rate        Minimum sampling rate, or zero to disable LP
 *                          accel mode.
 *  @return     0 if successful.
 */
/* 
int mpu_lp_accel_mode(unsigned char rate)
{
    unsigned char tmp[2];

    if (rate > 40)
        return -1;

    if (!rate) {
        mpu_set_int_latched(0);
        tmp[0] = 0;
        tmp[1] = BIT_STBY_XYZG;
//        if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 2, tmp))
        if (i2c_writeBlock(  GYRO_REG_pwr_mgmt_1, 2, tmp))
            return -1;
        chip_cfg.lp_accel_mode = 0;
        return 0;
    }
    // For LP accel, we automatically configure the hardware to produce latched
    // interrupts. In LP accel mode, the hardware cycles into sleep mode before
    // it gets a chance to deassert the interrupt pin; therefore, we shift this
    //  responsibility over to the MCU.
    //
    // Any register read will clear the interrupt.
     
    mpu_set_int_latched(1);
    tmp[0] = BIT_LPA_CYCLE;
    if (rate == 1) {
        tmp[1] = INV_LPA_1_25HZ;
        mpu_set_lpf(5);
    } else if (rate <= 5) {
        tmp[1] = INV_LPA_5HZ;
        mpu_set_lpf(5);
    } else if (rate <= 20) {
        tmp[1] = INV_LPA_20HZ;
        mpu_set_lpf(10);
    } else {
        tmp[1] = INV_LPA_40HZ;
        mpu_set_lpf(20);
    }
    tmp[1] = (tmp[1] << 6) | BIT_STBY_XYZG;
//    if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 2, tmp))
    if (i2c_writeBlock(  GYRO_REG_pwr_mgmt_1, 2, tmp))
        return -1;
    chip_cfg.sensors = INV_XYZ_ACCEL;
    chip_cfg.clk_src = 0;
    chip_cfg.lp_accel_mode = 1;
//    mpu_configure_fifo(0);    // not neaded; it seems that fifo set up is done when dmp feature are set up

    return 0;
}
*/

/**
 *  @brief      Read raw gyro data directly from the registers.                           // not used by oXs
 *  @param[out] data        Raw data in hardware units.
 *  @param[out] timestamp   Timestamp in milliseconds. Null if not needed.
 *  @return     0 if successful.
 */
 /*
int mpu_get_gyro_reg(short *data)
{
    unsigned char tmp[6];

    if (!(chip_cfg.sensors & INV_XYZ_GYRO))
        return -1;

    if (i2c_read( INV6050_addr, GYRO_REG_raw_gyro, 6, tmp))
        return -1;
    data[0] = (tmp[0] << 8) | tmp[1];
    data[1] = (tmp[2] << 8) | tmp[3];
    data[2] = (tmp[4] << 8) | tmp[5];
//    if (timestamp)             // set as coment to save flash memory
//        get_ms(timestamp);
    return 0;
}
*/

/**
 *  @brief      Read raw accel data directly from the registers.                         // not used by oXs
 *  @param[out] data        Raw data in hardware units.
 *  @param[out] timestamp   Timestamp in milliseconds. Null if not needed.
 *  @return     0 if successful.
 */
/* 
int mpu_get_accel_reg(short *data)
{
    unsigned char tmp[6];

    if (!(chip_cfg.sensors & INV_XYZ_ACCEL))
        return -1;

    if (i2c_read( INV6050_addr, GYRO_REG_raw_accel, 6, tmp))
        return -1;
    data[0] = (tmp[0] << 8) | tmp[1];
    data[1] = (tmp[2] << 8) | tmp[3];
    data[2] = (tmp[4] << 8) | tmp[5];
//    if (timestamp)                   // set as comment to save flash memory
//        get_ms(timestamp);
    return 0;
}
*/

/**                                                                                      // Not used by oXs
 *  @brief      Read temperature data directly from the registers.
 *  @param[out] data        Data in q16 format.
 *  @param[out] timestamp   Timestamp in milliseconds. Null if not needed.
 *  @return     0 if successful.
 */
/* 
int mpu_get_temperature(long *data)
{
    unsigned char tmp[2];
    short raw;

    if (!(chip_cfg.sensors))
        return -1;

    if (i2c_read( INV6050_addr, GYRO_REG_temp, 2, tmp))
        return -1;
    raw = (tmp[0] << 8) | tmp[1];
//    if (timestamp)               // set as comment to save flash memory
//        get_ms(timestamp);

    data[0] = (long)((35 + ((raw - (float)INV6050_temp_offset) / INV6050_temp_sens)) * 65536L);
    return 0;
}
*/

/**
 *  @brief      Push biases to the accel bias registers.                                          // not used by oXs but we should look if it can be used or not.
 *  This function expects biases relative to the current sensor output, and
 *  these biases will be added to the factory-supplied values.
 *  @param[in]  accel_bias  New biases.
 *  @return     0 if successful.
 */

int mpu_set_accel_bias(const long *accel_bias)                                                       
{
    unsigned char data[6];
    short accel_hw[3];
    short got_accel[3];
    short fg[3];

    if (!accel_bias)
        return -1;
    if (!accel_bias[0] && !accel_bias[1] && !accel_bias[2])
        return 0;

    if (i2c_read( INV6050_addr, 3, 3, data))
        return -1;
    fg[0] = ((data[0] >> 4) + 8) & 0xf;
    fg[1] = ((data[1] >> 4) + 8) & 0xf;
    fg[2] = ((data[2] >> 4) + 8) & 0xf;

    accel_hw[0] = (short)(accel_bias[0] * 2 / (64 + fg[0]));
    accel_hw[1] = (short)(accel_bias[1] * 2 / (64 + fg[1]));
    accel_hw[2] = (short)(accel_bias[2] * 2 / (64 + fg[2]));

    if (i2c_read( INV6050_addr, 0x06, 6, data))
        return -1;

    got_accel[0] = ((short)data[0] << 8) | data[1];
    got_accel[1] = ((short)data[2] << 8) | data[3];
    got_accel[2] = ((short)data[4] << 8) | data[5];

    accel_hw[0] += got_accel[0];
    accel_hw[1] += got_accel[1];
    accel_hw[2] += got_accel[2];

    data[0] = (accel_hw[0] >> 8) & 0xff;
    data[1] = (accel_hw[0]) & 0xff;
    data[2] = (accel_hw[1] >> 8) & 0xff;
    data[3] = (accel_hw[1]) & 0xff;
    data[4] = (accel_hw[2] >> 8) & 0xff;
    data[5] = (accel_hw[2]) & 0xff;

//    if (i2c_write( INV6050_addr, 0x06, 6, data))
    if (i2c_writeBlock(  0x06, 6, data))
        return -1;
    return 0;
}


/**
 *  @brief  Reset FIFO read/write pointers.
 *  @return 0 if successful.
 */
/* 
int mpu_reset_fifo(void)
{
    unsigned char data;

//    if (!(chip_cfg.sensors))         return -1;                // can be ommitted if whe assume that sensors is always filled

//    data = 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, &data))
    if (i2c_writeByte(  GYRO_REG_int_enable, 0))                // disable interrupt
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, &data))
    if (i2c_writeByte( GYRO_REG_fifo_en, 0))                    // disable all types of data in fifo (avoid that data are loaded in fifo) 
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &data))
    if (i2c_writeByte( GYRO_REG_user_ctrl, 0))                  // disable mpu fifo (bit 0x08) and fifo DMP (bit 0x80) 
        return -1;

    if (chip_cfg.dmp_on) {                                                     // this doe snot seem ok because there is no reset if dmp is off
//        data = BIT_FIFO_RST | BIT_DMP_RST;
//      if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &data))
        if (i2c_writeByte(  GYRO_REG_user_ctrl, BIT_FIFO_RST | BIT_DMP_RST ))   // reset the fifo (= clear the data in fifo); There are 2 bits
            return -1;
        delay(50);                                                              // wait that the reset is performed
//        data = BIT_DMP_EN | BIT_FIFO_EN;
//        if (chip_cfg.sensors & INV_XYZ_COMPASS)
//            data |= BIT_AUX_IF_EN;
//        if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &data))
        if (i2c_writeByte(  GYRO_REG_user_ctrl, BIT_DMP_EN | BIT_FIFO_EN))      // enable the fifo ( the 2 types)
            return -1;
        if (chip_cfg.int_enable)
            data = BIT_DMP_INT_EN;
        else
            data = 0;
//        if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, &data))
        if (i2c_writeByte( GYRO_REG_int_enable, data))                          // enable interrupt
            return -1;
        data = 0;
//        if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, &data)) // strange but this requires less flash
        if (i2c_writeBlock( GYRO_REG_fifo_en, 1, &data)) // strange but this requires less flash
//        if (i2c_writeByte(  GYRO_REG_fifo_en, 0) )                            // this is normally not needed because it is already done some steps before
            return -1;
    } else {
//        data = BIT_FIFO_RST;
//        if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &data))
        if (i2c_writeByte(  GYRO_REG_user_ctrl, BIT_FIFO_RST))                  // this force a reset of fifo : from datasheet :This bit resets the FIFO buffer when set to 1 while FIFO_EN equals 0.
            return -1;                                                          // it is strange that there is no delay for this reset
//        if (chip_cfg.bypass_mode || !(chip_cfg.sensors & INV_XYZ_COMPASS))
//            data = BIT_FIFO_EN;
//        else
//            data = BIT_FIFO_EN | BIT_AUX_IF_EN;
//        if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &data))            
        if (i2c_writeByte(GYRO_REG_user_ctrl, BIT_FIFO_EN | BIT_AUX_IF_EN))     // this enable fifo but it is strange that Aux is enable and DMP is not enable. This bit resets the I2C Master when set to 1 while I2C_MST_EN equals 0  
            return -1;                                                          //  This is not correct: in fact BIT_AUX_IF_EN = 0x20 = I2CMaster reset
        delay(50);                                                              // wait here ; This is buggy
        if (chip_cfg.int_enable)
            data = BIT_DATA_RDY_EN;
        else
            data = 0;
//        if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, &data))
        if (i2c_writeByte( GYRO_REG_int_enable, data))                         // reactive interrupt if they where active
            return -1;
//        if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, &chip_cfg.fifo_enable))    // strange but this line take less flash memory 
        if (i2c_writeBlock(  GYRO_REG_fifo_en, 1, &chip_cfg.fifo_enable))    // strange but this line take less flash memory 
//        if (i2c_writeByte( GYRO_REG_fifo_en, chip_cfg.fifo_enable))        //  enable type of fifo that where enable
            return -1;
    } // end of handling when dmp is off
    return 0;
}
*/

int mpu_reset_fifo(void)
{
    i2c_writeByte(  GYRO_REG_int_enable, 0) ;               // reg 0x38 ; disable interrupt
    i2c_writeByte( GYRO_REG_fifo_en, 0)     ;               // reg 0x23 ; disable all types of data in fifo (avoid that data are loaded in fifo) 
    i2c_writeByte( GYRO_REG_user_ctrl, 0)   ;               // disable mpu fifo (bit 0x08) and fifo DMP (bit 0x80) 
    i2c_writeByte(  GYRO_REG_user_ctrl, BIT_FIFO_RST | BIT_DMP_RST ) ;      // reset the fifo (= clear the data in fifo); There are 2 bits
    delay(50);                                                              // wait that the reset is performed
    i2c_writeByte( GYRO_REG_user_ctrl, BIT_DMP_EN | BIT_FIFO_EN);           // enable the fifo ( the 2 types)
    i2c_writeByte( GYRO_REG_int_enable, BIT_DMP_INT_EN ) ;                  // reg 0x38 ; byte = 0X02 ; enable interrupt on DMP ; this bit is not documented
    i2c_writeByte( GYRO_REG_fifo_en, 0)     ;               // reg 0x23 ; disable all types of data in fifo (avoid that data are loaded in fifo) // not sure it is required but it was so in original code
    return 0;
}


/**
 *  @brief      Get the gyro full-scale range.
 *  @param[out] fsr Current full-scale range.
 *  @return     0 if successful.
 */
/* 
int mpu_get_gyro_fsr(unsigned short *fsr)
{
    switch (chip_cfg.gyro_fsr) {
    case INV_FSR_250DPS:
        fsr[0] = 250;
        break;
    case INV_FSR_500DPS:
        fsr[0] = 500;
        break;
    case INV_FSR_1000DPS:
        fsr[0] = 1000;
        break;
    case INV_FSR_2000DPS:
        fsr[0] = 2000;
        break;
    default:
        fsr[0] = 0;
        break;
    }
    return 0;
}
*/

/**
 *  @brief      Set the gyro full-scale range.
 *  @param[in]  fsr Desired full-scale range.
 *  @return     0 if successful.
 */
/* 
int mpu_set_gyro_fsr(unsigned short fsr)
{
    unsigned char data;

    if (!(chip_cfg.sensors))
        return -1;

    switch (fsr) {
    case 250:
        data = INV_FSR_250DPS << 3;
        break;
    case 500:
        data = INV_FSR_500DPS << 3;
        break;
    case 1000:
        data = INV_FSR_1000DPS << 3;
        break;
    case 2000:
        data = INV_FSR_2000DPS << 3;
        break;
    default:
        return -1;
    }

    if (chip_cfg.gyro_fsr == (data >> 3))
        return 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_gyro_cfg, 1, &data))
    if (i2c_writeByte(  GYRO_REG_gyro_cfg, data))
        return -1;
    chip_cfg.gyro_fsr = data >> 3;
    return 0;
}
*/

/**
 *  @brief      Get the accel full-scale range.
 *  @param[out] fsr Current full-scale range.
 *  @return     0 if successful.
 */
/*
int mpu_get_accel_fsr(unsigned char *fsr)
{
    switch (chip_cfg.accel_fsr) {
    case INV_FSR_2G:
        fsr[0] = 2;
        break;
    case INV_FSR_4G:
        fsr[0] = 4;
        break;
    case INV_FSR_8G:
        fsr[0] = 8;
        break;
    case INV_FSR_16G:
        fsr[0] = 16;
        break;
    default:
        return -1;
    }
    if (chip_cfg.accel_half)
        fsr[0] <<= 1;
    return 0;
}
*/

/**
 *  @brief      Set the accel full-scale range.
 *  @param[in]  fsr Desired full-scale range.
 *  @return     0 if successful.
 */
 /*
int mpu_set_accel_fsr(unsigned char fsr)
{
    unsigned char data;

//    if (!(chip_cfg.sensors)) return -1;   // can be ommitted is we assume that sensor is always filled

    switch (fsr) {
    case 2:
        data = INV_FSR_2G << 3;
        break;
    case 4:
        data = INV_FSR_4G << 3;
        break;
    case 8:
        data = INV_FSR_8G << 3;
        break;
    case 16:
        data = INV_FSR_16G << 3;
        break;
    default:
        return -1;
    }

    if (chip_cfg.accel_fsr == (data >> 3))
        return 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_accel_cfg, 1, &data))
    if (i2c_writeByte( GYRO_REG_accel_cfg, data))
        return -1;
    chip_cfg.accel_fsr = data >> 3;
    return 0;
}
*/
  

/**
 *  @brief      Get the current DLPF setting.
 *  @param[out] lpf Current LPF setting.
 *  0 if successful.
 */
 /*
int mpu_get_lpf(unsigned short *lpf)
{
    switch (chip_cfg.lpf) {
    case INV_FILTER_188HZ:
        lpf[0] = 188;
        break;
    case INV_FILTER_98HZ:
        lpf[0] = 98;
        break;
    case INV_FILTER_42HZ:
        lpf[0] = 42;
        break;
    case INV_FILTER_20HZ:
        lpf[0] = 20;
        break;
    case INV_FILTER_10HZ:
        lpf[0] = 10;
        break;
    case INV_FILTER_5HZ:
        lpf[0] = 5;
        break;
    case INV_FILTER_256HZ_NOLPF2:
    case INV_FILTER_2100HZ_NOLPF:
    default:
        lpf[0] = 0;
        break;
    }
    return 0;
}
*/

/**
 *  @brief      Set digital low pass filter.
 *  The following LPF settings are supported: 188, 98, 42, 20, 10, 5.
 *  @param[in]  lpf Desired LPF setting.
 *  @return     0 if successful.
 */
 /*
int mpu_set_lpf(unsigned short lpf)
{
    unsigned char data;

//    if (!(chip_cfg.sensors)) return -1;      // can be ommitted if we assume that sensors is always filled

    if (lpf >= 188)
        data = INV_FILTER_188HZ;
    else if (lpf >= 98)
        data = INV_FILTER_98HZ;
    else if (lpf >= 42)
        data = INV_FILTER_42HZ;
    else if (lpf >= 20)
        data = INV_FILTER_20HZ;
    else if (lpf >= 10)
        data = INV_FILTER_10HZ;
    else
        data = INV_FILTER_5HZ;

    if (chip_cfg.lpf == data)
        return 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_lpf, 1, &data))
    if (i2c_writeByte( GYRO_REG_lpf, data))
        return -1;
    chip_cfg.lpf = data;
    return 0;
}
*/


/**
 *  @brief      Get sampling rate.
 *  @param[out] rate    Current sampling rate (Hz).
 *  @return     0 if successful.
 */
/* 
int mpu_get_sample_rate(unsigned short *rate)
{
    if (chip_cfg.dmp_on)
        return -1;
    else
        rate[0] = chip_cfg.sample_rate;
    return 0;
}
*/

/**
 *  @brief      Set sampling rate.
 *  Sampling rate must be between 4Hz and 1kHz.
 *  @param[in]  rate    Desired sampling rate (Hz).
 *  @return     0 if successful.
 */
/* 
int mpu_set_sample_rate(unsigned short rate)
{
    unsigned char data;

//    if (!(chip_cfg.sensors))  return -1;    // can be omitted if we assume that sensor is always filled

    if (chip_cfg.dmp_on)
        return -1;
    else {
        if (chip_cfg.lp_accel_mode) {
            if (rate && (rate <= 40)) {
                // Just stay in low-power accel mode. 
                mpu_lp_accel_mode(rate);
                return 0;
            }
            // Requested rate exceeds the allowed frequencies in LP accel mode,
            // switch back to full-power mode.
             
            mpu_lp_accel_mode(0);
        }
        if (rate < 4)
            rate = 4;
        else if (rate > 1000)
            rate = 1000;

        data = 1000 / rate - 1;
//        if (i2c_write( INV6050_addr, GYRO_REG_rate_div, 1, &data))
        if (i2c_writeByte( GYRO_REG_rate_div, data))
            return -1;

        chip_cfg.sample_rate = 1000 / (1 + data);

        // Automatically set LPF to 1/2 sampling rate. 
        mpu_set_lpf(chip_cfg.sample_rate >> 1);
        return 0;
    }
}
*/
int mpu_set_sample_rate200hz()
{
//    unsigned char data;
//        data = 4 ; // 1000 / rate - 1 ; //so 4 when rate = 200 hz
//        if (i2c_write( INV6050_addr, GYRO_REG_rate_div, 1, &data))
        if (i2c_writeByte( GYRO_REG_rate_div, 4 ))
            return -1;

//        chip_cfg.sample_rate =  200 ; //1000 / (1 + data);

        // set LPF to 1/2 sampling rate. 
//        mpu_set_lpf(100);
        if (i2c_writeByte( GYRO_REG_lpf, INV_FILTER_98HZ ))        return -1;
        return 0;
}



/**
 *  @brief      Get gyro sensitivity scale factor.
 *  @param[out] sens    Conversion from hardware units to dps.
 *  @return     0 if successful.
 */
/* 
int mpu_get_gyro_sens(float *sens)
{
    switch (chip_cfg.gyro_fsr) {
    case INV_FSR_250DPS:
        sens[0] = 131.f;
        break;
    case INV_FSR_500DPS:
        sens[0] = 65.5f;
        break;
    case INV_FSR_1000DPS:
        sens[0] = 32.8f;
        break;
    case INV_FSR_2000DPS:
        sens[0] = 16.4f;
        break;
    default:
        return -1;
    }
    return 0;
}
*/


/**
 *  @brief      Get accel sensitivity scale factor.
 *  @param[out] sens    Conversion from hardware units to g's.
 *  @return     0 if successful.
 */
/* 
int mpu_get_accel_sens(unsigned short *sens)
{
    switch (chip_cfg.accel_fsr) {
    case INV_FSR_2G:
        sens[0] = 16384;
        break;
    case INV_FSR_4G:
        sens[0] = 8092;
        break;
    case INV_FSR_8G:
        sens[0] = 4096;
        break;
    case INV_FSR_16G:
        sens[0] = 2048;
        break;
    default:
        return -1;
    }
    if (chip_cfg.accel_half)
        sens[0] >>= 1;
    return 0;
}
*/
/**
 *  @brief      Get current FIFO configuration.
 *  @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  @param[out] sensors Mask of sensors in FIFO.
 *  @return     0 if successful.
 */
/* 
int mpu_get_fifo_configxx(unsigned char *sensors)
{
    sensors[0] = chip_cfg.fifo_enable;
    return 0;
}
*/

/**
 *  @brief      Select which sensors are pushed to FIFO.          // not used by oxs because the fifo is set up by dmp feature
 *  @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  @param[in]  sensors Mask of sensors to push to FIFO.
 *  @return     0 if successful.
 */
/* 
int mpu_configure_fifo(unsigned char sensors)
{
    unsigned char prev;
    int result = 0;

    // Compass data isn't going into the FIFO. Stop trying. 
//    sensors &= ~INV_XYZ_COMPASS;

    if (chip_cfg.dmp_on)
        return 0;
    else {
///        if (!(chip_cfg.sensors))
//            return -1;
        prev = chip_cfg.fifo_enable;
        chip_cfg.fifo_enable = sensors & chip_cfg.sensors;
        if (chip_cfg.fifo_enable != sensors)
            // You're not getting what you asked for. Some sensors are
             // asleep.
             
            result = -1;
        else
            result = 0;
        if (sensors || chip_cfg.lp_accel_mode)
            set_int_enable(1);
        else
            set_int_enable(0);
        if (sensors) {
            if (mpu_reset_fifo()) {
                chip_cfg.fifo_enable = prev;
                return -1;
            }
        }
    }

    return result;
}
*/

/**
 *  @brief      Get current power state.
 *  @param[in]  power_on    1 if turned on, 0 if suspended.
 *  @return     0 if successful.
 */
/* 
int mpu_get_power_state(unsigned char *power_on)
{
    if (chip_cfg.sensors)
        power_on[0] = 1;
    else
        power_on[0] = 0;
    return 0;
}
*/

/**
 *  @brief      Turn specific sensors on/off.
 *  @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  \n INV_XYZ_COMPASS
 *  @param[in]  sensors    Mask of sensors to wake.
 *  @return     0 if successful.
 */
/*
int mpu_set_sensors(unsigned char sensors)
{
    unsigned char data;

    if (sensors & INV_XYZ_GYRO)
        data = INV_CLK_PLL;
    else if (sensors)
        data = 0;
    else
        data = BIT_SLEEP;
//    if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 1, &data)) {
    if (i2c_writeByte( GYRO_REG_pwr_mgmt_1, data)) {
        chip_cfg.sensors = 0;
        return -1;
    }
//    chip_cfg.clk_src = data & ~BIT_SLEEP;

    data = 0;
    if (!(sensors & INV_X_GYRO))
        data |= BIT_STBY_XG;
    if (!(sensors & INV_Y_GYRO))
        data |= BIT_STBY_YG;
    if (!(sensors & INV_Z_GYRO))
        data |= BIT_STBY_ZG;
    if (!(sensors & INV_XYZ_ACCEL))
        data |= BIT_STBY_XYZA;
//    if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_2, 1, &data)) {
    if (i2c_writeByte( GYRO_REG_pwr_mgmt_2, data)) {
        chip_cfg.sensors = 0;
        return -1;
    } 
//    if (sensors && (sensors != INV_XYZ_ACCEL))         // Latched interrupts only used in LP accel mode. 
//        mpu_set_int_latched(0);

    chip_cfg.sensors = sensors;
  //  chip_cfg.lp_accel_mode = 0;
    delay(50);
    return 0;
}
*/

void mpu_enable_pwm_mgnt() {
  i2c_writeByte( GYRO_REG_pwr_mgmt_1, INV_CLK_PLL ) ;  // define the clock to be used
  i2c_writeByte( GYRO_REG_pwr_mgmt_2, 0 ) ; // 
  delay(50);
  i2c_writeByte( GYRO_REG_int_pin_cfg,BIT_ACTL | BIT_BYPASS_EN ) ;       // in reg 0x37 ,set interrupt active low and allow to let MPU bypass I2C to HMC5883 ( requires also I2C_MST-EN bit = 0 in reg 6A and SLEEP = 0 in PWR_MNGT_1 )
//  chip_cfg.sensors = (INV_XYZ_GYRO | INV_XYZ_ACCEL ) ;
}
/**
 *  @brief      Read the MPU interrupt status registers.
 *  @param[out] status  Mask of interrupt bits.
 *  @return     0 if successful.
 */
 /*
int mpu_get_int_status(short *status)
{
    unsigned char tmp[2];
//    if (!chip_cfg.sensors)
//        return -1;
    if (i2c_read( INV6050_addr, GYRO_REG_dmp_int_status, 2, tmp))
        return -1;
    status[0] = (tmp[0] << 8) | tmp[1];
    return 0;
}
*/

/**
 *  @brief      Get one packet from the FIFO.
 *  If @e sensors does not contain a particular sensor, disregard the data
 *  returned to that pointer.
 *  \n @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  \n If the FIFO has no new data, @e sensors will be zero.
 *  \n If the FIFO is disabled, @e sensors will be zero and this function will
 *  return a non-zero error code.
 *  @param[out] gyro        Gyro data in hardware units.
 *  @param[out] accel       Accel data in hardware units.
 *  @param[out] timestamp   Timestamp in milliseconds.
 *  @param[out] sensors     Mask of sensors read from FIFO.
 *  @param[out] more        Number of remaining packets.
 *  @return     0 if successful.  Negative if error:  -1:  Misconfigured; -2:  I2C read error; -3:  Fifo Overflow
 */
/* 
int mpu_read_fifo(short *gyro, short *accel, unsigned char *sensors, unsigned char *more)
{
    // Assumes maximum packet size is gyro (6) + accel (6). 
    unsigned char data[MAX_PACKET_LENGTH];
    unsigned char packet_size = 0;
    unsigned short fifo_count, index = 0;

    if (chip_cfg.dmp_on)
        return -1;

    sensors[0] = 0;
//    if (!chip_cfg.sensors)
//        return -5;
//    if (!chip_cfg.fifo_enable)
//        return -6;

    if (chip_cfg.fifo_enable & INV_X_GYRO)
        packet_size += 2;
    if (chip_cfg.fifo_enable & INV_Y_GYRO)
        packet_size += 2;
    if (chip_cfg.fifo_enable & INV_Z_GYRO)
        packet_size += 2;
    if (chip_cfg.fifo_enable & INV_XYZ_ACCEL)
        packet_size += 6;

    if (i2c_read( INV6050_addr, GYRO_REG_fifo_count_h, 2, data))
        return -2;
    fifo_count = (data[0] << 8) | data[1];
    if (fifo_count < packet_size)
        return 0;
//    log_i("FIFO count: %hd\n", fifo_count);
    if (fifo_count > (INV6050_max_fifo >> 1)) {
        // FIFO is 50% full, better check overflow bit. 
        if (i2c_read( INV6050_addr, GYRO_REG_int_status, 1, data))
            return -2;
        if (data[0] & BIT_FIFO_OVERFLOW) {
            mpu_reset_fifo();
            return -3;
        }
    }
//    get_ms((unsigned long*)timestamp); // set as comment to save flash memory

    if (i2c_read( INV6050_addr, GYRO_REG_fifo_r_w, packet_size, data))
        return -2;
    more[0] = fifo_count / packet_size - 1;
    sensors[0] = 0;

    if ((index != packet_size) && chip_cfg.fifo_enable & INV_XYZ_ACCEL) {
        accel[0] = (data[index+0] << 8) | data[index+1];
        accel[1] = (data[index+2] << 8) | data[index+3];
        accel[2] = (data[index+4] << 8) | data[index+5];
        sensors[0] |= INV_XYZ_ACCEL;
        index += 6;
    }
    if ((index != packet_size) && chip_cfg.fifo_enable & INV_X_GYRO) {
        gyro[0] = (data[index+0] << 8) | data[index+1];
        sensors[0] |= INV_X_GYRO;
        index += 2;
    }
    if ((index != packet_size) && chip_cfg.fifo_enable & INV_Y_GYRO) {
        gyro[1] = (data[index+0] << 8) | data[index+1];
        sensors[0] |= INV_Y_GYRO;
        index += 2;
    }
    if ((index != packet_size) && chip_cfg.fifo_enable & INV_Z_GYRO) {
        gyro[2] = (data[index+0] << 8) | data[index+1];
        sensors[0] |= INV_Z_GYRO;
        index += 2;
    }

    return 0;
}
*/

/**
 *  @brief      Get one unparsed packet from the FIFO.
 *  This function should be used if the packet is to be parsed elsewhere.
 *  @param[in]  length  Length of one FIFO packet.
 *  @param[in]  data    FIFO packet.
 *  @param[in]  more    Number of remaining packets.
 *  @return     0 if successful.  Negative if error:  -1:  DMP Not On; -2:  I2C read error; -3:  Fifo Overflow -4: No Sensors -5: No more data available
*/

#ifdef DEBUG_READ_FIFO
    unsigned short fifo_count_debug ;
    unsigned short mpu_getfifo_count_debug(){
      return fifo_count_debug; 
    }
#endif    

int mpu_read_fifo_stream(unsigned short length, unsigned char *data,  unsigned char *more)
{
    unsigned char tmp[2];
    unsigned short fifo_count;
//    if (!chip_cfg.dmp_on)    // error if dmp is not ON
//        return -1;
//    if (!chip_cfg.sensors)
//        return -4;              // 2 = there is no sensor defined to be active( or to be read?)

    if (i2c_read( INV6050_addr, GYRO_REG_fifo_count_h, 2, tmp))
        return -2;             //  I2C error reading the 6050
    fifo_count = (tmp[0] << 8) | tmp[1];
#ifdef DEBUG_READ_FIFO
    fifo_count_debug = fifo_count; 
#endif    
    if (fifo_count < length) {
        more[0] = 0;
        return -5;             //  fifo does not contains the expected number of char.
    }
    if (fifo_count > (INV6050_max_fifo >> 1)) {
        /* FIFO is 50% full, better check overflow bit. */
        if (i2c_read( INV6050_addr, GYRO_REG_int_status, 1, tmp))
            return -2;         // i2c error reading the fifo
        if (tmp[0] & BIT_FIFO_OVERFLOW) {
            mpu_reset_fifo();
            return -3;        // fifo inside 6050 has lost some data.
        }
    }

    if (i2c_read( INV6050_addr, GYRO_REG_fifo_r_w, length, data))
        return -2;
    more[0] = fifo_count / length - 1;
    return 0;
}

/**
 *  @brief      Set device to bypass mode.
 *  @param[in]  bypass_on   1 to enable bypass mode.
 *  @return     0 if successful.
 */
/*
int mpu_set_bypass(unsigned char bypass_on)
{
    unsigned char tmp;

    if (chip_cfg.bypass_mode == bypass_on)
        return 0;

    if (bypass_on) {
        if (i2c_read( INV6050_addr, GYRO_REG_user_ctrl, 1, &tmp))
            return -1;
        tmp &= ~BIT_AUX_IF_EN;
//        if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &tmp))
        if (i2c_writeByte( GYRO_REG_user_ctrl, tmp))
            return -1;
        delay(3);
        tmp = BIT_BYPASS_EN;
        if (chip_cfg.active_low_int)
            tmp |= BIT_ACTL;
        if (chip_cfg.latched_int)
            tmp |= BIT_LATCH_EN | BIT_ANY_RD_CLR;
//        if (i2c_write( INV6050_addr, GYRO_REG_int_pin_cfg, 1, &tmp))        // strange but this line requires less flash memory
        if (i2c_writeBlock( GYRO_REG_int_pin_cfg, 1, &tmp))        // strange but this line requires less flash memory
//        if (i2c_writeByte( GYRO_REG_int_pin_cfg, tmp))
            return -1;
    } else {
        // Enable I2C master mode if compass is being used. 
        if (i2c_read( INV6050_addr, GYRO_REG_user_ctrl, 1, &tmp))
            return -1;
        if (chip_cfg.sensors & INV_XYZ_COMPASS)
            tmp |= BIT_AUX_IF_EN;
        else
            tmp &= ~BIT_AUX_IF_EN;
//        if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, &tmp))
        if (i2c_writeByte(  GYRO_REG_user_ctrl, tmp))
            return -1;
        delay(3);
        if (chip_cfg.active_low_int)
            tmp = BIT_ACTL;
        else
            tmp = 0;
        if (chip_cfg.latched_int)
            tmp |= BIT_LATCH_EN | BIT_ANY_RD_CLR;
//        if (i2c_write( INV6050_addr, GYRO_REG_int_pin_cfg, 1, &tmp))         // strange but this line requires less flash memory
        if (i2c_writeBlock(  GYRO_REG_int_pin_cfg, 1, &tmp))         // strange but this line requires less flash memory
//        if (i2c_writeByte( GYRO_REG_int_pin_cfg, tmp))
            return -1;
    }
    chip_cfg.bypass_mode = bypass_on;
    return 0;
}
*/

/**
 *  @brief      Set interrupt level.
 *  @param[in]  active_low  1 for active low, 0 for active high.
 *  @return     0 if successful.
 */
/* 
int mpu_set_int_level(unsigned char active_low)
{
    chip_cfg.active_low_int = active_low;
    return 0;
}
*/
/**
 *  @brief      Enable latched interrupts.
 *  Any MPU register will clear the interrupt.
 *  @param[in]  enable  1 to enable, 0 to disable.
 *  @return     0 if successful.
 */
 /*
int mpu_set_int_latched(unsigned char enable)
{
    unsigned char tmp;
    if (chip_cfg.latched_int == enable)
        return 0;

    if (enable)
        tmp = BIT_LATCH_EN | BIT_ANY_RD_CLR;
    else
        tmp = 0;
    if (chip_cfg.bypass_mode)
        tmp |= BIT_BYPASS_EN;
    if (chip_cfg.active_low_int)
        tmp |= BIT_ACTL;
//    if (i2c_write( INV6050_addr, GYRO_REG_int_pin_cfg, 1, &tmp))
    if (i2c_writeByte( GYRO_REG_int_pin_cfg,tmp))
        return -1;
    chip_cfg.latched_int = enable;
    return 0;
}
*/
/*
static int get_accel_prod_shift(float *st_shift)                          // this function is not used by oXs; 
{
    unsigned char tmp[4], shift_code[3], ii;

    if (i2c_read( INV6050_addr, 0x0D, 4, tmp))
        return 0x07;

    shift_code[0] = ((tmp[0] & 0xE0) >> 3) | ((tmp[3] & 0x30) >> 4);
    shift_code[1] = ((tmp[1] & 0xE0) >> 3) | ((tmp[3] & 0x0C) >> 2);
    shift_code[2] = ((tmp[2] & 0xE0) >> 3) | (tmp[3] & 0x03);
    for (ii = 0; ii < 3; ii++) {
        if (!shift_code[ii]) {
            st_shift[ii] = 0.f;
            continue;
        }
        // Equivalent to..
        // st_shift[ii] = 0.34f * powf(0.92f/0.34f, (shift_code[ii]-1) / 30.f)
        //
        st_shift[ii] = 0.34f;
        while (--shift_code[ii])
            st_shift[ii] *= 1.034f;
    }
    return 0;
}
*/

/*
static int accel_self_test(long *bias_regular, long *bias_st)
{
    int jj, result = 0;
    float st_shift[3], st_shift_cust, st_shift_var;

    get_accel_prod_shift(st_shift);
    for(jj = 0; jj < 3; jj++) {
        st_shift_cust = labs(bias_regular[jj] - bias_st[jj]) / 65536.f;
        if (st_shift[jj]) {
            st_shift_var = st_shift_cust / st_shift[jj] - 1.f;
            if (fabs(st_shift_var) > TEST_max_accel_var)
                result |= 1 << jj;
        } else if ((st_shift_cust < TEST_min_g) ||
            (st_shift_cust > TEST_max_g))
            result |= 1 << jj;
    }

    return result;
}
*/
/*
static int gyro_self_test(long *bias_regular, long *bias_st)
{
    int jj, result = 0;
    unsigned char tmp[3];
    float st_shift, st_shift_cust, st_shift_var;

    if (i2c_read( INV6050_addr, 0x0D, 3, tmp))
        return 0x07;

    tmp[0] &= 0x1F;
    tmp[1] &= 0x1F;
    tmp[2] &= 0x1F;

    for (jj = 0; jj < 3; jj++) {
        st_shift_cust = labs(bias_regular[jj] - bias_st[jj]) / 65536.f;
        if (tmp[jj]) {
            st_shift = 3275.f / TEST_gyro_sens;
            while (--tmp[jj])
                st_shift *= 1.046f;
            st_shift_var = st_shift_cust / st_shift - 1.f;
            if (fabs(st_shift_var) > TEST_max_gyro_var)
                result |= 1 << jj;
        } else if ((st_shift_cust < TEST_min_dps) ||
            (st_shift_cust > TEST_max_dps))
            result |= 1 << jj;
    }
    return result;
}
*/
/*
//static int get_st_biases(long *gyro, long *accel, unsigned char hw_test)               // this function is not used by oXs    
int get_st_biases(long *gyro, long *accel, unsigned char hw_test)                        // modified in non static by MS
{
    unsigned char data[MAX_PACKET_LENGTH];
    unsigned char packet_count, ii;
    unsigned short fifo_count;

    data[0] = 0x01;
    data[1] = 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 2, data))
    if (i2c_writeBlock( GYRO_REG_pwr_mgmt_1, 2, data))    
        return -1;
    delay(200);
    data[0] = 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, data))
    if (i2c_writeBlock( GYRO_REG_int_enable, 1, data))
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, data))
    if (i2c_writeBlock( GYRO_REG_fifo_en, 1, data))
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 1, data))
    if (i2c_writeBlock( GYRO_REG_pwr_mgmt_1, 1, data))
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_i2c_mst, 1, data))
    if (i2c_writeBlock( GYRO_REG_i2c_mst, 1, data))
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, data))
    if (i2c_writeBlock( GYRO_REG_user_ctrl, 1, data))
        return -1;
    data[0] = BIT_FIFO_RST | BIT_DMP_RST;
//    if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, data))
    if (i2c_writeBlock(GYRO_REG_user_ctrl, 1, data))
        return -1;
    delay(15);
    data[0] = TEST_reg_lpf;
//    if (i2c_write( INV6050_addr, GYRO_REG_lpf, 1, data))
    if (i2c_writeBlock( GYRO_REG_lpf, 1, data))
        return -1;
    data[0] = TEST_reg_rate_div;
//    if (i2c_write( INV6050_addr, GYRO_REG_rate_div, 1, data))
    if (i2c_writeBlock(  GYRO_REG_rate_div, 1, data))
        return -1;
    if (hw_test)
        data[0] = TEST_reg_gyro_fsr | 0xE0;
    else
        data[0] = TEST_reg_gyro_fsr;
//    if (i2c_write( INV6050_addr, GYRO_REG_gyro_cfg, 1, data))
    if (i2c_writeBlock(  GYRO_REG_gyro_cfg, 1, data))
        return -1;

    if (hw_test)
        data[0] = TEST_reg_accel_fsr | 0xE0;
    else
        data[0] = TEST_reg_accel_fsr;
//    if (i2c_write( INV6050_addr, GYRO_REG_accel_cfg, 1, data))
    if (i2c_writeBlock(  GYRO_REG_accel_cfg, 1, data))
        return -1;
    if (hw_test)
        delay(200);

    // Fill FIFO for TEST_wait_ms milliseconds. 
    data[0] = BIT_FIFO_EN;
//    if (i2c_write( INV6050_addr, GYRO_REG_user_ctrl, 1, data))
    if (i2c_writeBlock( GYRO_REG_user_ctrl, 1, data))
        return -1;

    data[0] = INV_XYZ_GYRO | INV_XYZ_ACCEL;
//    if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, data))
    if (i2c_writeBlock( GYRO_REG_fifo_en, 1, data))
        return -1;
    delay(TEST_wait_ms);
    data[0] = 0;
//    if (i2c_write( INV6050_addr, GYRO_REG_fifo_en, 1, data))
    if (i2c_writeBlock( GYRO_REG_fifo_en, 1, data))
        return -1;

    if (i2c_read( INV6050_addr, GYRO_REG_fifo_count_h, 2, data))
        return -1;

    fifo_count = (data[0] << 8) | data[1];
    packet_count = fifo_count / MAX_PACKET_LENGTH;
    gyro[0] = gyro[1] = gyro[2] = 0;
    accel[0] = accel[1] = accel[2] = 0;

    for (ii = 0; ii < packet_count; ii++) {
        short accel_cur[3], gyro_cur[3];
        if (i2c_read( INV6050_addr, GYRO_REG_fifo_r_w, MAX_PACKET_LENGTH, data))
            return -1;
        accel_cur[0] = ((short)data[0] << 8) | data[1];
        accel_cur[1] = ((short)data[2] << 8) | data[3];
        accel_cur[2] = ((short)data[4] << 8) | data[5];
        accel[0] += (long)accel_cur[0];
        accel[1] += (long)accel_cur[1];
        accel[2] += (long)accel_cur[2];
        gyro_cur[0] = (((short)data[6] << 8) | data[7]);
        gyro_cur[1] = (((short)data[8] << 8) | data[9]);
        gyro_cur[2] = (((short)data[10] << 8) | data[11]);
        gyro[0] += (long)gyro_cur[0];
        gyro[1] += (long)gyro_cur[1];
        gyro[2] += (long)gyro_cur[2];
    }
#ifdef EMPL_NO_64BIT
    gyro[0] = (long)(((float)gyro[0]*65536.f) / TEST_gyro_sens / packet_count);
    gyro[1] = (long)(((float)gyro[1]*65536.f) / TEST_gyro_sens / packet_count);
    gyro[2] = (long)(((float)gyro[2]*65536.f) / TEST_gyro_sens / packet_count);
    if (has_accel) {
        accel[0] = (long)(((float)accel[0]*65536.f) / TEST_accel_sens /
            packet_count);
        accel[1] = (long)(((float)accel[1]*65536.f) / TEST_accel_sens /
            packet_count);
        accel[2] = (long)(((float)accel[2]*65536.f) / TEST_accel_sens /
            packet_count);
        // Don't remove gravity! 
        accel[2] -= 65536L;
    }
#else
    gyro[0] = (long)(((long long)gyro[0]<<16) / TEST_gyro_sens / packet_count);
    gyro[1] = (long)(((long long)gyro[1]<<16) / TEST_gyro_sens / packet_count);
    gyro[2] = (long)(((long long)gyro[2]<<16) / TEST_gyro_sens / packet_count);
    accel[0] = (long)(((long long)accel[0]<<16) / TEST_accel_sens /
        packet_count);
    accel[1] = (long)(((long long)accel[1]<<16) / TEST_accel_sens /
        packet_count);
    accel[2] = (long)(((long long)accel[2]<<16) / TEST_accel_sens /
        packet_count);
    // Don't remove gravity! 
    if (accel[2] > 0L)
        accel[2] -= 65536L;
    else
        accel[2] += 65536L;
#endif

    return 0;
}
*/

/**
 *  @brief      Trigger gyro/accel/compass self-test.                               // this function is normally not used by oXs (so no flash is used)
 *  On success/error, the self-test returns a mask representing the sensor(s)
 *  that failed. For each bit, a one (1) represents a "pass" case; conversely,
 *  a zero (0) indicates a failure.
 *
 *  \n The mask is defined as follows:
 *  \n Bit 0:   Gyro.
 *  \n Bit 1:   Accel.
 *  \n Bit 2:   Compass.
 *
 *  \n Currently, the hardware self-test is unsupported for MPU6500. However,
 *  this function can still be used to obtain the accel and gyro biases.
 *
 *  \n This function must be called with the device either face-up or face-down
 *  (z-axis is parallel to gravity).
 *  @param[out] gyro        Gyro biases in q16 format.
 *  @param[out] accel       Accel biases (if applicable) in q16 format.
 *  @return     Result mask (see above).
 */
/* 
int mpu_run_self_test(long *gyro, long *accel)
{
    const unsigned char tries = 2;
    long gyro_st[3], accel_st[3];
    unsigned char accel_result, gyro_result;
    int ii;
    int result;
    unsigned char accel_fsr, fifo_sensors, sensors_on;
    unsigned short gyro_fsr, sample_rate, lpf;
    unsigned char dmp_was_on;

    if (chip_cfg.dmp_on) {
        mpu_set_dmp_state(0);
        dmp_was_on = 1;
    } else
        dmp_was_on = 0;

    // Get initial settings. 
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);
    mpu_get_lpf(&lpf);
    mpu_get_sample_rate(&sample_rate);
    sensors_on = chip_cfg.sensors;
    mpu_get_fifo_config(&fifo_sensors);

    // For older chips, the self-test will be different. 
    for (ii = 0; ii < tries; ii++)
        if (!get_st_biases(gyro, accel, 0))
            break;
    if (ii == tries) {
        // If we reach this point, we most likely encountered an I2C error.
        // We'll just report an error for all three sensors.
         
        result = 0;
        goto restore;
    }
    for (ii = 0; ii < tries; ii++)
        if (!get_st_biases(gyro_st, accel_st, 1))
            break;
    if (ii == tries) {
        // Again, probably an I2C error. 
        result = 0;
        goto restore;
    }
    accel_result = accel_self_test(accel, accel_st);
    gyro_result = gyro_self_test(gyro, gyro_st);

    result = 0;
    if (!gyro_result)
        result |= 0x01;
    if (!accel_result)
        result |= 0x02;

restore:
    // Set to invalid values to ensure no I2C writes are skipped. 
    chip_cfg.gyro_fsr = 0xFF;
    chip_cfg.accel_fsr = 0xFF;
    chip_cfg.lpf = 0xFF;
    chip_cfg.sample_rate = 0xFFFF;
    chip_cfg.sensors = 0xFF;
    chip_cfg.fifo_enable = 0xFF;
    chip_cfg.clk_src = INV_CLK_PLL;
    mpu_set_gyro_fsr(gyro_fsr);
    mpu_set_accel_fsr(accel_fsr);
    mpu_set_lpf(lpf);
    mpu_set_sample_rate(sample_rate);
    mpu_set_sensors(sensors_on);
    mpu_configure_fifo(fifo_sensors);

    if (dmp_was_on)
        mpu_set_dmp_state(1);

    return result;
}
*/

/**
 *  @brief      Write to the DMP memory.
 *  This function prevents I2C writes past the bank boundaries. The DMP memory
 *  is only accessible when the chip is awake.
 *  @param[in]  mem_addr    Memory location (bank << 8 | start address)
 *  @param[in]  length      Number of bytes to write.
 *  @param[in]  data        Bytes to write to memory.
 *  @return     0 if successful.
 */
int mpu_write_mem(unsigned short mem_addr, unsigned short length, unsigned char *data)
{
    unsigned char tmp[2];
//    if (!data)                                                                           // saving : could be avoided to save flash
//        return -1;
//    if (!chip_cfg.sensors)                                                            // saving : could be avoided to save flash
//        return -1;

    tmp[0] = (unsigned char)(mem_addr >> 8);
    tmp[1] = (unsigned char)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if (tmp[1] + length > INV6050_bank_size)                                             // saving : could be avoided to save flash 
        return -1;

//    if (i2c_write( INV6050_addr, GYRO_REG_bank_sel, 2, tmp))
    if (i2c_writeBlock( GYRO_REG_bank_sel, 2, tmp))
        return -1;
//    if (i2c_write( INV6050_addr, GYRO_REG_mem_r_w, length, data))
    if (i2c_writeBlock( GYRO_REG_mem_r_w, length, data))
        return -1;
    return 0;
}

/**
 *  @brief      Read from the DMP memory.
 *  This function prevents I2C reads past the bank boundaries. The DMP memory
 *  is only accessible when the chip is awake.
 *  @param[in]  mem_addr    Memory location (bank << 8 | start address)
 *  @param[in]  length      Number of bytes to read.
 *  @param[out] data        Bytes read from memory.
 *  @return     0 if successful.
 */
int mpu_read_mem(unsigned short mem_addr, unsigned short length, unsigned char *data)   // saving : this function is used when DMP firmware is uploaded; perhaps it can be avoided if we thrust the I2C
{
    unsigned char tmp[2];

//    if (!data)                                                                          // saving : could be avoided to save flash
//        return -1;
//    if (!chip_cfg.sensors)                                                           // saving : could be avoided
//        return -1;

    tmp[0] = (unsigned char)(mem_addr >> 8);
    tmp[1] = (unsigned char)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if (tmp[1] + length > INV6050_bank_size)                                            // saving : could be avoided
        return -1;

//    if (i2c_write( INV6050_addr, GYRO_REG_bank_sel, 2, tmp))
    if (i2c_writeBlock( GYRO_REG_bank_sel, 2, tmp))
        return -1;
    if (i2c_read( INV6050_addr, GYRO_REG_mem_r_w, length, data))
        return -1;
    return 0;
}

/**
 *  @brief      Load and verify DMP image.
 *  @param[in]  length      Length of DMP image.
 *  @param[in]  firmware    DMP code.
 *  @param[in]  start_addr  Starting address of DMP code memory.
 *  @param[in]  sample_rate Fixed sampling rate used when DMP is enabled.
 *  @return     0 if successful.
 */
//int mpu_load_firmware(unsigned short length, const unsigned char *firmware, unsigned short start_addr, unsigned short sample_rate)
int mpu_load_firmware(unsigned short length, const unsigned char *firmware, unsigned short start_addr)
{
    unsigned short ii;
    unsigned short this_write;
    /* Must divide evenly into INV6050_bank_size to avoid bank crossings. */
#define LOAD_CHUNK  (16)                                                   // write/read data by group of 16.
    unsigned char cur[LOAD_CHUNK], tmp[2];
	
	// For arduino, read firmware out of PROGMEM (Flash)
	
	unsigned char firmware_chunk[LOAD_CHUNK];
	
//    if (chip_cfg.dmp_loaded)                                            // saving : this check could be avoided
        /* DMP should only be loaded once. */
//        return -3;

//    if (!firmware)                                                         // saving : this check could be avoided
//        return -4;
    for (ii = 0; ii < length; ii += this_write) {
        this_write = min(LOAD_CHUNK, length - ii);                        //upload will be handle by max 16 bytes
		
    		unsigned short x;
    		unsigned char *pFirmware = (unsigned char *)&firmware[ii];
    		for ( x = 0; x < this_write; x++ ) {
    			firmware_chunk[x] = pgm_read_byte_near(pFirmware+x);           // fill a buffer of max 16 byte with the data from flash
    		}
        if (mpu_write_mem(ii, this_write, firmware_chunk))               // write the bytes at adress ii
            return -1;
			
        if (mpu_read_mem(ii, this_write, cur))
            return -1;
    		if (memcmp(firmware_chunk, cur, this_write))                     // compare if what has been read is the same.
                return -2;
    }

    /* Set program start address. */
    tmp[0] = start_addr >> 8;
    tmp[1] = start_addr & 0xFF;
//    if (i2c_write( INV6050_addr, GYRO_REG_prgm_start_h, 2, tmp))
    if (i2c_writeBlock( GYRO_REG_prgm_start_h, 2, tmp))
        return -5;

//    chip_cfg.dmp_loaded = 1;
//    chip_cfg.dmp_sample_rate = sample_rate;
    return 0;
}

/**
 *  @brief      Enable/disable DMP support.
 *  @param[in]  enable  1 to turn on the DMP.
 *  @return     0 if successful.
 */
 /*
int mpu_set_dmp_state(unsigned char enable)
{
    unsigned char tmp;                                        // saving : this test could be ommited
    if (chip_cfg.dmp_on == enable)
        return 0;

    if (enable) {
        if (!chip_cfg.dmp_loaded)                          // saving : this test could be ommited if we take care to load first
            return -1;
        // Disable data ready interrupt. 
        set_int_enable(0);                                   // probably not requested because this is the default value
        // Disable bypass mode. 
        mpu_set_bypass(0);                                             // further to check if really needed
        // Keep constant sample rate, FIFO rate controlled by DMP. 
        mpu_set_sample_rate(chip_cfg.dmp_sample_rate);                 // further to check if really needed
        // Remove FIFO elements. 
//        tmp = 0;
//        i2c_write( INV6050_addr, 0x23, 1, &tmp);
        i2c_writeByte(  0x23, 0);
        chip_cfg.dmp_on = 1;
        // Enable DMP interrupt. 
        set_int_enable(1);
        mpu_reset_fifo();
    }  else {
        // Disable DMP interrupt. 
        set_int_enable(0);
        // Restore FIFO settings. 
        tmp = chip_cfg.fifo_enable;
//        i2c_write( INV6050_addr, 0x23, 1, &tmp);
        i2c_writeByte(  0x23, tmp);
        chip_cfg.dmp_on = 0;
        mpu_reset_fifo();
    }    
    return 0;
}
*/
int mpu_set_dmp_state_on()
{
        i2c_writeByte(  GYRO_REG_int_enable, 0) ;   // reg 0x38 ; disable interrupt // normally not required because it was not enabled     
        mpu_set_sample_rate200hz();                 // not sure it must be done (because it is already done in firmware
        i2c_writeByte(  GYRO_REG_fifo_en , 0);      // reg 0x23 ; disable all types of data in fifo (avoid that data are loaded in fifo) // not sure it is required but it was so in original code
//        chip_cfg.dmp_on = 1;
        i2c_writeByte( GYRO_REG_int_enable, BIT_DMP_INT_EN ) ;                  // reg 0x38 ; byte = 0X02 ; enable interrupt on DMP ; this bit is not documented
        mpu_reset_fifo() ;  // this wil Enable DMP interrupt too (so not sure previous int enable is requested)
    return 0;
}

/**
 *  @brief      Get DMP state.
 *  @param[out] enabled 1 if enabled.
 *  @return     0 if successful.
 */
 /*
int mpu_get_dmp_state(unsigned char *enabled)
{
    enabled[0] = chip_cfg.dmp_on;
    return 0;
}
*/

/**                                                                               // this code is not used by oXs
 *  @brief      Enters LP accel motion interrupt mode.
 *  The behavior of this feature is very different between the MPU6050 and the
 *  MPU6500. Each chip's version of this feature is explained below.
 *
 *  \n MPU6050:
 *  \n When this mode is first enabled, the hardware captures a single accel
 *  sample, and subsequent samples are compared with this one to determine if
 *  the device is in motion. Therefore, whenever this "locked" sample needs to
 *  be changed, this function must be called again.
 *
 *  \n The hardware motion threshold can be between 32mg and 8160mg in 32mg
 *  increments.
 *
 *  \n Low-power accel mode supports the following frequencies:
 *  \n 1.25Hz, 5Hz, 20Hz, 40Hz
 *
 *  \n MPU6500:
 *  \n Unlike the MPU6050 version, the hardware does not "lock in" a reference
 *  sample. The hardware monitors the accel data and detects any large change
 *  over a short period of time.
 *
 *  \n The hardware motion threshold can be between 4mg and 1020mg in 4mg
 *  increments.
 *
 *  \n MPU6500 Low-power accel mode supports the following frequencies:
 *  \n 1.25Hz, 2.5Hz, 5Hz, 10Hz, 20Hz, 40Hz, 80Hz, 160Hz, 320Hz, 640Hz
 *
 *  \n\n NOTES:
 *  \n The driver will round down @e thresh to the nearest supported value if
 *  an unsupported threshold is selected.
 *  \n To select a fractional wake-up frequency, round down the value passed to
 *  @e lpa_freq.
 *  \n The MPU6500 does not support a delay parameter. If this function is used
 *  for the MPU6500, the value passed to @e time will be ignored.
 *  \n To disable this mode, set @e lpa_freq to zero. The driver will restore
 *  the previous configuration.
 *
 *  @param[in]  thresh      Motion threshold in mg.
 *  @param[in]  time        Duration in milliseconds that the accel data must
 *                          exceed @e thresh before motion is reported.
 *  @param[in]  lpa_freq    Minimum sampling rate, or zero to disable.
 *  @return     0 if successful.
 */
/*                    // commented because not use by oxs 
int mpu_lp_motion_interrupt(unsigned short thresh, unsigned char time,
    unsigned char lpa_freq)
{
    unsigned char data[3];

    if (lpa_freq) {
        unsigned char thresh_hw;


        // TODO: Make these const/#defines. 
        // 1LSb = 32mg. 
        if (thresh > 8160)
            thresh_hw = 255;
        else if (thresh < 32)
            thresh_hw = 1;
        else
            thresh_hw = thresh >> 5;

        if (!time)
            // Minimum duration must be 1ms. 
            time = 1;

        if (lpa_freq > 40)
            // At this point, the chip has not been re-configured, so the
            // function can safely exit.
             
            return -1;

        if (!chip_cfg.int_motion_only) {
            // Store current settings for later. 
            if (chip_cfg.dmp_on) {
                mpu_set_dmp_state(0);
                chip_cfg.cache.dmp_on = 1;
            } else
                chip_cfg.cache.dmp_on = 0;
            mpu_get_gyro_fsr(&chip_cfg.cache.gyro_fsr);
            mpu_get_accel_fsr(&chip_cfg.cache.accel_fsr);
            mpu_get_lpf(&chip_cfg.cache.lpf);
            mpu_get_sample_rate(&chip_cfg.cache.sample_rate);
            chip_cfg.cache.sensors_on = chip_cfg.sensors;
            mpu_get_fifo_config(&chip_cfg.cache.fifo_sensors);
        }

        // Disable hardware interrupts for now. 
        set_int_enable(0);

        // Enter full-power accel-only mode. 
        mpu_lp_accel_mode(0);

        // Override current LPF (and HPF) settings to obtain a valid accel
        //  reading.
         
        data[0] = INV_FILTER_256HZ_NOLPF2;
//        if (i2c_write( INV6050_addr, GYRO_REG_lpf, 1, data))
        if (i2c_writeBlock( GYRO_REG_lpf, 1, data))
            return -1;

        // NOTE: Digital high pass filter should be configured here. Since this
        // driver doesn't modify those bits anywhere, they should already be
        // cleared by default.
         

        // Configure the device to send motion interrupts. 
        // Enable motion interrupt. 
        data[0] = BIT_MOT_INT_EN;
//        if (i2c_write( INV6050_addr, GYRO_REG_int_enable, 1, data))
        if (i2c_writeBlock( GYRO_REG_int_enable, 1, data))
            goto lp_int_restore;

        // Set motion interrupt parameters. 
        data[0] = thresh_hw;
        data[1] = time;
//        if (i2c_write( INV6050_addr, GYRO_REG_motion_thr, 2, data))
        if (i2c_writeBlock( GYRO_REG_motion_thr, 2, data))
            goto lp_int_restore;

        // Force hardware to "lock" current accel sample. 
        delay(5);
        data[0] = (chip_cfg.accel_fsr << 3) | BITS_HPF;
//        if (i2c_write( INV6050_addr, GYRO_REG_accel_cfg, 1, data))
        if (i2c_writeBlock( GYRO_REG_accel_cfg, 1, data))
            goto lp_int_restore;

        // Set up LP accel mode. 
        data[0] = BIT_LPA_CYCLE;                                             // saving : we could fix the value being used
        if (lpa_freq == 1)
            data[1] = INV_LPA_1_25HZ;
        else if (lpa_freq <= 5)
            data[1] = INV_LPA_5HZ;
        else if (lpa_freq <= 20)
            data[1] = INV_LPA_20HZ;
        else
            data[1] = INV_LPA_40HZ;
        data[1] = (data[1] << 6) | BIT_STBY_XYZG;
//        if (i2c_write( INV6050_addr, GYRO_REG_pwr_mgmt_1, 2, data))
        if (i2c_writeBlock( GYRO_REG_pwr_mgmt_1, 2, data))
            goto lp_int_restore;

        chip_cfg.int_motion_only = 1;
        return 0;
    } else {
        // Don't "restore" the previous state if no state has been saved. 
        unsigned int ii;
        char *cache_ptr = (char*)&chip_cfg.cache;
        for (ii = 0; ii < sizeof(chip_cfg.cache); ii++) {
            if (cache_ptr[ii] != 0)
                goto lp_int_restore;
        }
        // If we reach this point, motion interrupt mode hasn't been used yet. 
        return -1;
    }
lp_int_restore:
    // Set to invalid values to ensure no I2C writes are skipped. 
    chip_cfg.gyro_fsr = 0xFF;
    chip_cfg.accel_fsr = 0xFF;
    chip_cfg.lpf = 0xFF;
    chip_cfg.sample_rate = 0xFFFF;
    chip_cfg.sensors = 0xFF;
    chip_cfg.fifo_enable = 0xFF;
    chip_cfg.clk_src = INV_CLK_PLL;
    mpu_set_sensors(chip_cfg.cache.sensors_on);
    mpu_set_gyro_fsr(chip_cfg.cache.gyro_fsr);
    mpu_set_accel_fsr(chip_cfg.cache.accel_fsr);
    mpu_set_lpf(chip_cfg.cache.lpf);
    mpu_set_sample_rate(chip_cfg.cache.sample_rate);
    mpu_configure_fifo(chip_cfg.cache.fifo_sensors);

    if (chip_cfg.cache.dmp_on)
        mpu_set_dmp_state(1);
        
    chip_cfg.int_motion_only = 0;
    return 0;
}
*/
/**
 *  @}
 */
