# Trickery
Trickery with LD_Preload

zum strace abelade: 
```bash
sudo apt-get install strace
```
zum ltrace (viel übersichtlicher) abelade: 
```bash
sudo apt-get install ltrace
```

im src privacy.c isch d datei wo alli hijacked funktione drikömme

zums teschte:
1. zum d shared library (.so) zbaue:
```bash
gcc -shared -fPIC -o privacy.so src/privacy.c -ldl
```
2. zum dini funktion zteschte: 
```bash
LD_PRELOAD=./privacy.so <COMMAND WO ME WILL HIJACKE>
```