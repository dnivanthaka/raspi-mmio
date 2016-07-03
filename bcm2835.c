#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bcm2835.h"

struct bcm2835_peripheral gpio = {GPIO_BASE};
struct bcm2835_peripheral bsc0 = {BSC0_BASE};
struct bcm2835_peripheral bsc1 = {BSC1_BASE};

int bcm2835_map_peripheral(struct bcm2835_peripheral *p){
	if((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0){
		printf("Failed to open /dev/mem, check permissions.\n");
		return -1;
	}

	p->gpio_map = mmap(
		NULL,
		BLOCK_SIZE,
		PROT_READ|PROT_WRITE,
		MAP_SHARED,
		p->mem_fd,
		p->base_addr
	);

	if(p->gpio_map == MAP_FAILED){
		perror("mmap");
		return -1;
	}

	p->addr = (volatile unsigned int *)p->gpio_map;

	return 0;
}

void bcm2835_unmap_peripheral(struct bcm2835_peripheral *p){
	munmap(p->gpio_map, BLOCK_SIZE);
	close(p->mem_fd);
}

void i2c_init(){
	INP_GPIO(0);
	SET_GPIO_ALT(0, 0);
	INP_GPIO(1);
	SET_GPIO_ALT(1, 0);
}

void wait_i2c_done(){
	int timeout = 50;
	while( (!((BSC0_S) & BSC_S_DONE)) && --timeout ){
		usleep(1000);
	}

	if(timeout == 0){
		printf("Error: wait_i2c_done() timeout.\n");
	}
}
