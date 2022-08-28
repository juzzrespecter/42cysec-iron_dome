#!/bin/bash

# ~~ dev ~~

NAME="our_host"

docker build . -t ${NAME}:latest
docker run \
        -it \
        --name ${NAME} \
        -v $PWD/irondome:/irondome \
        ${NAME} \
        /bin/bash