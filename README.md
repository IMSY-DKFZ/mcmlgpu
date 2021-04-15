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