FROM nvidia/cuda:11.4.0-devel-ubuntu20.04

WORKDIR /code

RUN apt-get update

COPY makefile makefile

COPY . .

RUN ["make", "clean_sm_20"]
RUN ["make", "gpumcml.sm_20"]
RUN ["chmod", "+x", "gpumcml.sm_20"]
RUN nvcc --version

ENTRYPOINT ["./gpumcml.sm_20"]

CMD ["-A"]