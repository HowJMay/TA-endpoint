INCLUDES = -I$(DEFINED_ERROR_PATH)

all: hardware_deps.o

hardware_deps.o: hardware_deps.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $<.d -o $@ $<
	