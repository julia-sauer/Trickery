build-priv:
	gcc -shared -fPIC -o libpriv.so src/privacy.c src/privacyStrcmp.c -ldl

build-conn:
	gcc -shared -fPIC -o libconn.so src/connect.c -ldl

build-mem:
	gcc -shared -fPIC -o libmem.so src/memory.c -ldl

lib1: build-priv
	LD_PRELOAD=./libpriv.so $(CMD) $(ARG)

lib2: build-conn
	LD_PRELOAD=./libconn.so curl $(ARG)

lib3: build-mem
	LD_PRELOAD=./libmem.so $(CMD) $(ARG)

