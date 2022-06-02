# wdt-vortex
Driver, utilities and configuration for a watchdog on Winsystems PCM-C418 Vortex single board CPU

Original code for the **wdt_vortex** kernel module and **wdt_test** user program were provided by Winsystems.

## Contents of wdt-vortex package

### Kernel modules
- wdt_vortex.ko

### Executable binaries on /usr/bin
- wdt_test

### Configuration files
- /etc/modules-load.d/wdt-vortex.conf: enables loading the **wdt_vortex.ko** module at boot
- /etc/modprobe.d/wdt-vortex.conf: options for wdt_vortex.ko

## Building wdt-vortex package

### KERNEL_DIR
To build the **wdt_vortex.ko** module, make needs to be able to find the Linux kernel headers matching the kernel version of the target system.

The headers are typically found in **/usr/src**, in a directory such as **linux-headers-4.15.18-vortex86dx3**.

To build On Debian you need to install a **linux-headers** package corresponding to the kernel of the target system.  

For example, the header package for a Vortex DX3 running Ubuntu bionic, kernel 4.15.18:

    apt-get install linux-headers-4.15.18-vortex86dx3

Assuming builds are only done in containers, not on a Vortex, the **linux-headers** package only needs to be installed in the container image.

The Makefile will set KERNEL_DIR to the first directory that is found on /usr/src.  If there are more than one, you should move or uninstall the extra ones, or hand edit the Makefile and set the value of KERNEL_DIR. This can also be done if the headers are in some other directory than **/usr/src**:

    KERNEL_DIR := /my/location/linux-headers-4.15.18-vortex86dx3 

### Building in a Debian container
The nidas-devel package contains a **start_podman** script in **/opt/nidas/bin** which can be used to run docker/podman images for building software.

1. Clone the wdt-vortex repository:

        git clone https://github.com/NCAR/wdt-vortex.git
        cd wdt-vortex

1. Run a podman image interactively.  An interactive session may be necessary if you're installing
the package and the gpg-agent needs to prompt for the password to the \<eol-prog@ucar.edu\> signing key.

    start_podman bionic

    1. The Ubuntu bionic image on docker.io/ncar contains the linux-headers-4.15.18-vortex86dx3 package.

       If you need to install a different header package for the target system:

            apt-get update 
            apt-get install linux-headers-x.y.z

    1. Edit the Makefile if you need to override the default search for KERNEL_DIR on **/usr/src**:

            vi /root/current/Makefile

    1. Build the package, which will be placed in the parent directory (/root in this example):

            cd /root/current
            ./build_dpkg.sh i386

    1. Add the -I option to install the package with reprepo to the EOL debian repository on /net/ftp

            ./build_dpkg.sh i386 -I bionic

1. To build the package non-interacively

        start_podman bionic /root/current/build_dpkg.sh i386 -I bionic
