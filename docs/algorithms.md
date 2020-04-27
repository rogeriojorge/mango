# Available Algorithms {#algorithms}

# Overview

MANGO provides interfaces to a wide variety of optimization algorithms,
including both local and global algorithms, 
and derivative-free and derivative-based algorithms,
for both general and least-squares minimization.
(For derivative-based algorithms, MANGO uses
finite difference derivatives.) Most of the algorithms are provided via interfaces
to outside software packages (GSL, PETSc, NLOPT, and HOPSPACK). MANGO can also
provide its own optimization algorithms. Presently there is one such native algorithm,
`mango_levenberg_marquardt`.

The available algorithms are enumerated in mango::algorithm_type.
You can also find the algorithm list [in the table here](algorithms_8dat_source.html),
which also shows properties of each algorithm such as whether bound constraints
are supported or required, whether (finite-difference) derivatives are used, etc.

Each algorithm can be identified either by an integer index or by a string,
identical to the name of the integer index but lowercase. For instance, PETSc's POUNDERS algorithm
can be identified either with the integer constant mango::PETSC_POUNDERS or by the string `"petsc_pounders"`.
The string always begins with the name of the software package in which the algorithm is implemented:
`gsl`, `nlopt`, `petsc`, `hopspack`, or `mango` for the case of algorithms implemented directly in MANGO.
For packages that support multiple algorithms (all packages except HOPSPACK), the algorithm name then contains
an underscore, followed by the name of the specific algorithm.


# Selecting an algorithm

## C++

There are three ways to select an algorithm: from within your driver code by specifying the enumerated constant or string, or by loading in the string from a file.
From C++, the first two approaches are provided by mango::Problem::set_algorithm(), while the file-based approach is provided by mango::Problem::read_input_file().
For instance after creating a problem instance

~~~~~{.cpp}
mango::Problem my_problem(N_parameters, state_vector, &objective_function, argc, argv);
~~~~~

we could call

~~~~~{.cpp}
my_problem.set_algorithm(mango::GSL_BFGS);
~~~~~

or

~~~~~{.cpp}
my_problem.set_algorithm("gsl_bfgs");
~~~~~

or

~~~~~{.cpp}
my_problem.read_input_file("my_input_file.dat");
~~~~~

In the last case, the input file should consist of 2 lines. The first line is an integer giving the number of worker groups, while the second line contains the string
representation of the algorithm name (e.g. `nlopt_ln_neldermead`).

## Fortran

When calling MANGO from fortran, after creating an optimization problem object with

~~~~{.f90}
type(mango_problem) :: my_problem

call mango_problem_create(my_problem,N_parameters,state_vector,objective_function)
~~~~

the corresponding subroutine calls to select the algorithm are

~~~~{.f90}
call mango_set_algorithm(my_problem,MANGO_GSL_BFGS)
~~~~

or

~~~~{.f90}
call mango_set_algorithm_from_string(my_problem,"mango_gsl_bfgs")
~~~~

or

~~~~{.f90}
call mango_read_input_file(my_problem,"my_input_file.dat")
~~~~


We now discuss the algorithms provided by each package.


# GSL (Gnu Scientific Library)

