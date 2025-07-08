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
    std::vector<double> x1;
    std::vector<double> x2;
    std::vector<double> x3;
    std::vector<double> x4;

    for (int i = 0; i < N_STEPS; i++)
    {
        t.push_back(i);
        u.push_back(results.second[i][0]);
        x1.push_back(results.first[i][0]);
        x2.push_back(results.first[i][1]);
        x3.push_back(results.first[i][2]);
        x4.push_back(results.first[i][3]);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Simulation with " << N_STEPS << " Steps, with horizon N=" << HORIZON << ", took " << duration.count() << " ms." << std::endl;

    // Initialise plotter
    Plotter plot;

    // plot then show some data
    // plot.plot(t, u);
    plot.plot(t, x1);
    plot.plot(t, x2);
    plot.plot(t, x3);
    plot.plot(t, x4);

    plot.render();

    return 0;
}
