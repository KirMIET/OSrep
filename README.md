# osrep
curl -O https://raw.githubusercontent.com/IpovsOperatingSystems/os_lab_2019/master/lab1/src/background.sh
./background.sh > /dev/null &
wc -m text.txt

grep "cake" cake_rhymes.txt > with_cake.txt


bash << 'END'

for i in {1..150}; do
    od -An -N2 -i /dev/random | awk '{print $1 % 1000}'
done > numbers.txt
END

./average.sh $(cat numbers.txt)

lab2
gcc main.c swap.c -o programm
./programm

static lib
gcc -c /workspaces/OSrep/lab2/task2/revert_string.c -o revert_string.o

ar rcs librevert_string.a revert_string.o

gcc -static /workspaces/OSrep/lab2/task2/main.c -I:/workspaces/OSrep/lab2/task2 -L. -lrevert_string -o program_static

./program_static "Hello World"

dynamic lib
gcc -c -fPIC /workspaces/OSrep/lab2/task2/revert_string.c -o drevert_string.o

gcc -shared -o dlibrevert_string.so drevert_string.o

gcc /workspaces/OSrep/lab2/task2/main.c -I:/workspaces/OSrep/lab2/task2 -L. -l:dlibrevert_string.so -o program_dynamic

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

./program_dynamic "Hello World"