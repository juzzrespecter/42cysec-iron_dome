FROM debian 

RUN apt-get update 
RUN apt-get install -y golang gcc procps valgrind make

WORKDIR code