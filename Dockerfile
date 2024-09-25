FROM nvidia/cuda:11.8.0-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/London

RUN apt-get update
RUN apt-get install -y cmake git python3.10-dev python3-pip
RUN pip3 install typing_extensions

WORKDIR /dependencies
RUN git clone --branch v2.1.0 https://github.com/wjakob/nanobind.git
WORKDIR /dependencies/nanobind
RUN git submodule update --init --recursive
RUN mkdir build && cd build && cmake .. && make install -j

WORKDIR /code
COPY . .
WORKDIR /code/build
RUN nvcc --version
RUN cmake ..
RUN make MCML -j
RUN make mcmlgpu -j
RUN make package -j
RUN dpkg -i MCML*.deb
RUN chmod +x MCML

WORKDIR /code
RUN pip3 install .
ENTRYPOINT python3 examples/run_simulations.py

#ENTRYPOINT ["/code/build/MCML"]
#
#CMD ["-A"]
