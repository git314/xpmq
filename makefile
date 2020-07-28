xpmq: qnorm.o xpmq.o
	g++ -o /usr/local/bin/xpmq xpmq.o qnorm.o -O3 -I ~/softwares/armadillo-9.900.1/include/ -DARMA_DONT_USE_WRAPPER -lopenblas -llapack -std=c++11 -fopenmp
xpmq.o: qnorm.o
	g++ -c -o xpmq.o xpmq.cpp 
qnorm.o:
	gcc -c -o qnorm.o heads/qnorm.c -lm
clean: xpmq
	rm *.o
