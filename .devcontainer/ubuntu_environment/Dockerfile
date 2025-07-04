FROM ubuntu:noble

RUN apt-get update && apt-get install -y \
    nano && \
    rm -rf /var/lib/apt/lists/*

ARG USERNAME=mpc_solver
ARG USER_UID=1001
ARG USER_GID=$USER_UID

# Create a non-root user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && mkdir /home/$USERNAME/.config && chown $USER_UID:$USER_GID /home/$USERNAME/.config

# Set up sudo
RUN apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && rm -rf /var/lib/apt/lists/*

# Switch from root to user
USER $USERNAME

# Update all packages
RUN sudo apt update && sudo apt upgrade -y

# Install some extra packages
RUN sudo apt-get update \
    && sudo apt install -y git \
    && sudo apt install -y cmake \
    && sudo apt install -y build-essential \
    && sudo apt install -y g++-14 \
    && sudo rm -rf /var/lib/apt/lists/*

# Add build alias
RUN echo "alias build='cd /mpc_solver/build && cmake .. && cmake --build . && cd -'" >> ~/.bashrc
