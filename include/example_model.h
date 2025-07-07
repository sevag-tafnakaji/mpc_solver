#include <Eigen/Eigen>

#define N_STATES 4
#define N_INPUTS 1
#define N_OUTPUTS 2

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
