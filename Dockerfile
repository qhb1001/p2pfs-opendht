FROM aberaud/opendht:latest

WORKDIR /home/p2pfs-opendht

COPY . .

RUN apt-get update
RUN apt-get install -y libfuse-dev
RUN apt-get install -y pkg-config
RUN apt-get install -y build-essential



