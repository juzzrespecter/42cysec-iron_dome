FROM debian 

RUN apt-get update 
RUN apt-get install -y golang gcc procps valgrind make sudo strace
#make user to show binary can only be executed as root
RUN useradd non-root
RUN echo non-root:hola | chpasswd
RUN usermod -aG sudo non-root

WORKDIR code