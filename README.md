<p align="center">
    <img src="resources/icon.png" alt="Logo" width="200"/>
</p>

# Monte Carlo Multi Layer accelerated by GPU

This repository contains the base code for Monte Carlo simulations in a GPU of light transport on turbid media in GPU.
Custom implementations have been added to the original code developed by
[Erik Alerstam, David Han, and William C. Y. Lo](https://code.google.com/archive/p/gpumcml/).

This project adds the following features:

1. Modern build and install rules with CMake.
2. Custom targeting of compute capabilities for modern GPUs through the flag `-DCUDA_ARCH`.
3. Reduces IO operations thus increasing speed.
4. New docker image.
5. Easy install and uninstall mechanisms.
6. Conan packaging enabled.
7. Adds computation of penetration depth for each simulation at runtime.
8. Modern code styling using pre-commit hooks.
9. Progress bar display for simulations.
10. Reduced terminal clutter.
11. Eliminates dependency to deprecated `cutil` library.

[![asciicast](https://asciinema.org/a/EIYfdZXnjKoDHhXUN1TooJAWK.svg)](https://asciinema.org/a/EIYfdZXnjKoDHhXUN1TooJAWK)

# Setup development environment
All you need to have is a CUDA capable computer, `cmake` and `git lfs`. You can set up these dependencies by running
the following commands from a terminal:

```bash
sudo apt update
sudo apt install cmake git-lfs
git lfs pull # do this from the root directory of the repository
```

To develop a new feature you should create a new issue in [gitlab](https://git.dkfz.de/imsy/issi/mcmlgpu/-/issues). And
start to work in the feature by creating a new branch `<issue-number>-task_short_name`. Once
you have pushed your changes into the branch, you can start a merge request in GitLab.

# Build
You will first need to install some dependencies. You need to make sure that you have a CUDA capable computer.
You can easily check this by doing `nvcc --version` from a terminal. If there is no error in that command, then you can
proceed to install the following dependencies:

```bash
sudo apt install cmake git
git lfs install
git lfs pull
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

# Running an example
After building `MCML`, you can run an example to be sure that it is working as intended:

```bash
MCML -i resources/sample.mci -O batch.mco
```

This should create a file called `batch.mco` with the following content:

```text
ID,Specular,Diffuse,Absorbed,Transmittance,Penetration
MCML_Bat_NA_0_Sim_0_3.00e-07.mco,0.02404,0.0277725,0.948185,0,0.998
MCML_Bat_NA_0_Sim_0_3.02e-07.mco,0.02404,0.0299027,0.946057,0,0.998
```

# Contributing a feature/bug fix
If you have doubts on how to finish your feature branch, you can always ask for help

1. Create an issue on `GitHub <https://github.com/IMSY-DKFZ/mcmlgpu/issues>`_, if a task does not exist yet.
2. Assign the task to you.
3. Create a fork of the repository.
4. Create a new branch.
   The `branch name` has to match the following pattern: `<issue-number>-<short_description_of_task>`
5. Implement your feature
6. Update :code:`feature` branch: :code:`git checkout <branch_name> && git merge develop`.
7. Create a merge request for your feature.
   select `develop` as the destination branch.
8. The branch will be reviewed and automatically merged if there are no requested changes.

# Docker image
We also have a docker image that you can use for your projects. The image can be built by running the following command
in a terminal. Make sure to have _Docker_ or _Docker compose_ installed on your computer. You can append the flag
`--progress plain` to view more details about the progress.

```bash
docker build -t mcml:latest .
docker run mcml:latest
```

# Funding
This project has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No. [101002198]).

![ERC](resources/LOGO_ERC-FLAG_EU_.jpg)
![DKFZ](resources/LOGO_DKFZ.png)
