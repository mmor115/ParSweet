FROM ubuntu:22.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt update -y && apt install -y bzip2 libhwloc-dev vim git gdb python3 python3-pip \
    libpapi-dev patch cmake libblas-dev liblapack-dev libtbb-dev patch \
    libboost-dev libboost-filesystem-dev g++-12 gcc-12 \
    && apt clean

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 10
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 10

RUN git clone -b v1.9.0 --depth 1 https://github.com/STEllAR-GROUP/hpx.git /hpx
WORKDIR /hpx
RUN mkdir -p /hpx/build
WORKDIR /hpx/build
RUN cmake -DCMAKE_BUILD_TYPE=Release  \
      -DHPX_FILESYSTEM_WITH_BOOST_FILESYSTEM_COMPATIBILITY=ON \
      -DHPX_WITH_FETCH_ASIO=ON \
      -DHPX_WITH_MALLOC=system \
      -DHPX_WITH_MORE_THAN_64_THREADS=ON \
      -DHPX_WITH_EXAMPLES=Off \
      -DHPX_WITH_CXX_STANDARD=20 \
      -DHPX_WITH_DATAPAR_BACKEND=STD_EXPERIMENTAL_SIMD \
      ..
RUN make -j 1 install
#RUN git clone https://github.com/mmor115/ParallelSuite.git /psweet
#WORKDIR /psweet/build
#RUN cmake -DCMAKE_BUILD_TYPE=Release  ..
#RUN make
