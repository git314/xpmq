xpmq: qnorm.o xpmq.o
	g++ -o /usr/local/bin/xpmq xpmq.o qnorm.o -std=c++11 -O2 -larmadillo
xpmq.o: qnorm.o
	g++ -c -o xpmq.o xpmq.cpp 
qnorm.o:
	gcc -c -o qnorm.o heads/qnorm.c -lm
clean: xpmq
	rm *.o
