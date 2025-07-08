#include <iostream>
#include <chrono>
#include "mpc.h"
#include "example_model.h"
#include "Plotting/plotter.h"

#define HORIZON 40
#define N_STEPS 200

int main()
{

    LinearMPC test_mpc{HORIZON, A, B, P_f, Q, R};

    std::vector<double> t;

    auto start = std::chrono::high_resolution_clock::now();

    // simulate_dp(x, u, N_STEPS);
    auto results = test_mpc.simulate(N_STEPS, x_0, false, "/home/sevag/Documents/Projects/mpc_solver/test_file.csv");

    std::vector<double> u;

    for (int i = 0; i < N_STEPS; i++)
    {
        t.push_back(i);
        u.push_back(results.second[i][0]);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Simulation with " << N_STEPS << " Steps, with horizon N=" << HORIZON << ", took " << duration.count() << " ms." << std::endl;

    // Initialise plotter
    Plotter plot;

    // plot then show some data
    plot.plot(t, u);

    plot.render();

    return 0;
}
