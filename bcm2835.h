#ifndef BCM2708_H
#define BCM2708_H
// Low Level Access I/O from Linux
// Adopted from http://elinux.org/RPi_GPIO_Code_Samples
// http://www.pieter-jan.com/node/15
 
#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define BSC0_BASE		 (BCM2708_PERI_BASE + 0x205000) // I2C controller
#define BSC1_BASE		 (BCM2708_PERI_BASE + 0x804000) // I2C controller 2
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
 
// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio.addr+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio.addr+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.addr+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio.addr+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio.addr+10) // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio.addr+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio.addr+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio.addr+38) // Pull up/pull down clock

// I2C macros
#define BSC0_C          *(bsc0.addr + 0x00)
#define BSC0_S          *(bsc0.addr + 0x01)
#define BSC0_DLEN       *(bsc0.addr + 0x02)
#define BSC0_A          *(bsc0.addr + 0x03)
#define BSC0_FIFO       *(bsc0.addr + 0x04)

#define BSC1_C          *(bsc1.addr + 0x00)
#define BSC1_S          *(bsc1.addr + 0x01)
#define BSC1_DLEN       *(bsc1.addr + 0x02)
#define BSC1_A          *(bsc1.addr + 0x03)
#define BSC1_FIFO       *(bsc1.addr + 0x04)
 
#define BSC_C_I2CEN     (1 << 15)
#define BSC_C_INTR      (1 << 10)
#define BSC_C_INTT      (1 << 9)
#define BSC_C_INTD      (1 << 8)
#define BSC_C_ST        (1 << 7)
#define BSC_C_CLEAR     (1 << 4)
#define BSC_C_READ      1
 
#define START_READ      BSC_C_I2CEN|BSC_C_ST|BSC_C_CLEAR|BSC_C_READ
#define START_WRITE     BSC_C_I2CEN|BSC_C_ST
 
#define BSC_S_CLKT      (1 << 9)
#define BSC_S_ERR       (1 << 8)
#define BSC_S_RXF       (1 << 7)
#define BSC_S_TXE       (1 << 6)
#define BSC_S_RXD       (1 << 5)
#define BSC_S_TXD       (1 << 4)
#define BSC_S_RXR       (1 << 3)
#define BSC_S_TXW       (1 << 2)
#define BSC_S_DONE      (1 << 1)
#define BSC_S_TA        1
 
#define CLEAR_STATUS    BSC_S_CLKT|BSC_S_ERR|BSC_S_DONE

struct bcm2835_peripheral{
	unsigned long base_addr;
	int mem_fd;
	void *gpio_map;
	//I/O access
	volatile unsigned *addr;
};

//struct bcm2835_peripheral gpio = {GPIO_BASE};

extern struct bcm2835_peripheral gpio;
extern struct bcm2835_peripheral bsc0;
extern struct bcm2835_peripheral bsc1;
 
int bcm2835_map_peripheral(struct bcm2835_peripheral *p);
void bcm2835_unmap_peripheral(struct bcm2835_peripheral *p);
void bcm2835_i2c_0_init();
void bcm2835_i2c_1_init();
void bcm2835_wait_i2c_0_done();
void bcm2835_wait_i2c_1_done()
#endif
