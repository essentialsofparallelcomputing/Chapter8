All: MinWorkExample Send_Recv SynchronizedTimer FileRead GlobalSums DebugPrintout ScatterGather StreamTriad GhostExchange HybridMPIPlusOpenMP

MinWorkExample: MinWorkExample/MinWorkExampleMPI

MinWorkExample/MinWorkExampleMPI:
	cd MinWorkExample; cmake .; make; mpirun -n 2 ./MinWorkExampleMPI

Send_Recv: Send_Recv/SendRecv1

Send_Recv/SendRecv1:
	cd Send_Recv; cmake .; make; mpirun -n 2 ./SendRecv2; mpirun -n 2 SendRecv3; mpirun -n 2 SendRecv4; mpirun -n 2 SendRecv5; mpirun -n 2 SendRecv6

SynchronizedTimer: SynchronizedTimer/SynchronizedTimer1

SynchronizedTimer/SynchronizedTimer1:
	cd SynchronizedTimer; cmake .; make; mpirun -n 2 ./SynchronizedTimer1; mpirun -n 2 ./SynchronizedTimer2

FileRead: FileRead/FileRead

FileRead/FileRead:
	cd FileRead; cmake .; make; mpirun -n 2 ./FileRead

GlobalSums: GlobalSums/globalsums

GlobalSums/globalsums:
	cd GlobalSums; cmake .; make; mpirun -n 2 ./globalsums

DebugPrintout: DebugPrintout/DebugPrintout

DebugPrintout/DebugPrintout:
	cd DebugPrintout; cmake .; make; mpirun -n 2 ./DebugPrintout

ScatterGather: ScatterGather/ScatterGather

ScatterGather/ScatterGather:
	cd ScatterGather; cmake .; make; mpirun -n 2 ./ScatterGather

StreamTriad: StreamTriad/StreamTriad

StreamTriad/StreamTriad:
	cd StreamTriad; cmake .; make; mpirun -n 2 ./StreamTriad; mpirun -n 2 ./StreamTriadVec

GhostExchange: GhostExchange/GhostExchange_ArrayAssign/GhostExchange

GhostExchange/GhostExchange_ArrayAssign/GhostExchange:
	cd GhostExchange; ./build.sh; ./batch.sh

HybridMPIPlusOpenMP: HybridMPIPlusOpenMP/CartExchange

HybridMPIPlusOpenMP/CartExchange:
	cd HybridMPIPlusOpenMP/; cmake .; make; mpirun -n 2 ./CartExchange

clean:
	cd MinWorkExample; make clean; make distclean
	cd Send_Recv; make clean; make distclean
	cd SynchronizedTimer; make clean; make distclean
	cd FileRead; make clean; make distclean
	cd GlobalSums; make clean; make distclean
	cd DebugPrintout; make clean; make distclean
	cd ScatterGather; make clean; make distclean
	cd StreamTriad; make clean; make distclean
	cd GhostExchange; ./clean.sh
	cd HybridMPIPlusOpenMP; make clean; make distclean
