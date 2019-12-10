sleep 1
./client DEPARTURE TP70 init: 10 takeoff: 50
./client DEPARTURE TP73 init: 10 takeoff: 50
./client DEPARTURE TP71 init: 20 takeoff: 40
./client DEPARTURE TP72 init: 30 takeoff: 60

./client ARRIVAL TP80 init: 40 eta: 50 fuel: 50
./client ARRIVAL TP81 init: 50 eta: 10 fuel: 30
./client ARRIVAL TP82 init: 60 eta: 10 fuel: 40


./client ARRIVAL TP83 init: 20 eta: 30 fuel: 50
./client ARRIVAL TP84 init: 20 eta: 30 fuel: 60
./client ARRIVAL TP85 init: 30 eta: 30 fuel: 60

#echo "ARRIVAL TP80 init: 40 eta: 50 fuel: 50
#ARRIVAL TP81 init: 50 eta: 10 fuel: 30
#ARRIVAL TP82 init: 60 eta: 10 fuel: 40" > input_pipe
