# Chapter 8 MPI: the parallel backbone
This is from Chapter 8 of Parallel and High Performance Computing book, Robey and Zamora
Manning Publications, available at http://manning.com

The book may be obtained at
   http://www.manning.com/?a_aid=ParallelComputingRobey

Copyright 2019-2020 Robert Robey, Yuliana Zamora, and Manning Publications
Emails: brobey@earthlink.net, yzamora215@gmail.com

See License.txt for licensing information.

MPI requires either OpenMPI or MPICH

# MinWorkExample (Book: listings 8.1 through 8.3)
      Demonstrates both a simple makefile and cmake
      Build with either make or cmake
      Run with ./MinWorkExampleMPI

# Send Receive examples (Book: listings 8.4 through 8.9)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./Send_Recv1 -- always hangs
      Run ./Send_Recv2 -- sometimes fails
      Run ./Send_Recv3 through ./Send_Recv6

# Collective examples

## Synchronized Timer - MPI_Barrier and MPI_Reduce (Book: listing 8.10 and 8.12)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./SynchronizedTimer1
      Run ./SynchronizedTimer2

## File Read - MPI_Bcast (Book: listing 8.11)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./FileRead

## GlobalSums - MPI_Reduce (Book: listing 8.13 through 8.15)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./globalsums

## DebugPrintout - MPI_Gather (Book: listing 8.16)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./DebugPrintout

## ScatterGather - MPI_Scatterv and MPI_Gatherv (Book: listing 8.17)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./ScatterGather

# StreamTriad (Book: listing 8.18)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./StreamTriad and ./StreamTriadVec
      
# GhostExchange (Book: listings 8.19 through 8.33)
      Run the ghost/cart exchange problems with 
      Edit the batch.sh file and change the mpirun commands to your liking
      ./batch.sh |& tee results.txt
      ./get_stats.sh | tee stats.out

# HybridMPIPlusOpenMP (Book: listing 8.34 and 8.35)
      Has both a simple makefile and cmake
      Build with either make or cmake
      Run ./CartExchange
