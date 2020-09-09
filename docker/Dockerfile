FROM ubuntu:focal

# set environment variables for tzdata
ARG TZ=America/New_York
ENV TZ ${TZ}

# include manual pages and documentation
RUN DEBIAN_FRONTEND=noninteractive apt-get update && yes | unminimize

# install GCC-related packages
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
        binutils-doc \
        cpp-doc \
        gcc-doc \
        g++ \
        g++-multilib \
        gdb \
        gdb-doc \
        glibc-doc \
        libblas-dev \
        liblapack-dev \
        liblapack-doc \
        libstdc++-10-doc \
        make \
        make-doc

# install clang-related packages
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
        clang \
        clang-10-doc \
        lldb

# install qemu for pset 4 (sadly, this pulls in a lot of crap)
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
        qemu-system-x86

# install programs used for system exploration
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
        blktrace \
        linux-tools-generic \
        strace \
        tcpdump

# install interactive programs (emacs, vim, nano, man, sudo, etc.)
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
        bc \
        curl \
        dc \
        emacs-nox \
        git \
        git-doc \
        man \
        micro \
        nano \
        sudo \
        vim \
        wget

# set up libraries
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
    libreadline-dev \
    locales

# set up default locale
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8

# remove unneeded .deb files
RUN rm -r /var/lib/apt/lists/*

# create binary reporting version of dockerfile
RUN (echo '#\!/bin/sh'; echo 'echo 2') > /usr/bin/cs61-docker-version; chmod ugo+rx,u+w,go-w /usr/bin/cs61-docker-version

# set up passwordless sudo for user cs61-user
RUN useradd -m -s /bin/bash cs61-user && \
    echo "cs61-user ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/cs61-init
USER cs61-user

# git build arguments
ARG USER=CS61\ User
ARG EMAIL=nobody@example.com

# configure your environment
RUN git config --global user.name "${USER}" && \
    git config --global user.email "${EMAIL}" && \
    (echo "(custom-set-variables"; echo " '(c-basic-offset 4)"; echo " '(indent-tabs-mode nil))") > ~/.emacs && \
    (echo "set expandtab"; echo "set shiftwidth=4"; echo "set softtabstop=4") > ~/.vimrc

WORKDIR /home/cs61-user

# Initial version of this Dockerfile by Todd Morrill, CS 61 DCE student
