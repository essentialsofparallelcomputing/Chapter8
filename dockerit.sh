#!/bin/sh
docker build -f Dockerfile.Ubuntu20.4
docker run -it --entrypoint /bin/bash chapter6
