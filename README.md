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