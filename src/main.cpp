#include <Eigen/Eigen>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include "mpc.h"
#include "example_model.h"

#define HORIZON 40
#define N_STEPS 200

int main()
{

    LinearMPC test_mpc{HORIZON, A, B, P_f, Q, R};

    auto start = std::chrono::high_resolution_clock::now();

    // simulate_dp(x, u, N_STEPS);
    test_mpc.simulate(N_STEPS, x_0, true, "/home/sevag/Documents/Projects/mpc_solver/test_file.csv");

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Simulation with " << N_STEPS << " Steps, with horizon N=" << HORIZON << ", took " << duration.count() << " ms." << std::endl;

    return 0;
}
