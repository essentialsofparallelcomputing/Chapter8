FROM debian:bullseye
WORKDIR /tmp
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -qq update && \
    apt-get -qq install -y cmake git vim gcc g++ gfortran software-properties-common \
            mpich libmpich-dev \
            openmpi-bin openmpi-doc libopenmpi-dev

RUN apt-get clean && rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-c"]

RUN groupadd chapter8 && useradd -m -s /bin/bash -g chapter8 chapter8

WORKDIR /home/chapter8
RUN chown -R chapter8:chapter8 /home/chapter8
USER chapter8

RUN git clone --recursive https://github.com/essentialsofparallelcomputing/Chapter8.git

WORKDIR /home/chapter8/Chapter8
RUN make

ENTRYPOINT ["bash"]
