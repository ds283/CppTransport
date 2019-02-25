FROM debian:stretch-slim AS cpptransport-build
WORKDIR /cpptransport
CMD bash -l

# set up a suitable build environment
RUN apt-get update && apt-get -y install \
    bison \
    build-essential \
    cmake \
    g++-6 \
    git \
    graphviz \
    libbison-dev \
    libboost-all-dev \
    libginac-dev \
    libopenmpi-dev \
    libssl-dev \
    libsqlite3-dev \
    python-seaborn

# pull down CppTransport from git and checkout experimental branch
RUN git clone https://github.com/ds283/CppTransport.git

# configure CppTransport build, then make and install
RUN cd CppTransport/CppTransport && git checkout experimental && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/cpptransport201801 && make install -j8

# copy configuration files
COPY ./.cpptransport /root
COPY ./.cpptransport_runtime /root
COPY ./.profile /root
