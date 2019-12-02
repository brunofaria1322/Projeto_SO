all:
	gcc -Wall main.c header.h readConfig.c torre.c writeLog.c verify.c -o main -pthread
	gcc -Wall client.c -o client -pthread
	gnome-terminal
	./main
