FROM ubuntu:noble

# set environment variables for tzdata
ARG TZ=America/New_York
ENV TZ=${TZ}

# include manual pages and documentation
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update &&\
 apt-get -y install unminimize &&\
 yes | unminimize

# install GCC-related packages
RUN apt-get -y install\
 binutils-doc\
 cpp-14-doc\
 gcc-14-doc\
 g++-14\
 g++-14-multilib\
 gdb\
 gdb-doc\
 glibc-doc\
 libblas-dev\
 liblapack-dev\
 liblapack-doc\
 libstdc++-14-doc\
 make\
 make-doc

# install clang-related packages
RUN apt-get -y install\
 clang-19\
 clang-19-doc\
 lldb-19

# install qemu for WeensyOS (sadly, this pulls in a lot of crap)
RUN apt-get -y install\
 qemu-system-x86

# install programs used for system exploration
RUN apt-get -y install\
 blktrace\
 linux-tools-generic\
 strace\
 tcpdump

# install interactive programs (emacs, vim, nano, man, sudo, etc.)
RUN apt-get -y install\
 bc\
 curl\
 dc\
 emacs-nox\
 git\
 git-doc\
 man\
 micro\
 nano\
 psmisc\
 sudo\
 vim\
 wget

# set up libraries
RUN apt-get -y install\
 libreadline-dev\
 locales\
 wamerican

# install programs used for networking
RUN apt-get -y install\
 dnsutils\
 inetutils-ping\
 iproute2\
 net-tools\
 netcat-openbsd\
 telnet\
 time\
 traceroute

# install programs useful for disk images
RUN apt-get -y install\
 fdisk\
 mtools

# set up default locale
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV ENDITOR=nano

# remove unneeded .deb files
RUN rm -r /var/lib/apt/lists/*

# set up passwordless sudo for user cs61-user
ARG UID=1001
RUN useradd -m -s /bin/bash -u "${UID}" cs61-user &&\
 echo "cs61-user ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/cs61-init

# create binary reporting version of dockerfile
RUN (echo '#\!/bin/sh'; echo 'echo 22') > /usr/bin/cs61-docker-version; chmod ugo+rx,u+w,go-w /usr/bin/cs61-docker-version

# git build arguments
ARG USER=CS61\ User
ARG EMAIL=nobody@example.com

# ensure /usr/bin/c++, /usr/bin/g++, /usr/bin/clang++
RUN test -z "`which c++`" && ln -s g++-14 /usr/bin/c++;\
 test -z "`which g++`" && ln -s g++-14 /usr/bin/g++;\
 test -z "`which clang`" && ln -s clang-19 /usr/bin/clang;\
 test -z "`which clang++`" && ln -s clang++-19 /usr/bin/clang++;\
 test -z "`which lldb`" && ln -s lldb-19 /usr/bin/lldb

# configure your environment
USER cs61-user
RUN git config --global user.name "${USER}" &&\
 git config --global user.email "${EMAIL}" &&\
 git config --global safe.directory "*" &&\
 (echo "(custom-set-variables"; echo " '(c-basic-offset 4)"; echo " '(indent-tabs-mode nil))") > ~/.emacs &&\
 (echo "set expandtab"; echo "set shiftwidth=4"; echo "set softtabstop=4") > ~/.vimrc &&\
 cat /dev/null > ~/.bash_profile &&\
 echo "# 2022: avoid a Docker bug with user mapping by listing working directory" >> ~/.bash_profile &&\
 echo "ls -al > /dev/null" >> ~/.bash_profile &&\
 echo "for i in \`mount | grep /home/cs61-user | sed 's/^.*\\(\\/home[^ ]*\\).*/\\\\1/'\`; do ls -al \$i > /dev/null; done" >> ~/.bash_profile &&\
 echo "# make ssh-auth.sock user-readable" >> ~/.bash_profile &&\
 (echo "if test -e /run/host-services/ssh-auth.sock; then"; echo "  sudo chown cs61-user:cs61-user /run/host-services/ssh-auth.sock"; echo "fi") >> ~/.bash_profile &&\
 echo ". ~/.bashrc" >> ~/.bash_profile &&\
 rm -f ~/.bash_logout &&\
 echo "add-auto-load-safe-path ~" > ~/.gdbinit

WORKDIR /home/cs61-user
CMD ["/bin/bash", "-l"]

# Initial version of this Dockerfile by Todd Morrill, CS 61 DCE student
