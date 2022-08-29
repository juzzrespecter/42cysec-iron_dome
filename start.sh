#!/bin/bash

# ~~ dev ~~

NAME="our_host"

docker build . -t ${NAME}:latest
docker run \
        -it \
        --name ${NAME} \
        -v $PWD/code:/code \
        ${NAME} \
        /bin/bash
