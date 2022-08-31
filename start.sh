#!/bin/bash

# ~~ dev ~~

NAME="our_host"

case $1 in
    "up")
	docker build . -t ${NAME}:latest
	docker run \
               -it \
               --name ${NAME} \
               -v $PWD/code:/code \
			   -v $PWD/files:/files \
               ${NAME} \
               /bin/bash
	;;
    "down")
	docker stop ${NAME}
	docker rm ${NAME}
	;;
    "enter")
	docker exec -it ${NAME} /bin/bash
	;;
    *)
	echo "usage: ./start.sh [ up | down | enter ]"
	exit 1
esac
