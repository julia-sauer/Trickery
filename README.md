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

im src sind d files mit de neue implementatione vo de funktione
im tests sind d teschts wo me die neue implementatione teschtet.

zums teschte:
1. zum d shared library (.so) zbaue:
```bash
make
```
2. zum d teschts zbaue: 
```bash
make test
```
3. zum alles uszführe/teschte: 
```bash 
make run
```
