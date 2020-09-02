CS 61 Docker
============

The [Docker][] container-based virtualization service lets you run a minimal
CS 61 environment, including a virtual Linux host, on a Mac OS X or Windows
host, without the overhead of a full virtual machine solution like [VMware
Workstation][], [VMware Fusion][], or [VirtualBox][].

It should be possible to do *all* CS 61 problem sets on CS 61 Docker.

Advantages of Docker:

* Docker can start and stop virtual machines incredibly quickly.
* Docker-based virtual machines are leaner and take less space on your machine.
* With Docker, you can *edit* your code in your home environment, but *run* it
  on a Linux host.

Disadvantages of Docker:

* Docker does not offer a full graphical environment. You will need to run
  programs exclusively in the terminal.
* Docker technology is a less user-friendly than virtual machines. You’ll have
  to type weird commands.
* You won’t get the fun, different feeling of a full Linux desktop.

Setting up a CS 61 Docker environment
-------------------------------------

First, build your Docker image. The first step only needs to be done once; the
last step should be performed any time the `Dockerfile` changes.

1. Download and install [Docker][].

2. Clone a copy of the [cs61-lectures repository][].

3. Change into the `cs61-lectures/docker` directory.

4. Build the image. The command will take a couple minutes.

    ```shellsession
$ docker build -t cs61:latest -f Dockerfile .
```

Second, run your Docker image. Example command and output:

```shellsession
$ docker run -it --rm -v ~/cs61-lectures:/home/cs61-user/cs61-lectures cs61:latest
cs61-user@ae28a76602d8:~$ ls
cs61-lectures
```

Explanation:

* `docker run` tells Docker to start a new virtual machine.
* `-it` says Docker should run interactively (`-i`) using a terminal (`-t`).
* `--rm` says Docker should remove the virtual machine when it is done.
* `-v LOCALDIR:LINUXDUR` says Docker should share a directory between your
  host and the Docker virtual machine. Here, I’ve asked for the host’s
  `~/cs61-lectures` directory to be mapped inside the virtual machine onto the
  `/home/cs61-user/cs61-lectures` directory, which is the virtual machine
  user’s `~/cs61-lectures` directory.
* `cs61:latest` names the Docker image to run (namely, the one you built).

The `docker run` command then plonks you into a virtual machine! A prompt like
`cs61-user@ae28a76602d8:~$` means that your terminal is connected to the VM.
You can execute any commands you want.

To escape from the VM, either type Control-D or `exit`.

[Docker]: https://docker.com/
[VMware Workstation]: https://www.vmware.com/products/workstation-player.html
[VMware Fusion]: https://www.vmware.com/products/fusion.html
[VirtualBox]: https://www.virtualbox.org/
[cs61-lectures repository]: https://github.com/cs61/cs61-lectures/