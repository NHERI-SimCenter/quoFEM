FROM noamelisha/simcenterbackend:latest

COPY / /app/quoFEM

WORKDIR /app/quoFEM
RUN rm -rf build
RUN mkdir build
WORKDIR /app/quoFEM/build
COPY /Examples /app/quoFEM//build/Examples

RUN conan install .. --build missing
RUN qmake ../quoFEM.pro
RUN make

CMD ["./quoFEM"]