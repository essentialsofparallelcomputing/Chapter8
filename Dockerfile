FROM ubuntu:18.04 AS builder
WORKDIR /project
RUN apt-get update && \
    apt-get install -y bash cmake git openmpi-bin openmpi-doc libopenmpi-dev g++ vim wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-c"]

RUN groupadd -r chapter8 && useradd -r -s /bin/false -g chapter8 chapter8

WORKDIR /chapter8
RUN chown -R chapter8:chapter8 /chapter8
USER chapter8

RUN git clone https://github.com/essentialsofparallelcomputing/Chapter8.git

ENTRYPOINT ["bash"]
