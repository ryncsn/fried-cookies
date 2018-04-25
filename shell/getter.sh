#!/bin/bash

count=1356;
for i in `seq 114 150`;
do
    for j in `seq 12`; do
        wget http:///$i/-${j}.jpg -O $count.jpg
        count=$(($count + 1));
    done
    sleep 15
done