The [Gnu Scientific Library](https://www.gnu.org/software/gsl/doc/html/index.html) contains several serial (non-parallelized) algorithms for general and
least-squares optimization. 
GSL is usually available on HPC systems. It can also be installed on personal computers using package manager systems like macports or `apt-get`.
To use GSL algorithms, MANGO must be built with `MANGO_GSL_AVAILABLE=T` set in the makefile.

For general optimization problem, 
MANGO provides four GSL algorithms, which are discussed in detail [here in the GSL manual](https://www.gnu.org/software/gsl/doc/html/multimin.html#).
The option `gsl_nm` is the Nelder-Mead simplex algorithm, specifically the `gsl_multimin_fminimizer_nmsimplex2` version discussed in the GSL manual.
The option `gsl_bfgs` is the version of BFGS called `gsl_multimin_fdfminimizer_vector_bfgs2` in the GSL manual.
The options `gsl_conjugate_fr` and `gsl_conjugate_pr` give the Fletcher-Reeves and Polak-Ribiere conjugate gradient algorithms respectively.
The steepest-descent algorithm discussed in the GSL manual is not supported in MANGO.
Note that none of these GSL algorithms support bound constraints.

For least-squares problems,
MANGO provides four GSL algorithms, which are discussed in detail [here in the GSL manual](https://www.gnu.org/software/gsl/doc/html/nls.html#solving-the-trust-region-subproblem-trs):
`gsl_lm` (Levenberg-Marquardt), `gsl_dogleg` (Powell's dogleg method), `gsl_ddogleg` (double dogleg), and `gsl_subspace2d` (two-dimensional subspace).
Note that none of these GSL least-squares algorithms support bound constraints, and all require `N_terms >= N_parameters`.
The "Levenberg-Marquardt with Geodesic Acceleration" and "Steihaug-Toint Conjugate Gradient" algorithms, 
discussed in the GSL manual, are not presently supported in MANGO.


# NLOpt

[NLOpt](https://nlopt.readthedocs.io/en/latest/) is a library with many serial algorithms for general optimization. It has no algorithms specific to least-squares problems.
NLOpt is not commonly found on HPC systems. 
It can be downloaded and built by changing to the `mango/external_packages` directory and running

     > install_nlopt

To use NLOpt algorithms, MANGO must be built with `MANGO_NLOPT_AVAILABLE=T` set in the makefile.

So many algorithms are available in NLOpt that they will not be listed here; consult mango::algorithm_type or
[algorithms.dat](algorithms_8dat_source.html) for the complete list. 
A detailed explanation of each algorithm can be found
[here in the NLOpt manual](https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/).

The MANGO name for each NLOpt algorithm is identical to the corresponding name in NLOpt;
e.g. the algorithm named `NLOPT_LN_NELDERMEAD` in NLOpt is called mango::NLOPT_LN_NELDERMEAD and `nlopt_ln_neldermead` in MANGO.
Each NLOpt algorithm name begins with `nlopt_` followed by `gn_`, `ln_`, or `ld_`. The letter `g` vs `l` indicates a global vs local algorithm.
The letter `n` vs `d` indicates a derivative-free vs derivative-based algorithm respectively.
So, for instance, `nlopt_gn_direct` is a global derivative-free algorithm, `nlopt_ln_praxis` is a local derivative-free algorithm,
and `nlopt_ld_lbfgs` is a local derivative-based algorithm.
All of the global algorithms require that bound constraints be set.


# PETSc / TAO

[PETSc](https://www.mcs.anl.gov/petsc/) is a large library for scientific computing, and it includes the optimization library TAO. 
PETSc is usually available on HPC systems. It can also be installed on personal computers using package manager systems like macports,
or following the instructions [here](https://www.mcs.anl.gov/petsc/documentation/installation.html).
To use PETSc/TAO algorithms, MANGO must be built with `MANGO_PETSC_AVAILABLE=T` set in the makefile.

MANGO presently supports three optimization algorithms from TAO. `petsc_nm` is the Nelder-Mead simplex algorithm
for local derivative-free general optimization problems.
`petsc_pounders` is the POUNDERS algorithm for least-squares derivative-free minimization.
`petsc_brgn` is the "Bound-constrained regularized Gauss-Newton" algorithm for least-squares
derivative-based minimization. Note that `petsc_brgn` is available only for PETSc versions 3.11 and higher.
For more details of the algorithms, see the [TAO manual here](https://www.mcs.anl.gov/petsc/petsc-current/docs/tao_manual.pdf) 
and the [TAO API reference](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/Tao/index.html).

The least-squares algorithms `petsc_pounders` and `petsc_brgn` are the only least-squares algorithms in MANGO
that support bound constraints. These two algorithms do not require `N_terms >= N_parameters`, in contrast
to the GSL least-squares algorithms.


# HOPSPACK

[HOPSPACK](https://dakota.sandia.gov/packages/hopspack) is a library for parallelized derivative-free general optimization.
HOPSPACK is not commonly available on HPC systems. 
It can be downloaded by changing to the `mango/external_packages` directory and running

     > install_hopspack

This command will not actually build the library; rather it is compiled at the same time as MANGO, since MANGO
needs to modify several of the HOPSPACK source files.
To use HOPSPACK, MANGO must be built with `MANGO_HOPSPACK_AVAILABLE=T` set in the makefile.


The HOPSPACK package contains only a single algorithm, so there is no underscore in the integer or string name for this algorithm
in MANGO (mango::HOPSPACK and `"mango_hopspack"` respectively). This algorithm is unique in that it supports
concurrent (i.e. parallel) evaluations of the objective function even though it does not use finite-difference derivatives.
HOPSPACK does not require bound constraints, but it performs best when accurate bound constraints are supplied.


# Algorithms provided directly by MANGO

Presently, MANGO provides just one of its own algorithms, with integer mango::MANGO_LEVENBERG_MARQUARDT and string name `"mango_levenberg_marquardt"`.
This is a derivative-based algorithm for local least-squares minimization. MANGO's implementation of the Levenberg-Marquardt algorithm
has several advantages compared to `gsl_lm`. First, MANGO's version uses concurrent (i.e. parallel) evaluation of the residuals
over several values of the parameter \f$ \lambda \f$, whereas `gsl_lm` uses a serial search in \f$ \lambda \f$,
making `gsl_lm` quite load-imbalanced.
Second, `mango_levenberg_marquardt` does not require `N_terms >= N_parameters`, unlike `gsl_lm`.

To use `mango_levenberg_marquardt`, you must have the [Eigen library](http://eigen.tuxfamily.org), which is available
on many HPC systems. 
Eigen can also be downloaded by changing to the `mango/external_packages` directory and running

     > install_eigen

This command will not do any compiling or linking, since Eigen is a header-only library.
To use `mango_levenberg_marquardt`, MANGO must be built with `MANGO_EIGEN_AVAILABLE=T` set in the makefile.
