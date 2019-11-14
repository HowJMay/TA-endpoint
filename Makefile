ROOT_DIR = $(CURDIR)
MBEDTLS = $(ROOT_DIR)/mbedtls

CFLAGS = -fPIC -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -O2 -g3
LDFLAGS =

INCLUDES = -I$(MBEDTLS)/include
LIBS = $(MBEDTLS)/library/libmbedx509.a $(MBEDTLS)/library/libmbedtls.a $(MBEDTLS)/library/libmbedcrypto.a

SOURCES = main.c https.c crypto_utils.c serializer.c tryte_byte_conv.c

OBJS = $(SOURCES:.c=.o)

.SUFFIXES:.c .o

all: ta_client

mbedtls_make:
	@for dir in $(MBEDTLS); do \
		$(MAKE) -C $$dir ; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

ta_client: mbedtls_make $(OBJS)
	@echo Linking: $@ ....
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)  -L$(ROOT_DIR)/third_party/openssl -lcrypto
	#$(CC) -g -o $@ $(OBJS) $(LDFLAGS) $(LIBS) -lcrypto

%.o: %.c
	@echo Compiling: $< ....
	#$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $^ -I../openssl/include
	$(CC) -g -c $(CFLAGS) $(INCLUDES) -o $@ $^ -I $(ROOT_DIR)/third_party/openssl/include/

clean: clean_client mbedtls_clean

clean_client:
	rm -f ta_client *.o

mbedtls_clean:
	@for dir in $(MBEDTLS); do \
		$(MAKE) -C $$dir clean; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done