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

im src privacy.c isch d datei wo alli hijacked funktione drikömme wo mit privacy z tue hän

wenn ihr e neui Datei machet für e Funktion wo nüt mit privacy z tue het, denn könned ihr im Makefile eif e neus libNR mache und halt entsprechend s LD_PRELOAD in schriebe und oobe s build wenns e neui library bruucht.

zums teschte:
1. zum d shared library (.so) zbaue:
```bash
gcc -shared -fPIC -o privacy.so src/privacy.c -ldl
```
2. zum dini funktion zteschte: 
```bash
LD_PRELOAD=./privacy.so <COMMAND WO ME WILL HIJACKE>
```

gcc -shared -fPIC -o lib<name>.so <file> -ldl ERKLÄRT:
-shared sagt dass eine shared library .so gebaut wird und nicht ein ausführbares programm (ohne -shared würde LD_PRELOAD damit nicht funtkionieren.)
-fPIC PIC=Position Independent Code jedes Program das die library benutzt lädt sie an eine andere speicheradresse, sprich ohne -fPIC würd das crashen
-o sagt das folgendes Output ist (muss lib<name>.so sein weil das wird von Linker erwartet.)
<file> the source that is compilated
-l link against library
dl dynamic linking library -> brauchen wir wegen dlsym, also um die 'richtige' funktion/syscall zu finden wenn unsere funktion nicht greifen soll.