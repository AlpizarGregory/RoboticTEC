obj-m += arduino_custom_driver.o

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

load:
	sudo insmod arduino_custom_driver.ko

unload:
	sudo rmmod arduino_custom_driver

cdc:
	sudo rmmod cdc_acm

check:
	sudo lsmod | grep -E "arduino|cdc"
	sudo ls -lh /dev/ | grep -E "arduino|ACM"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

