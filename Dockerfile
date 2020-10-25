# Part of the examples from the Parallel and High Performance Computing
# Robey and Zamora, Manning Publications
#   https://github.com/EssentialsofParallelComputing/Chapter8
#
# The built image can be found at:
#
#   https://hub.docker.com/r/essentialsofparallelcomputing/chapter8
#
# Author:
# Bob Robey <brobey@earthlink.net>

FROM ubuntu:20.04
LABEL maintainer Bob Robey <brobey@earthlink.net>

ARG DOCKER_LANG=en_US
ARG DOCKER_TIMEZONE=America/Denver

WORKDIR /tmp
RUN apt-get -qq update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get -qq install -y locales tzdata && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV LANG=$DOCKER_LANG.UTF-8 \
    LANGUAGE=$DOCKER_LANG:UTF-8

RUN ln -fs /usr/share/zoneinfo/$DOCKER_TIMEZONE /etc/localtime && \
    locale-gen $LANG && update-locale LANG=$LANG && \
    dpkg-reconfigure -f noninteractive locales tzdata

ENV LC_ALL=$DOCKER_LANG.UTF-8

RUN apt-get -qq update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get -qq install -y cmake git vim gcc g++ gfortran software-properties-common \
            mpich libmpich-dev \
            openmpi-bin openmpi-doc libopenmpi-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Installing latest GCC compiler (version 10)
RUN apt-get -qq update && \
    apt-get -qq install -y gcc-10 g++-10 gfortran-10 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN update-alternatives \
      --install /usr/bin/gcc      gcc      /usr/bin/gcc-9       80 \
      --slave   /usr/bin/g++      g++      /usr/bin/g++-9          \
      --slave   /usr/bin/gfortran gfortran /usr/bin/gfortran-9     \
      --slave   /usr/bin/gcov     gcov     /usr/bin/gcov-9      && \
    update-alternatives \
      --install /usr/bin/gcc      gcc      /usr/bin/gcc-10      90 \
      --slave   /usr/bin/g++      g++      /usr/bin/g++-10         \
      --slave   /usr/bin/gfortran gfortran /usr/bin/gfortran-10    \
      --slave   /usr/bin/gcov     gcov     /usr/bin/gcov-10     && \
    chmod u+s /usr/bin/update-alternatives


SHELL ["/bin/bash", "-c"]

RUN groupadd chapter8 && useradd -m -s /bin/bash -g chapter8 chapter8

RUN usermod -a -G video chapter8

WORKDIR /home/chapter8
RUN chown -R chapter8:chapter8 /home/chapter8
USER chapter8

ENV LANG='en_US.UTF-8'
ENV DISPLAY :0
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES display,graphics,utility

RUN git clone --recursive https://github.com/essentialsofparallelcomputing/Chapter8.git

WORKDIR /home/chapter8/Chapter8
#RUN make

ENTRYPOINT ["bash"]
