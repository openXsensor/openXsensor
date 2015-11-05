/* =============================================================================
Nav6 source code is placed under the MIT license

Copyright (c) 2013 Kauai Labs

Portions of this work are based upon the I2C Dev Library by Jeff Rowberg
(www.i2cdevlib.com) which is open-source licensed under the MIT
License.  This work is also based upon the Arduino software
library which is licensed under a Creative Commons license.

This work also incorporates a version of the official Invensense Motion
Driver v. 5.1 library, which has been ported by Kauai Labs to the Arduino
libraries.  This library has a separate license.  The nav6 MIT license does
not extended to the Motion Driver 5.1 Library; anyone intending to modify
this library source code should contact Invensense and register as a 
developer at their website.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=============================================================================
*/

#define EMPL_TARGET_ATMEGA328
//#include <Wire.h>
//#include "I2Cdev.h"
#include "helper_3dmath.h"
extern "C" {
  #include "inv_mpu.h"
  #include "inv_mpu_dmp_motion_driver.h"
}
#include "oXs_imu.h"

void setup_imu() {
    // MPU-6050 Initialization
    // Gyro sensitivity:      2000 degrees/sec
    // Accel sensitivity:     2 g
    // Gyro Low-pass filter:  42Hz
    // DMP Update rate:       100Hz
#ifdef DEBUG
    Serial.print(F("Initializing MPU..."));
#endif
    boolean mpu_initialized = false;
    while ( !mpu_initialized ) {
      if ( initialize_mpu() ) {
        mpu_initialized = true;
#ifdef DEBUG        
        Serial.print(F("Success"));
#endif        
        //boolean gyro_ok, accel_ok;
        //run_mpu_self_test(gyro_ok,accel_ok);
        enable_mpu();    
      }
      else {
#ifdef DEBUG        
        Serial.print(F("Failed"));    
#endif        
 //       mpu_force_reset();
        delay(100);
#ifdef DEBUG
        Serial.println(F("Re-initializing"));
#endif        
      }
    }
#ifdef DEBUG
    Serial.println(F("Initialization Complete"));
#endif
}



/*****************************************
* Conversion Factors
*****************************************/
// angle in radians = angle in degrees * Pi / 180 
const float degrees_to_radians = M_PI / 180.0;
// angle in degrees = angle in radians * 180 / Pi
const float radians_to_degrees = 180.0 / M_PI;

/*****************************************
* MPU Calibration
*****************************************/
#define STARTUP_CALIBRATION_DELAY_MS        19000
#define CALIBRATED_OFFSET_AVERAGE_PERIOD_MS  1000

#define NAV6_CALIBRATION_STATE_WAIT 0 // Waiting for MPU to complete internal calibration
#define NAV6_CALIBRATION_STATE_ACCUMULATE  1 // Accumulate Yaw/Pitch/Roll offsets
#define NAV6_CALIBRATION_STATE_COMPLETE    2 // Normal Operation

int calibration_state = NAV6_CALIBRATION_STATE_WAIT;
int calibration_accumulator_count = 0;
float yaw_accumulator = 0.0;
float quaternion_accumulator[4] = { 0.0, 0.0, 0.0, 0.0 };
float calibrated_yaw_offset = 0.0;
float calibrated_quaternion_offset[4] = { 0.0, 0.0, 0.0, 0.0 }; 

/******************************************
* Magnetometer State
******************************************/
int16_t mag_x = 0;
int16_t mag_y = 0;
int16_t mag_z = 0;
float compass_heading_radians = 0.0;
float compass_heading_degrees = 0.0;

/****************************************
* Gyro/Accel/DMP State
****************************************/
float temp_centigrade = 0.0;  // Gyro/Accel die temperature
float ypr[3] = { 0, 0, 0 };
long curr_mpu_temp;
unsigned long sensor_timestamp;

struct FloatVectorStruct {
  float x;
  float y;
  float z;
};

struct FloatVectorStruct gravity;

/***************************************
* Invensense Hardware Abstracation Layer
***************************************/
struct hal_s {
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
};

