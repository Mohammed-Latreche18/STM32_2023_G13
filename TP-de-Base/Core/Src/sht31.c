#include "sht31.h"




float humidity_read_value (I2C_HandleTypeDef *hi2c1, UART_HandleTypeDef *huart2){
	   static const uint8_t CAPTEUR_ADRS = 0x44 << 1;
	   static const uint8_t CAPTEUR_CMD_MSB = 0x24;
	   static const uint8_t CAPTEUR_CMD_LSB = 0x16;
	   float umid;

		HAL_StatusTypeDef ret;
		uint8_t buf[20];
		volatile uint16_t value;
		volatile float temp;
		float Decimal_part;
		float Entier_part;
	    buf[0] = CAPTEUR_CMD_MSB;
	    		buf[1] = CAPTEUR_CMD_LSB;
	    		ret = HAL_I2C_Master_Transmit( hi2c1, CAPTEUR_ADRS, buf, 2, HAL_MAX_DELAY);
	    		if ( ret != HAL_OK)
	    		{
	    			strcpy((char*)buf, "erreur_T!!\r\n");
	    			HAL_UART_Transmit(huart2, buf, sizeof(buf), 1000);
	    		}
	    			else
	    		{



	    		ret = HAL_I2C_Master_Receive(hi2c1, CAPTEUR_ADRS, buf, 6, HAL_MAX_DELAY);
	    					if ( ret != HAL_OK)
	    				{
	    					strcpy((char*)buf, "erreur_R!!\r\n");
	    					HAL_UART_Transmit(huart2, buf, sizeof(buf), 1000);
	    				}
	    		else
	    		{

	    			value  =   buf[1] | buf[0] << 8;


	    			temp = -45 + 175 * ( (float)value / 65535);

	    			Entier_part = (int) temp;
	    			Decimal_part = temp;
	    			Decimal_part *= 100;
	    			Decimal_part = Decimal_part - (Entier_part * 100);
	    			value = buf[4] | buf[3] << 8;

	    			umid = -49 + 315 *( (float)value / 65535);

	    		}
	    		}
	    return umid;
}
