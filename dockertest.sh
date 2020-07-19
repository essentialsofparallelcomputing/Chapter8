#!/bin/sh
docker build --no-cache -t chapter8 .
#docker run -it --entrypoint /bin/bash chapter8
docker build --no-cache -f Dockerfile.Ubuntu20.04 -t chapter8 .
#docker run -it --entrypoint /bin/bash chapter8
docker build --no-cache -f Dockerfile.debian -t chapter8 .
#docker run -it --entrypoint /bin/bash chapter8
