/* @file :  main.c
 * @author  jesus mojica
 * @version 1.0.0
 * @date    29/01/2021
 * @brief   Archivo principal
 * @details
*/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL02Z4.h"
#include "fsl_debug_console.h"

#include "sdk_hal_gpio.h"
#include "sdk_hal_uart0.h"
#include "sdk_hal_i2c0.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MMA8451_I2C_DEVICE_ADDRESS	        0x1D
#define MMA8451_WHO_AM_I_MEMORY_ADDRESS		0x0D

#define CTRL_REG1                         0x00
#define XYZ_DATA_CFG                      0x01
#define CTRL_REG1                         0x01

// ejes del acelerometro
#define MMA8451_OUT_X_MSB                   0x01
#define MMA8451_OUT_X_LSB                   0x02
#define MMA8451_OUT_Y_MSB                   0X03
#define MMA8451_OUT_Y_LSB                   0x04
#define MMA8451_OUT_Z_MSB                   0x05
#define MMA8451_OUT_Z_LSB                   0x06


/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void) {

	status_t status;
	uint8_t nuevo_byte_uart;
	uint8_t	nuevo_dato_i2c;
	uint16_t significativa_MSB;    // variable para guardar la parte mas significativa
    uint16_t M_significativa_LSB;    //variable para guardar la parte menos significativa
    int16_t Dato_Final ;           //variable para guardar la union de MSB y LSB

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    (void)uart0Inicializar(115200);	//115200bps
    (void)i2c0MasterInit(100000);	//100kbps
//    i2c0ConfigData();

    PRINTF("M acelerometro\r\n");
    PRINTF("X datos eje X\r\n");
    PRINTF("Y datos eje Y\r\n");
    PRINTF("Z datos eje Z\r\n");


    while(1) {



    	if(uart0CuantosDatosHayEnBuffer()>0){
    		status=uart0LeerByteDesdeBuffer(&nuevo_byte_uart);
    		if(status==kStatus_Success){
    			printf("dato:%c\r\n",nuevo_byte_uart);

    			switch (nuevo_byte_uart) {

				case 'a':
				case 'A':
					gpioPutToggle(KPTB10);
					break;

				case 'v':
					gpioPutHigh(KPTB7);
					break;
				case 'V':
					gpioPutLow(KPTB7);
					break;

				case 'r':
					gpioPutValue(KPTB6,1);
					break;
				case 'R':
					gpioPutValue(KPTB6,0);
					break;

				case 'M':
					i2c0MasterReadByte(&nuevo_dato_i2c, MMA8451_I2C_DEVICE_ADDRESS, MMA8451_WHO_AM_I_MEMORY_ADDRESS);

					if(nuevo_dato_i2c==0x1A)
						printf("MMA8451 Encontrado!!\r\n");
					else
						printf("MMA8451 Error\r\n");

					break;

				case 'X':
				case 'x':
					i2c0ConfigData();
	    			i2c0MasterReadByte(&significativa_MSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_X_MSB);
	    			i2c0MasterReadByte(&M_significativa_LSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_X_LSB);

	    				Dato_Final=(significativa_MSB<<8)|(M_significativa_LSB);

	    				printf("Dato final eje X es igual: %d \r\n ", Dato_Final);

					break;

				case 'Y':
				case 'y':
					i2c0ConfigData();
                	i2c0MasterReadByte(&significativa_MSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_Y_MSB);
		            i2c0MasterReadByte(&M_significativa_LSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_Y_LSB);

		                Dato_Final=(significativa_MSB<<8)|(M_significativa_LSB);

						printf("Dato final eje Y es igual: %d \r\n ", Dato_Final);

					break;

	     		case 'Z':
				case 'z':
					i2c0ConfigData();
					i2c0MasterReadByte(&significativa_MSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_Z_MSB);
					i2c0MasterReadByte(&M_significativa_LSB, MMA8451_I2C_DEVICE_ADDRESS,MMA8451_OUT_Z_LSB);

					Dato_Final=(significativa_MSB<<8)|(M_significativa_LSB);
					printf("Dato final eje Z es igual: %d \r\n ", Dato_Final);

					break;

				}
    		}
    		else{
    			printf("error\r\n");
    		}
    	}
    }

    return 0 ;
}
