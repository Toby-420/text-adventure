@echo off
echo Compiling...
gcc -o play icon/icon.o main.c -lncurses -Wall -Werror -Wpedantic -O3 -ljson-c -fstack-protector -C -H -trigraphs
echo Changing MZ header...
powershell -noprofile -executionpolicy bypass -file ./scripts/textalteration.ps1
echo Editing files...
REM del text\history.txt
REM del text\savefile.json
REM copy icon\icon.hpck text\savefile.json
echo Done.
