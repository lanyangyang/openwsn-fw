#ifndef BMP388_H
#define BMP388_H

#include "stdint.h"

// BMP388 I2C Address
#define BMP388_I2C_ADDR          0x76  

// REGISTER ADDRESSES
#define BMP388_REG_CHIP_ID       0x00
#define BMP388_REG_ERR_REG       0x02
#define BMP388_REG_STATUS        0x03
#define BMP388_REG_PRESSURE_DATA 0x04  // Pressure Data (0x04-0x06)
#define BMP388_REG_TEMPERATURE_DATA 0x07  // Temperature Data (0x07-0x09)
#define BMP388_REG_SENSORTIME    0x0C  
#define BMP388_REG_EVENT         0x10
#define BMP388_REG_INT_STATUS    0x11
#define BMP388_REG_FIFO_LENGTH   0x12  
#define BMP388_REG_FIFO_DATA     0x14
#define BMP388_REG_FIFO_WTM_0    0x15  
#define BMP388_REG_FIFO_CONFIG_1 0x17
#define BMP388_REG_FIFO_CONFIG_2 0x18
#define BMP388_REG_INT_CTRL      0x19
#define BMP388_REG_IF_CONF       0x1A
#define BMP388_REG_PWR_CTRL      0x1B
#define BMP388_REG_OSR           0x1C  // Oversampling Register
#define BMP388_REG_ODR           0x1D  // Output Data Rate
#define BMP388_REG_CONFIG        0x1F
#define BMP388_REG_CALIB_DATA    0x31  // Calibration Data (0x31-0x5F)
#define BMP388_REG_CMD           0x7E

// System Functions
uint8_t bmp388_who_am_i(void);
void bmp388_reset(void);
void bmp388_init(void);
void bmp388_is_cmd_ready(void);

// Setting Functions
void bmp388_set_power_mode(uint8_t mode);
void bmp388_set_iir_filter_coefficient(uint8_t coefficient);
void bmp388_set_oversampling(uint8_t osr_t, uint8_t osr_p);  // Combined temperature and pressure oversampling

// Calibration Data Functions
void bmp388_read_calibration_data(void);

// Sensor Data Functions
void bmp388_read_pressure_temperature(double *Pressure, double *Temperature);
uint32_t bmp388_read_sensor_time(void);

#endif // BMP388_H
