FROM debian 

RUN apt-get update 
RUN apt-get install -y golang gcc procps valgrind make sudo strace \
    && useradd non-root \
    && echo non-root:hola | chpasswd \
    && usermod -aG sudo non-root

WORKDIR code