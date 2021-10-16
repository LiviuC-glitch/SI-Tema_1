INC=/usr/local/ssl/include/ 
LIB=/usr/local/ssl/lib/ 

all:
	gcc -I$(INC) -L$(LIB) -o modulKM.exe modulKM.c -lcrypto -ldl
	gcc -I$(INC) -L$(LIB) -o mod_a.exe mod_a.c -lcrypto -ldl
	gcc -I$(INC) -L$(LIB) -o mod_b.exe mod_b.c -lcrypto -ldl
	