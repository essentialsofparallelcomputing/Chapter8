default:	SendRecv1 SendRecv2 SendRecv3 SendRecv4 SendRecv5 SendRecv6
all:	SendRecv1 SendRecv2 SendRecv3 SendRecv4 SendRecv5 SendRecv6

SendRecv1: SendRecv1.c
	mpicc SendRecv1.c  -o SendRecv1

SendRecv2: SendRecv2.c
	mpicc SendRecv2.c  -o SendRecv2

SendRecv3: SendRecv3.c
	mpicc SendRecv3.c  -o SendRecv3

SendRecv4: SendRecv4.c
	mpicc SendRecv4.c  -o SendRecv4

SendRecv5: SendRecv5.c
	mpicc SendRecv5.c  -o SendRecv5

SendRecv6: SendRecv6.c
	mpicc SendRecv6.c  -o SendRecv6

clean:
	rm -f SendRecv1 SendRecv1.o SendRecv2 SendRecv2.o
	rm -f SendRecv3 SendRecv3.o SendRecv4 SendRecv4.o
	rm -f SendRecv5 SendRecv5.o SendRecv6 SendRecv6.o

distclean:
	rm -f Makefile
