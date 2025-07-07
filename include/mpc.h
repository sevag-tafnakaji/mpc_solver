#ifndef MPC_H
#define MPC_H

#include <Eigen/Eigen>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>

class MPC
{
public:
    int horizon{}; // -1 for inf horizon

    MPC(int _horizon = -1) : horizon(_horizon) {};

    virtual Eigen::VectorXd get_input(Eigen::VectorXd state) {};

    virtual Eigen::VectorXd motion_model(Eigen::VectorXd state, Eigen::VectorXd input) {};

    std::pair<std::vector<Eigen::VectorXd>, std::vector<Eigen::VectorXd>> simulate(int N_steps, Eigen::VectorXd x_0, bool save_data, std::string file_name)
    {
        std::vector<Eigen::VectorXd> x;
        std::vector<Eigen::VectorXd> u;

        x.push_back(x_0);

        for (int i = 0; i < N_steps; i++)
        {
            auto sim_step = simulate_step(x[i]);
            x.push_back(sim_step.first);
            u.push_back(sim_step.second);
        }

        if (save_data)
            write_to_csv(x, u, file_name);

        return std::make_pair(x, u);
    }

private:
    std::pair<Eigen::VectorXd, Eigen::VectorXd> simulate_step(Eigen::VectorXd state)
    {
        Eigen::VectorXd input = get_input(state);
        return std::make_pair(motion_model(state, input), input);
    }

    // Save to csv to plot results with python
    void write_to_csv(std::vector<Eigen::VectorXd> x,
                      std::vector<Eigen::VectorXd> u,
                      std::string file_name)
    {

        int n_states = x[0].size();
        int n_inputs = u[0].size();

        std::ofstream csv_file;
        csv_file.open(file_name);
        for (int i = 1; i <= n_states; i++)
        {
            csv_file << "x" << i << ",";
        }
        for (int j = 1; j <= n_inputs; j++)
        {
            csv_file << "u" << j;
            if (j != n_inputs)
                csv_file << ",";
            else
                csv_file << "\n";
        }

        int num_elems = u.size();

        for (int elem_idx = 0; elem_idx < num_elems; elem_idx++)
        {

            for (int state_idx = 0; state_idx < n_states; state_idx++)
            {
                csv_file << x[elem_idx][state_idx] << ",";
            }
            for (int input_idx = 0; input_idx < n_inputs; input_idx++)
            {
                csv_file << u[elem_idx][input_idx];
                if (input_idx != n_inputs - 1)
                    csv_file << ",";
                else
                    csv_file << "\n";
            }
        }
        csv_file.close();
    }
};

class LinearMPC : public MPC
{

public:
    LinearMPC(int _horizon, Eigen::MatrixXd _A, Eigen::MatrixXd _B,
              Eigen::MatrixXd _P_f, Eigen::MatrixXd _Q, Eigen::MatrixXd _R) : MPC{_horizon}
    {

        A = _A;
        B = _B;
        P_f = _P_f;
        Q = _Q;
        R = _R;
        N_STATES = static_cast<int>(_A.rows());
        N_INPUTS = static_cast<int>(_B.cols());

        // TODO: Add size checks before running to make eventual debugging easier.
    };

    Eigen::VectorXd motion_model(Eigen::VectorXd state, Eigen::VectorXd input)
    {
        // TODO: Add size checks before running to make eventual debugging easier.

        return A * state + B * input;
    }

    // TODO: Change it from vectors to only current + prev values
    Eigen::MatrixXd DP_Riccati()
    {
        std::vector<Eigen::MatrixXd> K_k; // N_INPUTS x N_STATES
        std::vector<Eigen::MatrixXd> P_k; // N_STATES x N_STATES

        P_k.push_back(P_f);

        // Course book goes from N to 0, therefore vector must be flipped at the end.
        for (int i = 0; i < horizon; i++)
        {
            Eigen::MatrixXd temp_k = -(R + B.transpose() * P_k[i] * B).inverse() * B.transpose() * P_k[i] * A; // N_INPUTS x N_STATES
            Eigen::MatrixXd temp_p = Q + A.transpose() * P_k[i] * A + A.transpose() * P_k[i] * B * temp_k;     // N_STATES x N_STATES
            K_k.push_back(temp_k);
            P_k.push_back(temp_p);
        }

        std::reverse(P_k.begin(), P_k.end());
        std::reverse(K_k.begin(), K_k.end());

        return K_k[0];
    }

    // Solved iteratively. Tested and provides the same results as MATLAB's idare command
    Eigen::MatrixXd Inf_Riccati()
    {
        // P = P_inf at the end
        Eigen::MatrixXd P = Q;  // N_STATES x N_STATES
        Eigen::MatrixXd P_next; // N_STATES x N_STATES

        double diff = std::numeric_limits<double>::max();

        int iter_idx = 0;
        int max_iter = 100000;
        float tolerance = 0.1f;

        while (diff > tolerance || iter_idx > max_iter)
        {
            iter_idx++;
            P_next = Q + A.transpose() * P * A - A.transpose() * P * B * (R + B.transpose() * P * B).inverse() * B.transpose() * P * A;
            diff = fabs((P_next - P).maxCoeff());
            P = P_next;
        }

        return -(R + B.transpose() * P * B).inverse() * B.transpose() * P * A;
    }

    Eigen::VectorXd get_input(Eigen::VectorXd state)
    {
        Eigen::MatrixXd K;
        // TODO: Add check for horizon == 0
        if (horizon > 0)
            K = DP_Riccati();
        else
            K = Inf_Riccati();

        return K * state;
    }

private:
    int N_STATES;
    int N_INPUTS;
    Eigen::MatrixXd A, B, P_f, Q, R;
};

#endif
