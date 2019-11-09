all:
	gcc -Wall main.c header.h readConfig.c torre.c writeLog.c pipe.c -o main -pthread
