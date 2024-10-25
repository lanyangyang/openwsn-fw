//This is the simple driver file of BMP388
//It is inspired form https://github.com/libdriver/bmp388/
//Modified to fit nrf52840-dk board

#include "bmp388.h"
#include "i2c.h"
#include "leds.h"
//calibration data cube
//used for data compensation
//data format is form BMP388 handbook
struct BMP388_Calib_Data_Strcut
{
	double  PAR_T[3];//3 for tempreture
	double  PAR_P[11];//11 for pressure
};
struct BMP388_Calib_Data_Strcut BMP388_Calib_Data;


//================INIT================//
//find chip id
uint8_t bmp388_who_am_i(void) {
    uint8_t chip_id;
    //i2c_set_addr(BMP388_I2C_ADDR);
    i2c_read_bytes(BMP388_REG_CHIP_ID, &chip_id, 1);
    return chip_id;
}

//Reset function
void bmp388_reset(void) {
    uint8_t cmd = 0xB6;  //softreset
    //i2c_set_addr(BMP388_I2C_ADDR);
    i2c_write_bytes(BMP388_REG_CMD, &cmd, 1);
}

//check sensor status
void bmp388_Waiting_For_OK()
{
	uint8_t I2C_Receive_Data[1];
	i2c_read_bytes(BMP388_REG_STATUS, I2C_Receive_Data, 1);
	while(1)
	{
		uint8_t Temperature_Status=(I2C_Receive_Data[0]&0x40)>>6;
		uint8_t  Pressure_Status=(I2C_Receive_Data[0]&0x20)>>5;
		if(Temperature_Status&&Pressure_Status)
		{
			break;
		}
	}
 
}

//choose power mode
void bmp388_set_power_mode(uint8_t mode) {
    uint8_t pwr_ctrl;
    if (mode == 1) {
        // force mode
        pwr_ctrl = 0x10;
    } else if (mode == 2) {
        // normal mode
        pwr_ctrl = 0x33;
    }else{
        // sleep mode
        pwr_ctrl = 0x00;
    }

    i2c_write_bytes(BMP388_REG_PWR_CTRL, &pwr_ctrl, 1);
}

//set filter coeff
void bmp388_set_iir_filter_coefficient(uint8_t coefficient) {
    uint8_t config;
    config = 0;  //clear filter value
    
    uint8_t filter_setting;
    //choose coefficient
    switch (coefficient) {
        case 0:
            filter_setting = 0x00;
            break;
        case 1:
            filter_setting = 0x02;
            break;
        case 3:
            filter_setting = 0x04;
            break;
        case 7:
            filter_setting = 0x06;
            break;
        case 15:
            filter_setting = 0x08;
            break;
        case 31:
            filter_setting = 0x0A;
            break;
        case 63:
            filter_setting = 0x0C;
            break;
        case 127:
            filter_setting = 0x0E;
            break;
        default:
            filter_setting = 0x00;
            break;
    }
    
    config = filter_setting;  //set the filter value
    i2c_write_bytes(BMP388_REG_CONFIG, &config, 1);
}

void bmp388_set_oversampling(uint8_t osr_t, uint8_t osr_p) {
    uint8_t osr_reg_value = 0x00; // Clear register value

    // Determine oversampling setting for temperature (Bits 5..3)
    switch (osr_t) {
        case 1:
            osr_reg_value |= (0x00 << 3);  // x1 (no oversampling)
            break;
        case 2:
            osr_reg_value |= (0x01 << 3);  // x2 oversampling
            break;
        case 4:
            osr_reg_value |= (0x02 << 3);  // x4 oversampling
            break;
        case 8:
            osr_reg_value |= (0x03 << 3);  // x8 oversampling
            break;
        case 16:
            osr_reg_value |= (0x04 << 3);  // x16 oversampling
            break;
        case 32:
            osr_reg_value |= (0x05 << 3);  // x32 oversampling
            break;
        default:
            osr_reg_value |= (0x00 << 3);  // Default: no oversampling
            break;
    }

    // Determine oversampling setting for pressure (Bits 2..0)
    switch (osr_p) {
        case 1:
            osr_reg_value |= 0x00;  // x1 (no oversampling)
            break;
        case 2:
            osr_reg_value |= 0x01;  // x2 oversampling
            break;
        case 4:
            osr_reg_value |= 0x02;  // x4 oversampling
            break;
        case 8:
            osr_reg_value |= 0x03;  // x8 oversampling
            break;
        case 16:
            osr_reg_value |= 0x04;  // x16 oversampling
            break;
        case 32:
            osr_reg_value |= 0x05;  // x32 oversampling
            break;
        default:
            osr_reg_value |= 0x00;  // Default: no oversampling
            break;
    }

    // Write the combined oversampling value to the OSR register (0x1C)
    i2c_write_bytes(BMP388_REG_OSR, &osr_reg_value, 1);
}

// set data rate
void bmp388_Set_Data_Rate()
{
	  uint8_t I2C_Transmit_Data;
	  I2C_Transmit_Data=0x02;
	  i2c_write_bytes(BMP388_REG_ODR, &I2C_Transmit_Data, 1);
 
}


