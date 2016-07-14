#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>

//http://www.pieter-jan.com/node/15
//http://elinux.org/RPi_GPIO_Code_Samples

#include "bcm2835.h"

//#define I2C_ADDR         (0xB8 >> 1)
#define I2C_ADDR         0x5c
//#define I2C_ADDR         0x68
//#define I2C_ADDR    0b01101001


unsigned short crc16(unsigned char *ptr, unsigned char len);
int am2320_read(int, uint8_t*, uint8_t, uint8_t, uint8_t);

int main(int argc, char* argv[])
{
    int fd;
    uint8_t data[8] = {0};
    
    //fd = i2c_mopen(1);
    //i2c_mset_addr(fd, I2C_ADDR);
    	

    //i2c_mwrite_regs(fd, 0x03, data, 2);
    //i2c_mread(fd, data, 4);
    //am2320_read(fd, data, 0x03, 0x00, 4);

    //printf("data 0 = %x\n", data[0]); 
    //printf("data 1 = %x\n", data[1]); 
    //printf("data 2 = %x\n", data[2]); 
    //printf("data 3 = %x\n", data[3]); 

    //uint16_t humidity = data[0] << 8 | (0xFF & data[1]);
    //uint16_t temp = data[2] << 8 | (0xFF & data[3]);
    //printf("Temp = %d\n", temp);
    //printf("Humidity  = %d\n", humidity);
    
    //i2c_mclose(fd);
    if(bcm2835_map_peripheral(&gpio) == -1){
    	printf("Failed to map the physical GPIO registers into virtual memory space.\n");
	return -1;
    }

    if(bcm2835_map_peripheral(&bsc0) == -1){
	printf("Failed to map the physical BSC1 registers into virtual memory space.\n");
    }

    //GPIO out on gpio 4
    INP_GPIO(4);
    OUT_GPIO(4);

    GPIO_SET = 1 << 4;

    bcm2835_i2c_init();

    BSC0_A = I2C_ADDR;
    
    // Waking up
    BSC0_DLEN = 0;
    BSC0_FIFO = 0;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_WRITE;

    bcm2835_wait_i2c_done();
    usleep(1000);


    BSC0_DLEN = 3;
    BSC0_FIFO = 0x03;
    BSC0_FIFO = 0x00;
    BSC0_FIFO = 0x04;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_WRITE;

    bcm2835_wait_i2c_done();
	//wait time as per datasheet
	usleep(2600);

    //Reading data
    BSC0_DLEN = 8;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_READ;
    bcm2835_wait_i2c_done();

    data[0] = BSC0_FIFO & 0xff;
    data[1] = BSC0_FIFO & 0xff;
    data[2] = BSC0_FIFO & 0xff;
    data[3] = BSC0_FIFO & 0xff;
    data[4] = BSC0_FIFO & 0xff;
    data[5] = BSC0_FIFO & 0xff;
    data[6] = BSC0_FIFO & 0xff;
    data[7] = BSC0_FIFO & 0xff;

    //data[0] = 0x01;
    //data[1] = 0xf4;
    //data[2] = 0x00;
    //data[3] = 0xfa;

    printf("Results = >");
    printf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

    uint16_t humidity = data[0] << 8 | (0xFF & data[1]);
    uint16_t temp = data[2] << 8 | (0xFF & data[3]);
    printf("%d\n", humidity);
    printf("Temp = %d\n", temp);

	uint16_t crcval = crc16(&data[2], 4);
	printf("CRC val = %x\n", crcval);
    
    /*BSC0_DLEN = 3;
    BSC0_FIFO = 0x03;
    BSC0_FIFO = 0x0B;
    BSC0_FIFO = 1;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_WRITE;

    bcm2835_wait_i2c_done();

    //Reading data
    BSC0_DLEN = 1;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_READ;
    bcm2835_wait_i2c_done();

	data[0] = BSC0_FIFO;

	printf("Version = %d\n", data[0]);
    */

    /*  
    BSC0_DLEN = 1;
    BSC0_FIFO = 0x75;
    //BSC0_FIFO = 0x3A;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_WRITE;

    bcm2835_wait_i2c_done();

    //BSC0_FIFO = 0;

    //Reading data
    BSC0_DLEN = 1;
    BSC0_S = CLEAR_STATUS;
    BSC0_C = START_READ;
    bcm2835_wait_i2c_done();

    data[0] = BSC0_FIFO & 0xff;
    printf("Result = %x\n", data[0]);
    */

    bcm2835_unmap_peripheral(&gpio);
    bcm2835_unmap_peripheral(&bsc0);

    
    return 0;
}

unsigned short crc16(unsigned char *ptr, unsigned char len)
{
	unsigned short crc =0xFFFF;
	unsigned char i;
	while(len--){
		crc ^=*ptr++;
		for(i=0;i<8;i++){
			if(crc & 0x01){
				crc>>=1;
				crc^=0xA001;
			}else{
				crc>>=1;
			}
		}
	}
	return crc;
}

int am2320_read(int fd, uint8_t *data, uint8_t func, uint8_t start, uint8_t num)
{
	uint8_t cmd[3] = {0};

	cmd[0] = func;
	cmd[1] = start;
	cmd[2] = num;

	//wake device first, as per datasheet
	uint8_t tmp = 0;
	write(fd, &tmp, 1);
	usleep(1000);
	

	if(write(fd, &cmd, 3)  == 0){
		printf("Error writing to device\n");
		return -1;
	}

	read(fd, data, num);
}


