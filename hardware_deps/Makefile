INCLUDES = -I$(DEFINED_ERROR_PATH)

ifeq ($(SIERRA_WP7702), y)
	BUILD_PATH = sierra_wireless/wp7702
	HARDWARE_CFLAG = CFLAGS -DSIERRA_WP7702
endif

all: device.o

device.o: device.c
	$(MAKE) -C $(BUILD_PATH)
	$(CC) -c $(CFLAGS) $(INCLUDES) -MMD -MF device.c.d -o $@ $<

-include device.c.d