// get compensation form bmp388
void bmp388_Get_Compensation()
{
	uint8_t I2C_Receive_Data[21];
	i2c_read_bytes(BMP388_REG_CALIB_DATA, I2C_Receive_Data, 21);
	BMP388_Calib_Data.PAR_T[0]=((I2C_Receive_Data[1]<<8)|I2C_Receive_Data[0])/pow(2,-8);
	BMP388_Calib_Data.PAR_T[1]=((I2C_Receive_Data[3]<<8)|I2C_Receive_Data[2])/pow(2,30);
	BMP388_Calib_Data.PAR_T[2]=(int8_t)I2C_Receive_Data[4]/pow(2,48);
 
	BMP388_Calib_Data.PAR_P[0]=(((int16_t)((I2C_Receive_Data[6]<<8)|I2C_Receive_Data[5]))-pow(2,14))/pow(2,20);
	BMP388_Calib_Data.PAR_P[1]=(((int16_t)((I2C_Receive_Data[8]<<8)|I2C_Receive_Data[7]))-pow(2,14))/pow(2,29);
	BMP388_Calib_Data.PAR_P[2]=(int8_t)I2C_Receive_Data[9]/pow(2,32);
	BMP388_Calib_Data.PAR_P[3]=(int8_t)I2C_Receive_Data[10]/pow(2,37);
	BMP388_Calib_Data.PAR_P[4]=((I2C_Receive_Data[12]<<8)|I2C_Receive_Data[11])/pow(2,-3);
	BMP388_Calib_Data.PAR_P[5]=((I2C_Receive_Data[14]<<8)|I2C_Receive_Data[13])/pow(2,6);
	BMP388_Calib_Data.PAR_P[6]=(int8_t)I2C_Receive_Data[15]/pow(2,8);
	BMP388_Calib_Data.PAR_P[7]=(int8_t)I2C_Receive_Data[16]/pow(2,15);
	BMP388_Calib_Data.PAR_P[8]=(int16_t)((I2C_Receive_Data[18]<<8)|I2C_Receive_Data[17])/pow(2,48);
	BMP388_Calib_Data.PAR_P[9]=(int8_t)I2C_Receive_Data[19]/pow(2,48);
	BMP388_Calib_Data.PAR_P[10]=(int8_t)I2C_Receive_Data[20]/pow(2,65);
}

//Initialize BMP388 sensor
void bmp388_init(void) {

    bmp388_reset();  //Reset the sensor
    bmp388_set_oversampling(4,4);  //Set oversampling
    bmp388_set_iir_filter_coefficient(3);  //Set IIR filter coefficient
    bmp388_Set_Data_Rate();//Set output data rate (ODR) to 200Hz (0x00 corresponds to 5ms)
    bmp388_set_power_mode(2);  //Set power mode to normal
    bmp388_read_calibration_data();
}
 

//====================DATA PROCESS===================//


uint32_t bmp388_read_sensor_time(void) {
    uint8_t time_raw[3];
    uint8_t reg_address = BMP388_REG_SENSORTIME;
    //i2c_set_addr(BMP388_I2C_ADDR);
    i2c_read_bytes(BMP388_REG_SENSORTIME, time_raw, 3);
    uint32_t sensor_time;
    sensor_time |=  ((uint32_t)time_raw[2] << 16);
    sensor_time |=  ((uint32_t)time_raw[1] << 8);
    sensor_time |=  (uint32_t)time_raw[0];
    return sensor_time;
}

//================COMPENSATE==============//
//Compensate functions are from BOSCH BMP388 CSDN tutorial
void bmp388_Read_Pressure_Temperature(double *Pressure,double *Temperature)
{
 
	uint8_t I2C_Receive_Data[6];
 
	bmp388_Waiting_For_OK();
	i2c_read_bytes(BMP388_REG_PRESSURE_DATA, I2C_Receive_Data, 6);
	uint32_t Pressure_Integer=(I2C_Receive_Data[2]<<16)|(I2C_Receive_Data[1]<<8)|I2C_Receive_Data[0];
	uint32_t Temperature_Integer=(I2C_Receive_Data[5]<<16)|(I2C_Receive_Data[4]<<8)|I2C_Receive_Data[3];
 
	/*******COMPENSATION******/
	double T_partial_data1=(double)(Temperature_Integer-BMP388_Calib_Data.PAR_T[0]);
	double T_partial_data2=T_partial_data1*BMP388_Calib_Data.PAR_T[1];
	*Temperature=T_partial_data2+T_partial_data1*T_partial_data1*BMP388_Calib_Data.PAR_T[2]; //tempreture
 
	double P_partial_data1=BMP388_Calib_Data.PAR_P[5]*(*Temperature);
	double P_partial_data2=BMP388_Calib_Data.PAR_P[6]*(*Temperature)*(*Temperature);
	double P_partial_data3=BMP388_Calib_Data.PAR_P[7]*(*Temperature)*(*Temperature)*(*Temperature);
	double P_partial_out1=BMP388_Calib_Data.PAR_P[4]+P_partial_data1+P_partial_data2+P_partial_data3;
 
	P_partial_data1=BMP388_Calib_Data.PAR_P[1]*(*Temperature);
	P_partial_data2=BMP388_Calib_Data.PAR_P[2]*(*Temperature)*(*Temperature);
	P_partial_data3=BMP388_Calib_Data.PAR_P[3]*(*Temperature)*(*Temperature)*(*Temperature);
	double P_partial_out2=(double)(Pressure_Integer)*(BMP388_Calib_Data.PAR_P[0]+P_partial_data1+P_partial_data2+P_partial_data3);
 
	P_partial_data1=(double)(Pressure_Integer)*(double)(Pressure_Integer);
	P_partial_data2=BMP388_Calib_Data.PAR_P[8]+BMP388_Calib_Data.PAR_P[9]*(*Temperature);
	P_partial_data3=P_partial_data1*P_partial_data2;
	double P_partial_data4=P_partial_data3+(double)(Pressure_Integer)*(double)(Pressure_Integer)*(double)(Pressure_Integer)*BMP388_Calib_Data.PAR_P[10];
	*Pressure=P_partial_out1+P_partial_out2+P_partial_data4; //pressure
 }
