# Trickery
Trickery with LD_Preload

to download strace: 
```bash
sudo apt-get install strace
```
to download ltrace (sometimes helpful): 
```bash
sudo apt-get install ltrace
```
Inside src is the privacy.c file where all functions that are related to privacy are implemented in.

If a new .c-file is created, in the Makefile a new libNr must be added use/test the file with the LD_PRELOAD-statement. If it needs a new shared library, then a build must be added at the top

to test (without Makefile):
1. to build the shared library (.so):
```bash
gcc -shared -fPIC -o privacy.so src/privacy.c -ldl
```
2. to test the specific function: 
```bash
LD_PRELOAD=./privacy.so <COMMAND that uses your hijacked function OR ./TEST>
```

gcc -shared -fPIC -o lib<name>.so <file> -ldl explained:
-shared tells a shared library .so is build und not a executable file (without -shared LD_PRELOAD would not function.)
-fPIC PIC=Position Independent Code, every program using the library loads it into a diffrente memory space. This would lead to crashes without -fPIC.
-o tells the compiler that the following is the output (usually lib<name>.so, because of the linker)
<file> the source that is compilated
-l link against library
dl dynamic linking library -> used for the dlsym-statements, to call/find the real function/syscall if our hook should not catch.

## Problems/Conclusions

This is not possible to hook because that is a shell builtin aka it's an internal command of bash:
```bash
echo
```

This is also not possible to hook because it bypasses glibc's write() and calls the kernel directly (or via a private symbol) so our LD_PRELOAD overide of write() is never used:
```bash
/bin/echo
```
