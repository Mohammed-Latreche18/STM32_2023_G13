/*
 * sht31.h
 *
 *  Created on: 23 Feb 2023
 *      Author: moham
 */

#ifndef INC_SHT31_H_
#define INC_SHT31_H_

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "i2c.h"


#define _OPEN_SYS_ITOA_EXT

float humidity_read_value (I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart2);

#endif /* INC_SHT31_H_ */