static struct hal_s hal = {0};

#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
/* Starting sampling rate. */
#define DEFAULT_MPU_HZ    (100)
#define MAX_NAV6_MPU_RATE (100)
#define MIN_NAV6_MPU_RATE (4)
/* Data requested by client. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)

/****************************************
* Gyro/Accel/DMP Configuration
****************************************/
unsigned char accel_fsr;  // accelerometer full-scale rate, in +/- Gs (possible values are 2, 4, 8 or 16).  Default:  2
unsigned short dmp_update_rate; // update rate, in hZ (possible values are between 4 and 1000).  Default:  100
unsigned short gyro_fsr;  // Gyro full-scale_rate, in +/- degrees/sec, possible values are 250, 500, 1000 or 2000.  Default:  2000

/* The mounting matrix below tells the MPL how to rotate the raw 
 * data from the driver(s). The matrix below reflects the axis
 * orientation of the MPU-6050 on the nav6 circuit board.
 */
static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1};

// ******************************************************************************************
//                              Read imu 6050 sensor
// ******************************************************************************************
void read6050 () {
  // If the MPU Interrupt occurred, read the fifo and process the data
  
  if (hal.new_gyro && hal.dmp_on) {

        short gyro[3], accel[3], sensors;
        unsigned char more = 0;
        long quat[4];
        //float euler[3];
        /* This function gets new data from the FIFO when the DMP is in
         * use. The FIFO can contain any combination of gyro, accel,
         * quaternion, and gesture data. The sensors parameter tells the
         * caller which data fields were actually populated with new data.
         * For example, if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT), then
         * the FIFO isn't being filled with accel data.
         * The driver parses the gesture data to determine if a gesture
         * event has occurred; on an event, the application will be notified
         * via a callback (assuming that a callback function was properly
         * registered). The more parameter is non-zero if there are
         * leftover packets in the FIFO.
         */
        int success = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
        if (!more)                  // if no more data
            hal.new_gyro = 0;       // reset the indicator saying that data are available in the FIFO, so it will be updated by the callback function on the interrupt.
        
        if ( ( success == 0 ) && ( (sensors & INV_XYZ_ACCEL) != 0 ) && ( (sensors & INV_WXYZ_QUAT) != 0 ) ) {
              Quaternion q( (float)(quat[0] >> 16) / 16384.0f,
                            (float)(quat[1] >> 16) / 16384.0f,
                            (float)(quat[2] >> 16) / 16384.0f,
                            (float)(quat[3] >> 16) / 16384.0f);  
        
             // Calculate Yaw/Pitch/Roll
             // Update client with yaw/pitch/roll and tilt-compensated magnetometer data
             getGravity(&gravity, &q);
             dmpGetYawPitchRoll(ypr, &q, &gravity);
              float linear_acceleration_x;
              float linear_acceleration_y;
              float linear_acceleration_z;
              float q1[4];
              float q2[4];
              float q_product[4];
              float q_conjugate[4];
              float q_final[4];               
              float world_linear_acceleration_x;
              float world_linear_acceleration_y;
              float world_linear_acceleration_z;
    
              // calculate linear acceleration by 
              // removing the gravity component from raw acceleration values
               
              linear_acceleration_x = (((float)accel[0]) / (32768.0 / accel_fsr)) - gravity.x;
              linear_acceleration_y = (((float)accel[1]) / (32768.0 / accel_fsr)) - gravity.y;
              linear_acceleration_z = (((float)accel[2]) / (32768.0 / accel_fsr)) - gravity.z; 
              
              // Calculate world-frame acceleration
              
              q1[0] = quat[0] >> 16;
              q1[1] = quat[1] >> 16;
              q1[2] = quat[2] >> 16;
              q1[3] = quat[3] >> 16;
              
              q2[0] = 0;
              q2[1] = linear_acceleration_x;
              q2[2] = linear_acceleration_y;
              q2[3] = linear_acceleration_z;
              
              // Rotate linear acceleration so that it's relative to the world reference frame
              
              // http://www.cprogramming.com/tutorial/3d/quaternions.html
              // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
              // http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
              // ^ or: http://webcache.googleusercontent.com/search?q=cache:xgJAp3bDNhQJ:content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation&hl=en&gl=us&strip=1
          
              // P_out = q * P_in * conj(q)
              // - P_out is the output vector
              // - q is the orientation quaternion
              // - P_in is the input vector (a*aReal)
              // - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])
      
              // calculate quaternion product
              // Quaternion multiplication is defined by:
              //     (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
              //     (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
              //     (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
              //     (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
              
              q_product[0] = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];  // new w
              q_product[1] = q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2];  // new x
              q_product[2] = q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1];  // new y 
              q_product[3] = q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[0];  // new z
      
              q_conjugate[0] = q1[0];            
              q_conjugate[1] = -q1[1];            
              q_conjugate[2] = -q1[2];            
              q_conjugate[3] = -q1[3];            
      
              q_final[0] = q_product[0]*q_conjugate[0] - q_product[1]*q_conjugate[1] - q_product[2]*q_conjugate[2] - q_product[3]*q_conjugate[3];  // new w
              q_final[1] = q_product[0]*q_conjugate[1] + q_product[1]*q_conjugate[0] + q_product[2]*q_conjugate[3] - q_product[3]*q_conjugate[2];  // new x
              q_final[2] = q_product[0]*q_conjugate[2] - q_product[1]*q_conjugate[3] + q_product[2]*q_conjugate[0] + q_product[3]*q_conjugate[1];  // new y 
              q_final[3] = q_product[0]*q_conjugate[3] + q_product[1]*q_conjugate[2] - q_product[2]*q_conjugate[1] + q_product[3]*q_conjugate[0];  // new z
      
              world_linear_acceleration_x = q_final[1];
              world_linear_acceleration_y = q_final[2];
              world_linear_acceleration_z = q_final[3];
          
      }
    }   // end  (hal.new_gyro && hal.dmp_on)
}
/***************************************
* nav6 Protocol Configuration/State
***************************************/

