	sudo cp lib/libcrypto_c.a /usr/local/lib/
	sudo cp src/my_crypto.h /usr/local/include/
	sudo ldconfig

gcc main.c -lcrypto_c