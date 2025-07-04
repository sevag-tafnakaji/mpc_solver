#include <Eigen/Eigen>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

#define N_STATES 4
#define N_INPUTS 1
#define N_OUTPUTS 2

#define HORIZON 40
#define N_STEPS 200

Eigen::Vector<double, N_STATES> x_0{{M_PI / 38}, {0}, {0}, {0}};

Eigen::Matrix<double, N_STATES, N_STATES> A{{1.0041, 0.01, 0.0, -0.0},
                                            {0.8281, 1.0041, 0.0, -0.0093},
                                            {0.0002, 0.0, 1.0, 0.0098},
                                            {0.0491, 0.0002, 0.0, 0.9629}};

Eigen::Matrix<double, N_STATES, N_STATES> Q{{1.0, 0.0, 0.0, 0.0},
                                            {0.0, 1.0, 0.0, 0.0},
                                            {0.0, 0.0, 1.0, 0.0},
                                            {0.0, 0.0, 0.0, 1.0}};

Eigen::Matrix<double, N_STATES, N_STATES> P_f{{10.0, 0.0, 0.0, 0.0},
                                              {0.0, 10.0, 0.0, 0.0},
                                              {0.0, 0.0, 10.0, 0.0},
                                              {0.0, 0.0, 0.0, 10.0}};

Eigen::Matrix<double, N_STATES, N_INPUTS> B{{0.0007},
                                            {0.1398},
                                            {0.0028},
                                            {0.5605}};

Eigen::Matrix<double, N_INPUTS, N_INPUTS> R{{0.1}};

Eigen::Matrix<double, N_OUTPUTS, N_STATES> C{{1, 0, 0, 0},
                                             {0, 0, 1, 0}};

std::vector<Eigen::Matrix<double, N_INPUTS, N_STATES>> DP_Riccati(int N_horizon)
{
    std::vector<Eigen::Matrix<double, N_INPUTS, N_STATES>> K_k;
    std::vector<Eigen::Matrix<double, N_STATES, N_STATES>> P_k;

    P_k.push_back(P_f);

    // Course book goes from N to 0, therefore vector must be flipped at the end.
    for (int i = 0; i < N_horizon; i++)
    {
        Eigen::Matrix<double, N_INPUTS, N_STATES> temp_k = -(R + B.transpose() * P_k[i] * B).inverse() * B.transpose() * P_k[i] * A;
        Eigen::Matrix<double, N_STATES, N_STATES> temp_p = Q + A.transpose() * P_k[i] * A + A.transpose() * P_k[i] * B * temp_k;
        K_k.push_back(temp_k);
        P_k.push_back(temp_p);
    }

    std::reverse(P_k.begin(), P_k.end());
    std::reverse(K_k.begin(), K_k.end());

    return K_k;
}

// Solved iteratively. Tested and provides the same results as MATLAB's idare command
Eigen::Matrix<double, N_INPUTS, N_STATES> Inf_Riccati()
{
    // P = P_inf at the end
    Eigen::Matrix<double, N_STATES, N_STATES> P = Q;
    Eigen::Matrix<double, N_STATES, N_STATES> P_next;

    double diff = std::numeric_limits<double>::max();

    int iter_idx = 0;
    int max_iter = 100000;
    float tolerance = 0.1;
    float dt = 0.001;

    while (diff > tolerance || iter_idx > max_iter)
    {
        iter_idx++;
        P_next = Q + A.transpose() * P * A - A.transpose() * P * B * (R + B.transpose() * P * B).inverse() * B.transpose() * P * A;
        diff = fabs((P_next - P).maxCoeff());
        P = P_next;
    }

    return -(R + B.transpose() * P * B).inverse() * B.transpose() * P * A;
}

// Simulate controller solved through DP/Horizon control
void simulate_dp(std::vector<Eigen::Vector<double, N_STATES>> &x,
                 std::vector<Eigen::Vector<double, N_INPUTS>> &u,
                 int N_steps)
{
    for (unsigned int i = 0; i < N_steps; i++)
    {
        std::vector<Eigen::Matrix<double, N_INPUTS, N_STATES>> K = DP_Riccati(HORIZON);

        u.push_back(K[0] * x[i]);
        x.push_back(A * x[i] + B * u[i]);
    }
}

// Simulate controller solved through INF horizon
void simulate_inf(std::vector<Eigen::Vector<double, N_STATES>> &x,
                  std::vector<Eigen::Vector<double, N_INPUTS>> &u,
                  int N_steps)
{
    Eigen::Matrix<double, N_INPUTS, N_STATES> K = Inf_Riccati();
    for (unsigned int i = 0; i < N_steps; i++)
    {
        u.push_back(K * x[i]);
        x.push_back(A * x[i] + B * u[i]);
    }
}

// Save to csv to plot results with python
void write_to_csv(std::vector<Eigen::Vector<double, N_STATES>> x,
                  std::vector<Eigen::Vector<double, N_INPUTS>> u)
{
    std::string file_name = "../4_R_" + std::to_string(R[0, 0]) + "_N_" + std::to_string(HORIZON) + ".csv";

    std::ofstream csv_file;
    csv_file.open(file_name);
    csv_file << "x1,x2,x3,x4,u\n";

    int num_elems = u.size();

    for (unsigned int i = 0; i < num_elems; i++)
    {
        csv_file << x[i][0] << "," << x[i][1] << "," << x[i][2] << "," << x[i][3] << "," << u[i][0] << "\n";
    }
    csv_file.close();
}

int main()
{

    std::vector<Eigen::Matrix<double, N_INPUTS, N_STATES>> K;

    std::vector<Eigen::Vector<double, N_STATES>> x;
    std::vector<Eigen::Vector<double, N_INPUTS>> u;

    x.push_back(x_0);

    auto start = std::chrono::high_resolution_clock::now();

    simulate_dp(x, u, N_STEPS);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Simulation with " << N_STEPS << " Steps, with horizon N=" << HORIZON << ", took " << duration.count() << " ms." << std::endl;

    write_to_csv(x, u);

    return 0;
}
