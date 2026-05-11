build-priv:
	gcc -shared -fPIC -o libpriv.so src/privacy.c -ldl

build-conn:
	gcc -shared -fPIC -o libconn.so src/connect.c -ldl


lib1: build-priv
	LD_PRELOAD=./libpriv.so $(CMD)

lib2: build-priv
	LD_PRELOAD=./libpriv.so $(CMD) $(ARG)

lib3: build-conn
	LD_PRELOAD=./libconn.so curl $(ARG)

