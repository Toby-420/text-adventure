@echo off
echo Compiling...
gcc -o play assets/icon/icon.o src/*.c -lncurses -Wall -Werror -Wpedantic -O3 -ljson-c -lSDL2main -lSDL2_mixer -lSDL2 -lSDL2_ttf -fstack-protector -trigraphs -lmingw32
echo Changing MZ header...
powershell -noprofile -executionpolicy bypass -file ./scripts/textalteration.ps1
echo Editing files...
echo Done.