//boolean update_type = MSGID_YPR_UPDATE;
//char protocol_buffer[64];

void loop_xxxxxxxx() {
  
  // If the MPU Interrupt occurred, read the fifo and process the data
  
  if (hal.new_gyro && hal.dmp_on) {

    short gyro[3], accel[3], sensors;
    unsigned char more = 0;
    long quat[4];
    //float euler[3];
    /* This function gets new data from the FIFO when the DMP is in
     * use. The FIFO can contain any combination of gyro, accel,
     * quaternion, and gesture data. The sensors parameter tells the
     * caller which data fields were actually populated with new data.
     * For example, if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT), then
     * the FIFO isn't being filled with accel data.
     * The driver parses the gesture data to determine if a gesture
     * event has occurred; on an event, the application will be notified
     * via a callback (assuming that a callback function was properly
     * registered). The more parameter is non-zero if there are
     * leftover packets in the FIFO.
     */
    int success = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,
        &more);
    if (!more)
        hal.new_gyro = 0;
    
    if ( ( success == 0 ) && ( (sensors & INV_XYZ_ACCEL) != 0 ) && ( (sensors & INV_WXYZ_QUAT) != 0 ) ) {
      
      Quaternion q( (float)(quat[0] >> 16) / 16384.0f,
                    (float)(quat[1] >> 16) / 16384.0f,
                    (float)(quat[2] >> 16) / 16384.0f,
                    (float)(quat[3] >> 16) / 16384.0f);  

      // Calculate Yaw/Pitch/Roll
      // Update client with yaw/pitch/roll and tilt-compensated magnetometer data
      
      getGravity(&gravity, &q);
      dmpGetYawPitchRoll(ypr, &q, &gravity);
            
      boolean accumulate = false;
      if ( calibration_state == NAV6_CALIBRATION_STATE_WAIT ) {
        
        if ( millis() >= STARTUP_CALIBRATION_DELAY_MS ) {
          
          calibration_state = NAV6_CALIBRATION_STATE_ACCUMULATE;
        }
        
      }
/*      
      if ( calibration_state == NAV6_CALIBRATION_STATE_ACCUMULATE ) {
        
        accumulate = true;
        if ( millis() >= (STARTUP_CALIBRATION_DELAY_MS + CALIBRATED_OFFSET_AVERAGE_PERIOD_MS) ) {
          
          accumulate = false;
          calibrated_yaw_offset = yaw_accumulator / calibration_accumulator_count;
          calibrated_quaternion_offset[0] = quaternion_accumulator[0] / calibration_accumulator_count;
          calibrated_quaternion_offset[1] = quaternion_accumulator[1] / calibration_accumulator_count;
          calibrated_quaternion_offset[2] = quaternion_accumulator[2] / calibration_accumulator_count;
          calibrated_quaternion_offset[3] = quaternion_accumulator[3] / calibration_accumulator_count;
          calibration_state = NAV6_CALIBRATION_STATE_COMPLETE;
          
          // Since calibration data has likely changed, send an update
          
          int num_bytes = IMUProtocol::encodeStreamResponse(  protocol_buffer, update_type,
                                                                gyro_fsr, accel_fsr, dmp_update_rate, calibrated_yaw_offset, 
                                                                (uint16_t)(calibrated_quaternion_offset[0] * 16384),
                                                                (uint16_t)(calibrated_quaternion_offset[1] * 16384),
                                                                (uint16_t)(calibrated_quaternion_offset[2] * 16384),
                                                                (uint16_t)(calibrated_quaternion_offset[3] * 16384),
                                                                calibration_state );
          Serial.write((unsigned char *)protocol_buffer, num_bytes);         
        }
        else {
          
          calibration_accumulator_count++;
        
        }
      }
    
      float x = ypr[0] * radians_to_degrees;
      float y = ypr[1] * radians_to_degrees;
      float z = ypr[2] * radians_to_degrees;
  
      if ( accumulate ) {
        
        yaw_accumulator += x;
        quaternion_accumulator[0] += q.w;
        quaternion_accumulator[1] += q.x;
        quaternion_accumulator[2] += q.y;
        quaternion_accumulator[3] += q.z;
      
      }    
      if ( update_type == MSGID_QUATERNION_UPDATE ) {
        
        // Update client with quaternions and some raw sensor data
        
        int num_bytes = IMUProtocol::encodeQuaternionUpdate(  protocol_buffer, 
                                                               quat[0] >> 16, quat[1] >> 16, quat[2] >> 16, quat[3] >> 16,
                                                               accel[0], accel[1], accel[2],
                                                               mag_x, mag_y, mag_z,
                                                               temp_centigrade);     
        Serial.write((unsigned char *)protocol_buffer, num_bytes);
      }
      else if ( update_type == MSGID_GYRO_UPDATE ) {

        // Update client with raw sensor data only
        
        int num_bytes = IMUProtocol::encodeGyroUpdate(  protocol_buffer, 
                                                       gyro[0], gyro[1], gyro[2],
                                                       accel[0], accel[1], accel[2],
                                                       mag_x, mag_y, mag_z,
                                                       temp_centigrade);     
        Serial.write((unsigned char *)protocol_buffer, num_bytes);

      }
      else {
        
        // Send a Yaw/Pitch/Roll/Heading update
        
        x -= calibrated_yaw_offset;
    
        if ( x < -180 ) x += 360;
        if ( x > 180 ) x -= 360;
        if ( y < -180 ) y += 360;
        if ( y > 180 ) y -= 360;
        if ( z < -180 ) z += 360;
        if ( z > 180 ) z -= 360;

	float linear_acceleration_x;
	float linear_acceleration_y;
	float linear_acceleration_z;
        float q1[4];
        float q2[4];
        float q_product[4];
        float q_conjugate[4];
        float q_final[4];               
	float world_linear_acceleration_x;
	float world_linear_acceleration_y;
	float world_linear_acceleration_z;
    
        // calculate linear acceleration by 
        // removing the gravity component from raw acceleration values
         
        linear_acceleration_x = (((float)accel[0]) / (32768.0 / accel_fsr)) - gravity.x;
        linear_acceleration_y = (((float)accel[1]) / (32768.0 / accel_fsr)) - gravity.y;
        linear_acceleration_z = (((float)accel[2]) / (32768.0 / accel_fsr)) - gravity.z; 
        
        // Calculate world-frame acceleration
        
        q1[0] = quat[0] >> 16;
        q1[1] = quat[1] >> 16;
        q1[2] = quat[2] >> 16;
        q1[3] = quat[3] >> 16;
        
        q2[0] = 0;
        q2[1] = linear_acceleration_x;
        q2[2] = linear_acceleration_y;
        q2[3] = linear_acceleration_z;
        
        // Rotate linear acceleration so that it's relative to the world reference frame
        
        // http://www.cprogramming.com/tutorial/3d/quaternions.html
        // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
        // http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
        // ^ or: http://webcache.googleusercontent.com/search?q=cache:xgJAp3bDNhQJ:content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation&hl=en&gl=us&strip=1
    
        // P_out = q * P_in * conj(q)
        // - P_out is the output vector
        // - q is the orientation quaternion
        // - P_in is the input vector (a*aReal)
        // - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])

        // calculate quaternion product
        // Quaternion multiplication is defined by:
        //     (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
        //     (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
        //     (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
        //     (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
        
        q_product[0] = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];  // new w
        q_product[1] = q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2];  // new x
        q_product[2] = q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1];  // new y 
        q_product[3] = q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[0];  // new z

        q_conjugate[0] = q1[0];            
        q_conjugate[1] = -q1[1];            
        q_conjugate[2] = -q1[2];            
        q_conjugate[3] = -q1[3];            

        q_final[0] = q_product[0]*q_conjugate[0] - q_product[1]*q_conjugate[1] - q_product[2]*q_conjugate[2] - q_product[3]*q_conjugate[3];  // new w
        q_final[1] = q_product[0]*q_conjugate[1] + q_product[1]*q_conjugate[0] + q_product[2]*q_conjugate[3] - q_product[3]*q_conjugate[2];  // new x
        q_final[2] = q_product[0]*q_conjugate[2] - q_product[1]*q_conjugate[3] + q_product[2]*q_conjugate[0] + q_product[3]*q_conjugate[1];  // new y 
        q_final[3] = q_product[0]*q_conjugate[3] + q_product[1]*q_conjugate[2] - q_product[2]*q_conjugate[1] + q_product[3]*q_conjugate[0];  // new z

        world_linear_acceleration_x = q_final[1];
        world_linear_acceleration_y = q_final[2];
        world_linear_acceleration_z = q_final[3];
    
        int num_bytes = IMUProtocol::encodeYPRUpdate(protocol_buffer, x, y, z,compass_heading_degrees);     
        Serial.write((unsigned char *)protocol_buffer, num_bytes);
      }
    }
    else {
      
    }
  }
  
  // If any serial bytes are received, scan to see if a start
  // of message has been received.  Remove any bytes that precede
  // the start of a message.
 
  bool found_start_of_message = false;
  while ( Serial.available() > 0 ) {
    char rcv_byte = Serial.peek();
    if ( rcv_byte != PACKET_START_CHAR ) {
      Serial.read();
    }
    else {
      delay(2);
      found_start_of_message = true;
      break;
    }
  }
  
  // If sufficient bytes have been received, process the data and
  // if a valid message is received, handle it.
  
  boolean send_stream_response = false;
  if( found_start_of_message && ( Serial.available() >= STREAM_CMD_MESSAGE_LENGTH ) ) {
    int bytes_read = 0;
    while ( Serial.available() ) {
      if ( bytes_read >= sizeof(protocol_buffer) ) {
        break;
      }
      protocol_buffer[bytes_read++] = Serial.read();
    }
    int i = 0;
    // Scan the buffer looking for valid packets
    while ( i < bytes_read )
    {
      int bytes_remaining = bytes_read - i;
      char stream_type;
      unsigned char update_rate_hz;
      int packet_length = IMUProtocol::decodeStreamCommand( &protocol_buffer[i], bytes_remaining, stream_type, update_rate_hz ); 
      if ( packet_length > 0 )
      {
        send_stream_response = true;
        update_type = stream_type;
        
        // if update rate has changed, reconfigure the MPU w/the new rate.
        
        if ( update_rate_hz != dmp_update_rate ) {
          if ( update_rate_hz > MAX_NAV6_MPU_RATE ) {
            update_rate_hz = MAX_NAV6_MPU_RATE;
          }
          else if ( update_rate_hz < MIN_NAV6_MPU_RATE ) {
            update_rate_hz = MIN_NAV6_MPU_RATE;
          }
          disable_mpu();
          mpu_set_sample_rate(update_rate_hz);
          dmp_set_fifo_rate(update_rate_hz);
          // Read back configuration in case it was set improperly. 
          mpu_get_sample_rate(&dmp_update_rate);          
          enable_mpu();
        }
        
        i += packet_length;
      }
      else // current index is not the start of a valid packet; increment
      {
        i++;
      }
  */
    }
  
  }
 /* 
  if ( send_stream_response ) {
        int num_bytes = IMUProtocol::encodeStreamResponse(  protocol_buffer, update_type,
                                                              gyro_fsr, accel_fsr, dmp_update_rate, calibrated_yaw_offset, 
                                                              (uint16_t)(calibrated_quaternion_offset[0] * 16384),
                                                              (uint16_t)(calibrated_quaternion_offset[1] * 16384),
                                                              (uint16_t)(calibrated_quaternion_offset[2] * 16384),
                                                              (uint16_t)(calibrated_quaternion_offset[3] * 16384),
                                                              calibration_state );
      Serial.write((unsigned char *)protocol_buffer, num_bytes);
  }
*/  
}

