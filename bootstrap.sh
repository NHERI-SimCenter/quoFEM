#!/bin/bash

# Updates the package lists for upgrades and new package installations
sudo apt-get update

# Installs the 'sudo' package. The '-y' flag automatically answers yes to prompts.
apt-get install -y sudo python3-dev

# Install the needed packages
sudo apt install -y software-properties-common cmake python3-pip git

# Upgrades pip (Python package installer) to the latest version
sudo python3 -m pip install -U pip

# Installs the Conan package manager
pip3 install conan==1.60.1
pip3 install nheri-simcenter

python3 --version

which python3

which conan
