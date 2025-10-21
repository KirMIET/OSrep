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

wget https://raw.githubusercontent.com/KirMIET/OSlab_2025/master/lab3/text/lab3.md

./parallel_min_max --seed 42 --array_size 1000000 --pnum 4
./parallel_min_max --seed 123 --array_size 500000 --pnum 8 --by_files
./parallel_min_max --seed 42 --array_size 100000000000 --pnum 2 --timeout 1
ps -eo pid,state,time,command | grep Z
./parallel_sum --threads_num 4 --seed 42 --array_size 100000

lab5
./factorial -k 10 --pnum=4 --mod=1000000

lab6
gcc -pthread -o server server.c
gcc -o client client.c

./server --port 20001 --tnum 4

127.0.0.1:20001

./client --k 20 --mod 97 --servers servers.txt

lab7
./tcpserver 8080 100
./tcpclient 127.0.0.1 8080 100

./udpserver 20001 1024
./udpclient 127.0.0.1 20001 1024