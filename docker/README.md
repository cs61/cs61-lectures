CS 61 Docker
============

The [Docker][] container-based virtualization service lets you run a
minimal CS 61 environment, including Linux, on a Mac OS X or Windows
computer, without the overhead of a full virtual machine like [VMware
Workstation][], [VMware Fusion][], or [VirtualBox][].

It should be possible to do all CS 61 problem sets on CS 61 Docker.

Advantages of Docker:

* Docker can start and stop virtual machines incredibly quickly.
* Docker-based virtual machines are small and occupy little space on your machine.
* With Docker, you can easily *edit* your code in your home environment, but
  *compile and run* it on a Linux host.

Disadvantages of Docker:

* Docker does not offer a graphical environment. You will need to run all CS 61
  programs exclusively in the terminal.
* Docker technology is less user-friendly than virtual machines. You’ll have
  to type weird commands.
* You won’t get the fun, different feeling of a graphical Linux desktop.


## Creating CS 61 Docker

1.  Download and install [Docker][].

2.  Clone the [cs61-lectures repository][repo] onto your computer.

3.  Change into the `cs61-lectures/docker` directory.

4.  Run this command. It will take a while—up to ten minutes.

    ```shellsession
    $ ./cs61-build-docker
    ```

    The command starts up a virtual Linux-based computer running inside your
    computer. It then installs a bunch of software useful for CS 61 on that
    environment, then takes a snapshot of the running environment. (The
    snapshot has a name, such as `cs61:latest` or `cs61:arm64`.) Once the
    snapshot is created, it’ll take just a second or so for Docker to restart
    it.

We may need to change the Docker image during the term. If we do, you’ll
update your repository to get the latest Dockerfile, then re-run the
`./cs61-build-docker` command from Step 4. However, later runs should be
faster since they’ll take advantage of your previous work.

> `./cs61-build-docker` is a wrapper around `docker build`. On x86-64 hosts,
> it runs
> `docker build -t cs61:latest -f Dockerfile --platform linux/amd64 .`
{.note}

## Running CS 61 Docker by script

Our handout repositories, including `cs61-lectures` and `cs61-psets`, contain
a `cs61-run-docker` script that provides good arguments and boots Docker into
a view of the current directory. We will update this script throughout the
term.

For example, here’s an example of running CS 61 Docker on a Mac OS X host. At
first, `uname` (a program that prints the name of the currently running
operating system) reports `Darwin`. But after `./cs61-run-docker` connects the
terminal to a Linux virtual machine, `uname` reports `Linux`. At the end of
the example, `exit` quits the Docker environment and returns the terminal to
Mac OS X.

```shellsession
$ cd ~/cs61-lectures
$ uname
Darwin
$ uname -a
Darwin Eddies-MacBook-Pro.local 20.6.0 Darwin Kernel Version 20.6.0: Wed Jun 23 00:26:27 PDT 2021; root:xnu-7195.141.2~5/RELEASE_ARM64_T8101 arm64
$ ./cs61-run-docker
cs61-user@a47f05ea5085:~/cs61-lectures$ uname
Linux
cs61-user@a47f05ea5085:~/cs61-lectures$ uname -a
Linux 8006bb91a43b 6.10.4-linuxkit #1 SMP PREEMPT_DYNAMIC Mon Aug 12 08:48:58 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
cs61-user@a47f05ea5085:~/cs61-lectures$ ls
common  cs61-run-docker  docker  README.md
cs61-user@a47f05ea5085:~/cs61-lectures$ exit
exit
$ 
```

A prompt like `cs61-user@a47f05ea5085:~$` means that your terminal is
connected to the VM. (The `a47f05ea5085` part is a unique identifier for this
running VM.) You can execute any Linux commands you want. To escape from the
VM, type Control-D or run the `exit` command.

The script assumes your Docker container is named `cs61:latest` or `cs61:arm64`.


### Running CS 61 Docker by hand

If you don’t want to use the script, use a command like the following.

```shellsession
$ docker run -it --platform linux/amd64 --rm -v ~/cs61-lectures:/home/cs61-user/cs61-lectures cs61:latest
```

Explanation:

* `docker run` tells Docker to start a new virtual machine.
* `-it` says Docker should run interactively (`-i`) using a terminal (`-t`).
* `--platform linux/amd64` says Docker should emulate an x86-64-based machine.
  It’s necessary to specify this if you have (for example) an Apple M1-based
  laptop.
* `--rm` says Docker should remove the virtual machine when it is done.
* `-v LOCALDIR:LINUXDUR` says Docker should share a directory between your
  host and the Docker virtual machine. Here, I’ve asked for the host’s
  `~/cs61-lectures` directory to be mapped inside the virtual machine onto the
  `/home/cs61-user/cs61-lectures` directory, which is the virtual machine
  user’s `~/cs61-lectures` directory.
* `cs61:latest` names the Docker image to run (namely, the one you built).

Here’s an example session:

```shellsession
$ docker run -it --platform linux/amd64 --rm -v ~/cs61-lectures:/home/cs61-user/cs61-lectures cs61:latest
cs61-user@a15e6c4c8dbe:~$ ls
cs61-lectures
cs61-user@a15e6c4c8dbe:~$ echo "Hello, world"
Hello, world
cs61-user@a15e6c4c8dbe:~$ cs61-docker-version
22
cs61-user@a15e6c4c8dbe:~$ exit
exit
$ 
```

[Docker]: https://docker.com/
[VMware Workstation]: https://www.vmware.com/products/workstation-player.html
[VMware Fusion]: https://www.vmware.com/products/fusion.html
[VirtualBox]: https://www.virtualbox.org/
[cs61-lectures]: https://github.com/cs61/cs61-lectures/
[repo]: https://github.com/cs61/cs61-lectures/
