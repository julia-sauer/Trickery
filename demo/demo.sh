#! /bin/bash

gcc ../tests/fcloseTest.c -o fcloseTest
gcc ../tests/memoryTest.c -o memoryTest
gcc ../tests/readTest.c -o readTest
gcc ../tests/strcmpTest.c -o strcmpTest
gcc ../tests/writeTest.c -o writeTest

echo "Hiiilllffee" > help.txt
echo "THIS IS VERY SECRET CONTENT IN HERE!!! Julia is a dwarf!" > secret.txt
echo "I am scared in the dark, pleas do not close me" > ungeheim.txt
echo "It is really important that this file is not read by anyone!!!" > important.txt
echo "a² + b² = c², e = m*c²" > cheatsheet_OS.txt
echo "Vino" > wichtig.pdf
echo "printf("pseudo")" > pseudo.c
echo "This file is changeable" > static.txt

gcc -shared -fPIC ../src/privacy.c ../src/privacyStrcmp.c ../src/connect.c -o hook1.so -ldl
gcc -shared -fPIC ../src/privacy.c ../src/memory.c -o hook2.so -ldl


arg1="$1"

if [ "$arg1" -eq 1 ]; then #without hook lib
    echo "============= open ============="
    cat secret.txt #open
    echo "============= fclose ============="
    ./fcloseTest
    echo "============= read ============="
    ./readTest
    echo "============= fwrite ============="
    ./writeTest
    cat output.txt
    cat cheatsheet_OS.txt
    echo "============= connect ============="
    curl example.com
    echo "============= before mv (rename) and rm (unlinkat)============="
    ls
    mv help.txt nohelp.txt
    rm wichtig.pdf
    echo "============= after mv (rename) and rm (unlinkat) ============="
    ls
    echo "============= write ============="
    cat pseudo.c
    echo "============= strcmp ============="
    ./strcmpTest #wrong password
    ./strcmpTest #right password
    rm nohelp.txt
else
    echo "============= open ============="
    LD_PRELOAD=./hook1.so cat secret.txt #open
    echo "============= fclose ============="
    LD_PRELOAD=./hook2.so ./fcloseTest
    echo "============= read ============="
    LD_PRELOAD=./hook1.so ./readTest
    echo "============= fwrite ============="
    LD_PRELOAD=./hook1.so ./writeTest
    cat output.txt
    cat cheatsheet_OS.txt
    echo "============= connect ============="
    LD_PRELOAD=./hook1.so curl example.com
    echo "============= before mv (rename) and rm (unlinkat)============="
    ls
    LD_PRELOAD=./hook1.so mv help.txt nohelp.txt
    LD_PRELOAD=./hook1.so mv static.txt dynamic.txt
    LD_PRELOAD=./hook1.so rm wichtig.pdf
    echo "============= after mv (rename) and rm (unlinkat) ============="
    ls
    echo "============= write ============="
    LD_PRELOAD=./hook1.so cat pseudo.c
    echo "============= malloc & free ============="
    LD_PRELOAD=./hook2.so ./memoryTest
    echo "============= strcmp ============="
    LD_PRELOAD=./hook1.so ./strcmpTest #wrong password
    LD_PRELOAD=./hook1.so ./strcmpTest #right password
    rm you_wish.txt
fi
