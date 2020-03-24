FROM ubuntu:18.04 AS builder
WORKDIR /project
RUN apt-get update && \
    apt-get install -y cmake git vim gcc g++ gfortran software-properties-common \
            mpich libmpich-dev \
            openmpi-bin openmpi-doc libopenmpi-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Installing latest GCC compiler (version 9)
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update && \
    apt-get install -y gcc-9 g++-9 gfortran-9 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# We are installing both OpenMPI and MPICH. We could use the update-alternatives to switch between them
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90\
                        --slave /usr/bin/g++ g++ /usr/bin/g++-9\
                        --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-9\
                        --slave /usr/bin/gcov gcov /usr/bin/gcov-9

SHELL ["/bin/bash", "-c"]

RUN groupadd chapter8 && useradd -m -s /bin/bash -g chapter8 chapter8

WORKDIR /home/chapter8
RUN chown -R chapter8:chapter8 /home/chapter8
USER chapter8

RUN git clone --recursive https://github.com/essentialsofparallelcomputing/Chapter8.git

WORKDIR /home/chapter8/Chapter8
RUN make

ENTRYPOINT ["bash"]
