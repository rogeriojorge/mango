#ifndef MANGO_H
#define MANGO_H

#include<mpi.h>
#include<string>
#include<fstream>

namespace mango {

  typedef enum {
    PACKAGE_MANGO,
    PACKAGE_PETSC,
    PACKAGE_NLOPT,
    PACKAGE_HOPSPACK,
    PACKAGE_GSL,
    NUM_PACKAGES /* Not an actual package, just counting. */
  } package_type;

  typedef enum {
    PETSC_NM,
    PETSC_POUNDERS,
    NLOPT_GN_DIRECT,
    NLOPT_GN_DIRECT_L,
    NLOPT_GN_DIRECT_L_RAND,
    NLOPT_GN_DIRECT_NOSCAL,
    NLOPT_GN_DIRECT_L_NOSCAL,
    NLOPT_GN_DIRECT_L_RAND_NOSCAL,
    NLOPT_GN_ORIG_DIRECT,
    NLOPT_GN_ORIG_DIRECT_L,
    NLOPT_GN_CRS2_LM,
    NLOPT_LN_COBYLA,
    NLOPT_LN_BOBYQA,
    NLOPT_LN_PRAXIS,
    NLOPT_LN_NELDERMEAD,
    NLOPT_LN_SBPLX,
    NLOPT_LD_LBFGS,
    HOPSPACK,
    NUM_ALGORITHMS  /* Not an actual algorithm, just counting. */
  } algorithm_type;

  const double NUMBER_FOR_FAILED = 1.0e+12;

  typedef void (*objective_function_type)(int*, const double*, double*, int*);

  class problem {
  private:
    MPI_Comm mpi_comm_world;
    MPI_Comm mpi_comm_worker_groups;
    MPI_Comm mpi_comm_group_leaders;
    algorithm_type algorithm;
    bool algorithm_uses_derivatives;
    bool least_squares_algorithm;
    bool least_squares;
    int package;
    std::string algorithm_name;
    int N_procs_world;
    int mpi_rank_world;
    int N_procs_worker_groups;
    int mpi_rank_worker_groups;
    int N_procs_group_leaders;
    int mpi_rank_group_leaders;
    int worker_group;
    bool proc0_world;
    bool proc0_worker_groups;
    int N_worker_groups;
    int N_parameters;
    int N_terms;
    objective_function_type objective_function;
    int function_evaluations;
    std::ofstream output_file;
    int argc;
    char** argv;
    
    void group_leaders_loop();
    void optimize_least_squares();
    void defaults();
    void get_algorithm_properties();
    void optimize_petsc();
    void optimize_nlopt();
    void optimize_hopspack();
    void optimize_gsl();
    void write_file_line(const double*, double);
    /* double nlopt_objective_function(unsigned, const double*, double*, void*); 
    void objective_function_wrapper(const double*, double*, bool*); */

  public:
    double* state_vector;
    double* targets;
    double* sigmas;
    bool centered_differences;
    double finite_difference_step_size;
    std::string output_filename;

    /*  problem() : N_worker_groups(987) {}; */
    problem(int, double*, objective_function_type, int, char**); /* For non-least-squares problems */
    problem(int, double*, int, double*, double*); /* For least-squares problems */
    ~problem();
    void set_algorithm(algorithm_type);
    void set_algorithm(std::string);
    void read_input_file(std::string);
    void set_output_filename(std::string);
    void mpi_init(MPI_Comm);
    void optimize();
    bool is_least_squares();
    void objective_function_wrapper(const double*, double*, bool*); 
    void finite_difference_gradient(const double*, double*, double*);
  };
}

#endif
