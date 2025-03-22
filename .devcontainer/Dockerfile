FROM ubuntu:noble

# Official workaround for "groupadd: GID '1000' already exists"
RUN userdel -r ubuntu

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install basic dependencies for compilation
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    gcc-multilib \
    g++-multilib \
    make \
    wget \
    unzip \
    git \
    gdb \
    libc6-dbg \
    valgrind \
    strace \
    clang-format \
    python3 python3-pip python3-venv \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create a virtual environment and install colour-valgrind
RUN python3 -m venv /opt/venv && \
    /opt/venv/bin/pip install colour-valgrind

# Add the virtual environment to PATH for all users
ENV PATH="/opt/venv/bin:$PATH"

# Download and install Premake5
RUN wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta5/premake-5.0.0-beta5-linux.tar.gz \
    && tar -xvf premake-5.0.0-beta5-linux.tar.gz \
    && mv premake5 /usr/local/bin/ \
    && chmod +x /usr/local/bin/premake5 \
    && rm premake-5.0.0-beta5-linux.tar.gz

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    #
    # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME
