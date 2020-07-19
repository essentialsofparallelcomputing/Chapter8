#!/bin/sh
docker build -f Dockerfile.Ubuntu20.04 -t chapter8 .
docker run -it --entrypoint /bin/bash chapter8
