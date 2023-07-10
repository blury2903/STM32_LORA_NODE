/*
 * pzem004t.c
 *
 *  Created on: Nov 18, 2022
 *      Author: blury
 */

#include "pzem004t.h"
#include "LoRa.h"

//float value = 0.0;
//char valueArray[100] = {0};


void init_pzem(pzem *pzem, UART_HandleTypeDef *huart, uint8_t addr)
{
	if(addr < 0x01 || addr > 0xF8)
	{
		addr = PZEM_DEFAULT_ADDR;
	}
	pzem->_addr = addr;
	pzem->huart = huart;
}

uint8_t readAddress(pzem *pzem)
{
	uint8_t addr = 0;
	sendCmd8(pzem,CMD_RHR, WREG_ADDR, 0x01,0xF8);
	if(checkCRC(pzem, 7))
	{
		 HAL_Delay(20);
		 pzem->rxbufferIndex=0;
		 addr = ((uint32_t)pzem->rxBuffer[3] << 8 | // Raw address
		                           (uint32_t)pzem->rxBuffer[4]);

		 return addr;
	}
	return 0;
}

void sendCmd8(pzem *pzem, uint8_t cmd, uint16_t rAddr, uint16_t val, uint16_t slave_addr)
{
	uint8_t sendBuffer[8]; //send buffer

	if((slave_addr == 0xFFFF) || (slave_addr < 0x01) || (slave_addr > 0xF7))
	{
		slave_addr = pzem->_addr;
	}

	sendBuffer[0] = slave_addr;					// set slave address
	sendBuffer[1] = cmd;						// set command
	sendBuffer[2] = (rAddr >> 8) & 0xFF;		// set high byte of register address
	sendBuffer[3] = (rAddr) & 0xFF;				// set low byte of register address
	sendBuffer[4] = (val >> 8) & 0xFF;			// set high byte of register value
	sendBuffer[5] = (val) & 0xFF;				// set low byte of register value

	setCRC((uint8_t *)sendBuffer, 8);
	HAL_UART_Transmit(pzem->huart, sendBuffer, 8, 100);
	HAL_Delay(100);
}

uint8_t checkCRC(pzem *pzem, uint8_t len)
{
	uint16_t crc_check, crc_buffer;
	crc_buffer = ((uint16_t)pzem->rxBuffer[len-2]  | (uint16_t)pzem->rxBuffer[len-1] << 8);
	crc_check = CRC16(pzem->rxBuffer, len-2);

	if(crc_buffer == crc_check)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void setCRC(uint8_t *buf, uint16_t len)
{
	if(len <= 2) return;		//sanity check

	uint16_t crc = CRC16(buf, len-2);		// crc of data

	// Write high and low byte to last positions of sendBuffer
	buf[len - 1] = (crc >> 8) & 0xFF;		// high byte
	buf[len - 2] = crc & 0xFF;				// low byte

}


// Pre computed CRC table
static const uint16_t crcTable[] =
{
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

uint16_t CRC16(const volatile uint8_t *data, uint16_t len)
{
    uint8_t nTemp; // CRC table index
    uint16_t crc = 0xFFFF; // Default value

    while (len--)
    {
        nTemp = (*data++) ^ crc;
        crc >>= 8;
        crc ^= (uint16_t)crcTable[nTemp];
    }
    return crc;
}

uint8_t updateValues(pzem *pzem)
{
	sendCmd8(pzem, (uint8_t)CMD_RIR, 0x00, 0x0A, (uint16_t)pzem->_addr);
//	while(isrPzem);
//	isrPzem = 1;
	if(checkCRC(pzem, 25))
	{
		pzem->values.voltage = ((uint32_t)pzem->rxBuffer[3] << 8 | // Raw voltage in 0.1V
								(uint32_t)pzem->rxBuffer[4])/10.0;
		pzem->values.current = ((uint32_t)pzem->rxBuffer[5] << 8 | // Raw current in 0.001A
								(uint32_t)pzem->rxBuffer[6] |
								(uint32_t)pzem->rxBuffer[7] << 24 |
								(uint32_t)pzem->rxBuffer[8] << 16) / 1000.0;

		pzem->values.power =   ((uint32_t)pzem->rxBuffer[9] << 8 | // Raw power in 0.1W
								(uint32_t)pzem->rxBuffer[10] |
								(uint32_t)pzem->rxBuffer[11] << 24 |
								(uint32_t)pzem->rxBuffer[12] << 16) / 10.0;

		pzem->values.energy =  ((uint32_t)pzem->rxBuffer[13] << 8 | // Raw Energy in 1Wh
								(uint32_t)pzem->rxBuffer[14] |
								(uint32_t)pzem->rxBuffer[15] << 24 |
								(uint32_t)pzem->rxBuffer[16] << 16) / 1000.0;

		pzem->values.frequency=((uint32_t)pzem->rxBuffer[17] << 8 | // Raw Frequency in 0.1Hz
								(uint32_t)pzem->rxBuffer[18]) / 10.0;

		pzem->values.pf =      ((uint32_t)pzem->rxBuffer[19] << 8 | // Raw pf in 0.01
								(uint32_t)pzem->rxBuffer[20])/100.0;

		pzem->values.alarms =  ((uint32_t)pzem->rxBuffer[21] << 8 | // Raw alarm value
								(uint32_t)pzem->rxBuffer[22]);
		memset(pzem->rxBuffer,0,sizeof(pzem->rxBuffer));
		pzem->rxbufferIndex=0;

		return 1;
	}
	return 0;
}


/*
void printVoltage(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.voltage;
	sprintf(valueArray, "Voltage: %.1fV\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}

void printCurrent(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.current;
	sprintf(valueArray, "Current: %.3fA\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}

void printPower(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.power;
	sprintf(valueArray, "Power: %.1fW\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}

void printEnergy(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.energy;
	sprintf(valueArray, "Energy: %.2fWh\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}

void printFrequency(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.frequency;
	sprintf(valueArray, "Frequency: %.1fHz\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}

void printpF(UART_HandleTypeDef *huart, pzem *pzem)
{
	value = pzem->values.pf;
	sprintf(valueArray, "Power factor: %.2f\r\n", value);
	HAL_UART_Transmit(huart, (uint8_t *)valueArray, strlen(valueArray), 1000);
}
*/

/*
void spi_Transmit(LoRa* _LoRa, pzem *pzem)
{
	volt = pzem->values.voltage;
	curr = pzem->values.current;
	power = pzem->values.power;
	energy = pzem->values.energy;
	freq = pzem->values.frequency;
	pf = pzem->values.pf;

	sprintf(valueArray, "%.1f,%.3f,%.1f,%.2f,%.1f,%.2f,\r\n", volt, curr, power, energy, freq, pf);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_Voltage_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.voltage;
	sprintf(valueArray, "Voltage: %.1fV\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_Current_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.current;
	sprintf(valueArray, "Current: %.3fA\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_Power_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.power;
	sprintf(valueArray, "Power: %.1fW\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_Energy_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.energy;
	sprintf(valueArray, "Energy: %.2fWh\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_Frequency_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.frequency;
	sprintf(valueArray, "Frequency: %.1fHz\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}

void SPI_pF_Transmit(LoRa* _LoRa, pzem *pzem)
{
	value = pzem->values.pf;
	sprintf(valueArray, "Power factor: %.2f\r\n", value);
	LoRa_transmit(_LoRa, (uint8_t*) valueArray, strlen(valueArray), 3000);
}
*/
