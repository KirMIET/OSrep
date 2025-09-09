# osrep
curl -O https://raw.githubusercontent.com/IpovsOperatingSystems/os_lab_2019/master/lab1/src/background.sh
./background.sh > /dev/null &
wc -m text.txt

grep "cake" cake_rhymes.txt > with_cake.txt