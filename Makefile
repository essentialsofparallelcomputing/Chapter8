All: MinWorkExample Send_Recv SynchronizedTimer FileRead GlobalSums DebugPrintout ScatterGather StreamTriad GhostExchange HybridMPIPlusOpenMP

MinWorkExample: MinWorkExample/build/MinWorkExampleMPI

MinWorkExample/build/MinWorkExampleMPI:
	cd MinWorkExample; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./MinWorkExampleMPI

Send_Recv: Send_Recv/build/SendRecv1

Send_Recv/build/SendRecv1:
	cd Send_Recv; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./SendRecv2; mpirun -n 2 SendRecv3; mpirun -n 2 SendRecv4; mpirun -n 2 SendRecv5; mpirun -n 2 SendRecv6

SynchronizedTimer: SynchronizedTimer/build/SynchronizedTimer1

SynchronizedTimer/build/SynchronizedTimer1:
	cd SynchronizedTimer; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./SynchronizedTimer1; mpirun -n 2 ./SynchronizedTimer2

FileRead: FileRead/build/FileRead

FileRead/build/FileRead:
	cd FileRead; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./FileRead

GlobalSums: GlobalSums/build/globalsums

GlobalSums/build/globalsums:
	cd GlobalSums; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./globalsums

DebugPrintout: DebugPrintout/build/DebugPrintout

DebugPrintout/build/DebugPrintout:
	cd DebugPrintout; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./DebugPrintout

ScatterGather: ScatterGather/build/ScatterGather

ScatterGather/build/ScatterGather:
	cd ScatterGather; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./ScatterGather

StreamTriad: StreamTriad/build/StreamTriad

StreamTriad/build/StreamTriad:
	cd StreamTriad; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./StreamTriad; mpirun -n 2 ./StreamTriadVec

GhostExchange: GhostExchange/GhostExchange_ArrayAssign/GhostExchange

GhostExchange/GhostExchange_ArrayAssign/GhostExchange:
	cd GhostExchange; ./build.sh; ./batch.sh

HybridMPIPlusOpenMP: HybridMPIPlusOpenMP/build/CartExchange

HybridMPIPlusOpenMP/build/CartExchange:
	cd HybridMPIPlusOpenMP/; mkdir build; cd build; cmake ..; make; mpirun -n 2 ./CartExchange

clean:
	cd MinWorkExample; rm -rf build
	cd Send_Recv; rm -rf build
	cd SynchronizedTimer; rm -rf build
	cd FileRead; rm -rf build
	cd GlobalSums; rm -rf build
	cd DebugPrintout; rm -rf build
	cd ScatterGather; rm -rf build
	cd StreamTriad; rm -rf build
	cd GhostExchange; ./clean.sh
	cd HybridMPIPlusOpenMP; rm -rf build
