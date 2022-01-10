Monte Carlo Multi Layer

This repository contains the base code for Monte Carlo simulations in GPU. Some custom implementations have been added 
to the original code: https://code.google.com/archive/p/gpumcml/.

To build do the following:

```lang=bash
make download
make gpumcml.sm_20
```

`make download` download files needed for compilation: `cutil-linux` and `safeprimes_base32`.
Therefore you need internet connection for that step.

You further need to ensure that the `CUDA_INSTALL_ROOT` path in line 47 of the `makefile` points to your local nvcc path.
Default path is `/usr/local/cuda/bin/nvcc`, your local path structure (likely `/usr/*/bin/nvcc`) can be found via `which nvcc`.
