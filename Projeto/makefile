all:
	gcc -Wall  main.c header.h readConfig.c tower.c writeLog.c verify.c -o main -lpthread
	gcc -Wall client.c -o client -pthread
	gnome-terminal
	./main
