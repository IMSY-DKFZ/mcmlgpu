# Monte Carlo Multi Layer

This repository contains the base code for Monte Carlo simulations in GPU. Some custom implementations have been added 
to the original code developed by Erik Alerstam, David Han, and William C. Y. Lo: https://code.google.com/archive/p/gpumcml/.

# Setup development environment
All you need to have is a CUDA capable computer, `cmake` and `git flow`. You can set up these dependencies by running 
the following commands from a terminal:

```bash
sudo apt update
sudo apt install git-flow cmake
git flow init
```

To develop a new feature you should create a new issue in [gitlab](https://git.dkfz.de/imsy/issi/mcmlgpu/-/issues). And 
start to work in the feature by running `git flow feature start T<issue-number>-task_short_name`. Once you have pushed
your changes into the branch, you can start a merge request in gitlab.

# Build 
You will first need to install some dependencies. You need to make sure that you have a CUDA capable computer.
You can easily check this by doing `nvcc --version` from a terminal. If there is no error in that command, then you can
proceed to install the following dependencies:

```bash
sudo apt install cmake git
git lfs install
```

After installing the dependencies, you can build MCML as follows. 
You should make sure to indicate the correct GPU compute capabilities for your GPU. 
You can list the supported ones by doing: `nvcc --list-gpu-code`.

```lang=bash
mkdir build
cd build 
cmake .. -DCUDA_ARCH=86 -DCMAKE_INSTALL_PREFIX=/usr
make MCML -j
```

To install or uninstall the application on the system, you can run the following. 
You will need sudo permission if the path indicated in the previous step "CMAKE_INSTALL_PREFIX" is privileged.
````bash
make install
make uninstall
````

This application can also be packaged using conan. To do so, you should do the following from the root directory 
of the repository.

```bash
conan create . issi/stable-cuda11.5-sm86 -o cuda_arch=86
```

# Docker image
We also have a docker image that you can use for your projects. The image can be built by running the following command 
in a terminal. Make sure to have _Docker_ or _Docker compose_ installed on your computer. You can append the flag 
`--progress plain` to view more details about the progress. 

```bash
docker build -t mcml:latest .
docker run mcml:latest
```