/* Every time new gyro data is available, this function is called in an
 * ISR context. In this example, it sets a flag protecting the FIFO read
 * function.
 */
void gyro_data_ready_cb(void) {
  
    hal.new_gyro = 1;
}

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
unsigned short inv_row_2_scale(const signed char *row) {
    unsigned short b;
    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}


unsigned short inv_orientation_matrix_to_scalar( const signed char *mtx) {
    unsigned short scalar;
    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

boolean initialize_mpu() {
  
    int result;
    struct int_param_s int_param;

    /* Set up gyro.
     * Every function preceded by mpu_ is a driver function and can be found
     * in inv_mpu.h.
     */
    int_param.cb = gyro_data_ready_cb;
    int_param.pin = 0;
    result = mpu_init(&int_param);

    if ( result != 0 ) {
      Serial.print("mpu_init failed!");
      return false;
    }

    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    /* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&dmp_update_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);

    /* Initialize HAL state variables. */
    memset(&hal, 0, sizeof(hal));
    hal.sensors = ACCEL_ON | GYRO_ON;
    hal.report = PRINT_QUAT;

    /* To initialize the DMP:
     * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
     *    inv_mpu_dmp_motion_driver.h into the MPU memory.
     * 2. Push the gyro and accel orientation matrix to the DMP.
     * 3. Register gesture callbacks. Don't worry, these callbacks won't be
     *    executed unless the corresponding feature is enabled.
     * 4. Call dmp_enable_feature(mask) to enable different features.
     * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
     * 6. Call any feature-specific control functions.
     *
     * To enable the DMP, just call mpu_set_dmp_state(1). This function can
     * be called repeatedly to enable and disable the DMP at runtime.
     *
     * The following is a short summary of the features supported in the DMP
     * image provided in inv_mpu_dmp_motion_driver.c:
     * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
     * 200Hz. Integrating the gyro data at higher rates reduces numerical
     * errors (compared to integration on the MCU at a lower sampling rate).
     * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
     * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
     * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
     * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
     * an event at the four orientations where the screen should rotate.
     * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
     * no motion.
     * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
     * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
     * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
     * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
     */
    result = dmp_load_motion_driver_firmware();
    if ( result != 0 ) {
      Serial.print("Firmware Load ERROR ");
      Serial.println(result);
      return false;
    }
    dmp_set_orientation( inv_orientation_matrix_to_scalar(gyro_orientation));
    
    unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL |  DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    return true;
}

void disable_mpu() {
    mpu_set_dmp_state(0);
    hal.dmp_on = 0;
}

void enable_mpu() {

    mpu_set_dmp_state(1);  // This enables the DMP; at this point, interrupts should commence
    hal.dmp_on = 1;
}  

boolean run_mpu_self_test(boolean& gyro_ok, boolean& accel_ok) {
  
    int result;
    long gyro[3], accel[3];
    boolean success = false;

    gyro_ok = false;
    accel_ok = false;
    result = mpu_run_self_test(gyro, accel);
    if ( ( result & 0x1 ) != 0 ) {
      // Gyro passed self test
      gyro_ok = true;
      float sens;
      mpu_get_gyro_sens(&sens);
      gyro[0] = (long)(gyro[0] * sens);
      gyro[1] = (long)(gyro[1] * sens);
      gyro[2] = (long)(gyro[2] * sens);
      dmp_set_gyro_bias(gyro);
    }
    if ( ( result & 0x2 ) != 0 ) {
      // Accelerometer passed self test
      accel_ok = true;
      unsigned short accel_sens;
      mpu_get_accel_sens(&accel_sens);
      accel[0] *= accel_sens;
      accel[1] *= accel_sens;
      accel[2] *= accel_sens;
      dmp_set_accel_bias(accel);
    }

    success = gyro_ok && accel_ok;
  
    return success;
}

void getEuler(float *data, Quaternion *q) {

    data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);   // psi
    data[1] = -asin(2*q -> x*q -> z + 2*q -> w*q -> y);                              // theta
    data[2] = atan2(2*q -> y*q -> z - 2*q -> w*q -> x, 2*q -> w*q -> w + 2*q -> z*q -> z - 1);   // phi
}

