	sudo cp /lib/libcrypto.a /usr/local/lib/
	sudo cp /src/crypto_lib.hpp /usr/local/include/
	sudo ldconfig

    #g++ main.cpp -lcrypto