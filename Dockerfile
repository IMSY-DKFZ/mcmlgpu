FROM nvidia/cuda:11.8.0-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/London

WORKDIR /code

RUN apt-get update
RUN apt-get install -y cmake

COPY . .

WORKDIR /code/build
RUN nvcc --version
RUN cmake ..
RUN make MCML -j
RUN make package -j
RUN dpkg -i MCML*.deb
RUN chmod +x MCML

ENTRYPOINT ["/code/build/MCML"]

CMD ["-A"]