void getGravity(struct FloatVectorStruct *v, Quaternion *q) {

    v -> x = 2 * (q -> x*q -> z - q -> w*q -> y);
    v -> y = 2 * (q -> w*q -> x + q -> y*q -> z);
    v -> z = q -> w*q -> w - q -> x*q -> x - q -> y*q -> y + q -> z*q -> z;
}

void dmpGetYawPitchRoll(float *data, Quaternion *q, struct FloatVectorStruct *gravity) {
  
    // yaw: (about Z axis)
    data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);
    // pitch: (nose up/down, about Y axis)
    data[1] = atan(gravity -> x / sqrt(gravity -> y*gravity -> y + gravity -> z*gravity -> z));
    // roll: (tilt left/right, about X axis)
    data[2] = atan(gravity -> y / sqrt(gravity -> x*gravity -> x + gravity -> z*gravity -> z));
}

// The following code calculates the amount of free memory.

extern unsigned int __heap_start;
extern void *__brkval;
 
/*
  * The free list structure as maintained by the 
 * avr-libc memory allocation routines.
  */
struct __freelist {
   size_t sz;
   struct __freelist *nx;
};
 
/* The head of the free list structure */
extern struct __freelist *__flp;
 
/* Calculates the size of the free list */
int freeListSize() {
  
   struct __freelist* current;
   int total = 0;
 
  for (current = __flp; current; current = current->nx) {
     total += 2; /* Add two bytes for the memory block's header  */
     total += (int) current->sz;
   }
 
  return total;
}
 
int freeMemory()  {
   int free_memory;
 
  if ((int)__brkval == 0) {
     free_memory = ((int)&free_memory) - ((int)&__heap_start);
   } else {
     free_memory = ((int)&free_memory) - ((int)__brkval);
     free_memory += freeListSize();
   }
   return free_memory;
}

