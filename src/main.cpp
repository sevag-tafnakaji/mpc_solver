#include <iostream>
#include <chrono>
#include "mpc.h"
#include "example_model.h"
// #include "Plotting/plotter.h"
// #include <Plotter/Plotter.h>

#include <osqp++.h>

#define HORIZON 40
#define N_STEPS 200

using namespace Eigen;
using namespace osqp;

int main()
{

    // LinearMPC test_mpc{HORIZON, A, B, P_f, Q, R};

    // std::vector<double> t;
    // std::vector<double> t1;

    // auto start = std::chrono::high_resolution_clock::now();

    // auto results = test_mpc.simulate(N_STEPS, x_0, false, "/home/sevag/Documents/Projects/mpc_solver/test_file.csv");

    // std::vector<double> u;
    // std::vector<double> x1;
    // std::vector<double> x2;
    // std::vector<double> x3;
    // std::vector<double> x4;

    // std::vector<double> k;

    // for (int i = 0; i < N_STEPS; i++)
    // {
    //     t.push_back(i);
    //     t1.push_back(i - 100);
    //     k.push_back(2 * (i - 100) + 10);
    //     u.push_back(results.second[i][0]);
    //     x1.push_back(results.first[i][0]);
    //     x2.push_back(results.first[i][1]);
    //     x3.push_back(results.first[i][2]);
    //     x4.push_back(results.first[i][3]);
    // }

    // t1.push_back(N_STEPS - 100);
    // k.push_back(2 * (N_STEPS - 100) + 10);

    // auto end = std::chrono::high_resolution_clock::now();

    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // std::cout << "Simulation with " << N_STEPS << " Steps, with horizon N=" << HORIZON << ", took " << duration.count() << " ms." << std::endl;

    // // Initialise plotter
    // Plotter plot;

    // // plot then show some data
    // plot.plot(t, u);
    // plot.plot(t, x1);
    // plot.plot(t, x2);
    // plot.plot(t, x3);
    // plot.plot(t, x4);

    // // plot.plot(t1, k);

    // plot.render();

    const double kInfinity = std::numeric_limits<double>::infinity();
    SparseMatrix<double> objective_matrix(2, 2);
    const Triplet<double> kTripletsP[] = {
        {0, 0, 2.0}, {1, 0, 0.5}, {0, 1, 0.5}, {1, 1, 2.0}};
    objective_matrix.setFromTriplets(std::begin(kTripletsP),
                                     std::end(kTripletsP));

    SparseMatrix<double> constraint_matrix(1, 2);
    const Triplet<double> kTripletsA[] = {{0, 0, 1.0}};
    constraint_matrix.setFromTriplets(std::begin(kTripletsA),
                                      std::end(kTripletsA));

    OsqpInstance instance;
    instance.objective_matrix = objective_matrix;
    instance.objective_vector.resize(2);
    instance.objective_vector << 1.0, 0.0;
    instance.constraint_matrix = constraint_matrix;
    instance.lower_bounds.resize(1);
    instance.lower_bounds << 1.0;
    instance.upper_bounds.resize(1);
    instance.upper_bounds << kInfinity;

    OsqpSolver solver;
    OsqpSettings settings;
    // Edit settings if appropriate.
    auto status = solver.Init(instance, settings);
    // Assuming status.ok().
    OsqpExitCode exit_code = solver.Solve();
    // Assuming exit_code == OsqpExitCode::kOptimal.
    double optimal_objective = solver.objective_value();
    Eigen::VectorXd optimal_solution = solver.primal_solution();

    return 0;
}
