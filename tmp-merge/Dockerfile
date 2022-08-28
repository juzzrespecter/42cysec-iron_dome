FROM debian:bullseye-slim

RUN apt update \
    && apt install -y \
        make \
        gcc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /irondome
# mete un usuario bobo por aqui
# y por aqui se mete copia del binario

CMD tail -f /dev/null