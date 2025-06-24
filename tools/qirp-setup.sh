#/bin/bash

# Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear
# current dir
ROS_DISTRO=jazzy


apt_packages_base=( \
    ros-$ROS_DISTRO-ros-base \
    ros-$ROS_DISTRO-cv-bridge \
    ros-$ROS_DISTRO-vision-msgs \
    ros-$ROS_DISTRO-sensor-msgs \
    ros-$ROS_DISTRO-usb-cam  \
    ros-$ROS_DISTRO-rclcpp  \
    ros-$ROS_DISTRO-std-msgs \
    ros-$ROS_DISTRO-fastcdr  \
    ros-$ROS_DISTRO-rosidl-default-runtime  \
    ros-$ROS_DISTRO-ocr-msg  \
    ros-$ROS_DISTRO-rclcpp-components  \
    ros-$ROS_DISTRO-rcutils  \
    qnn-tools \
    libqnn-dev \
    libqnn1 \
    # libsndfile1-dev \
    # libhdf5-dev \
    tesseract-ocr \
    libopencv-dev \
    python3-numpy \
    python3-opencv \
    python-is-python3 \
    python3-pip \
    python3-colcon-common-extensions \
    python3-pandas \
    wget \
    libpulse-dev \
    python3-requests \
    vim \
    unzip \
    v4l-utils \
)
#qrb ros packages
apt_packages_base+=( \
    ros-$ROS_DISTRO-qrb-ros-transport-image-type \
    ros-$ROS_DISTRO-qrb-ros-transport-imu-type \
    ros-$ROS_DISTRO-qrb-ros-transport-point-cloud2-type \
    ros-$ROS_DISTRO-lib-mem-dmabuf \
    ros-jazzy-orbbec-camera  \
    ros-jazzy-orbbec-camera-msgs \
    ros-jazzy-qrb-ros-transport-point-cloud2-type \
    ros-jazzy-rplidar-ros \
    ros-jazzy-sample-hand-detection \
    ros-jazzy-sample-resnet101-quantized \
    ros-jazzy-simulation-sample-amr-simple-motion \
    ros-$ROS_DISTRO-ocr-msg  \
    ros-$ROS_DISTRO-ocr-service  \
    ros-$ROS_DISTRO-qrb-ros-system-monitor  \
    ros-$ROS_DISTRO-qrb-ros-system-monitor-interfaces  \
    qcom-battery-client  \  
    qcom-battery-service  \
    ros-$ROS_DISTRO-qrb-ros-battery  \
    ros-$ROS_DISTRO-qrb-ros-imu  \
    
)
model=()
model_label=()

pip_packages_base=( \
    typing_extensions  \
    pytesseract \
)

qrb_ros_node=( \
)

apt_packages_sample=()
pip_packages=()

DIR=/home/ubuntu/
#--------setup in ubuntu -----------
function ubuntu_setup(){
    try_times=5     
    if [ ! -f "$DIR/env_check" ]; then
        #git clone https://github.qualcomm.com/QUIC-QRB-ROS/qrb_ros_samples.git
        scripts_env_setup
        download_qrb_ros_node $try_times
        touch $DIR/env_check
    fi    
    download_depends
    setup_env
}
function setup_env(){
    source /opt/ros/$ROS_DISTRO/setup.sh
    export USB_CAMERA_PATH=$(v4l2-ctl --list-devices | grep -A1 "USB Camera" | tail -n1 | tr -d ' \t')
}
function scripts_env_setup(){
    
    sudo apt update && sudo DEBIAN_FRONTEND=noninteractive apt-get install -y software-properties-common curl 
    if [ $? -eq 0 ]; then
        echo "apt-get install -y  software-properties-common curl successfully "
    else
        echo "apt-get install -y  software-properties-common curl  fail"
        exit 1
    fi
    
    #add qualcomm  ppa
    echo "Add qualcomm  ppa , wait few minutes..."
    #echo "deb [trusted=yes] http://10.64.25.66:8888 ./" | sudo tee -a /etc/apt/sources.list
    sudo add-apt-repository -y ppa:ubuntu-qcom-iot/qcom-noble-ppa
    sudo add-apt-repository -y ppa:ubuntu-qcom-iot/qirp 
        
    #install qnn libs in real ubuntu env, not in docker
    if [ -f /.dockerenv ]; then
        echo "current in docker , not install qnn and ros base"
        pip3 uninstall numpy
    else          
        #add ros apt source
        sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
        echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null  
        if [ $? -eq 0 ]; then
            echo "add ros base successfully "
        else
            echo "add ros base fail"
            exit 1
        fi
    fi
    sudo apt update && sudo apt upgrade -y

    echo "apt install base pkgs ${apt_packages_base[@]} ... "
    #install apt pkgs
    for package in "${apt_packages_base[@]}"; do
        sudo  DEBIAN_FRONTEND=noninteractive apt-get install -y $package   --fix-missing
        if [ $? -eq 0 ]; then
            echo "apt-get install -y  $package successfully "
        else
            echo "apt-get install -y  $package  fail"
            exit 1
        fi
    done
    
    echo "pip install base pkgs ${pip_packages_base[@]} ... "
    #can't pip install in ubuntu24.04 docker 
    if [ -f /usr/lib/python3.12/EXTERNALLY-MANAGED ]; then
        sudo mv /usr/lib/python3.12/EXTERNALLY-MANAGED  /usr/lib/python3.12/EXTERNALLY-MANAGED.bk
    fi
    
    #install pip pkgs
    for package in "${pip_packages_base[@]}"; do
        pip install $package
        if [ $? -eq 0 ]; then
            echo "pip install $package successfully "
        else
            echo "pip install $package fail"
            exit 1
        fi  
    done
    
        
    #install yq tool
    sudo wget -qO /usr/local/bin/yq https://github.com/mikefarah/yq/releases/latest/download/yq_linux_arm64
    if [ $? -eq 0 ]; then
        echo "add yq tool successfully "
    else
        echo "add yq tool fail"
        exit 1
    fi
    
    sudo chmod a+x /usr/local/bin/yq
}

function download_qrb_ros_node(){
    cd $DIR/src
    local N=$1
    if [ $((N -1)) == 0 ];then
        echo "try to git clone $node $try_times times, fail"
        exit 1
    fi

    for node in ${qrb_ros_node[@]};do
        if [ -f $DIR/src/$(echo $node | awk -F'/' '{print $NF}').done ]; then 
            echo "$node has been download in $DIR/src"
        else
            git clone $node
            if [ $? -eq 0 ]; then
                echo "git clone $node successfully "
                touch $DIR/src/$(echo $node | awk -F'/' '{print $NF}').done
            else
                download_qrb_ros_node $((N-1))
                exit 1
            fi
        fi
    done
}

function download_depends(){
    echo "apt install pkgs ${apt_packages[@]} ... "
    #install apt pkgs
    search_item=""
    for package in "${apt_packages[@]}"; do
        if [[ $package == $search_item ]];then
            echo "$package has been install"
        else
            sudo  DEBIAN_FRONTEND=noninteractive apt-get install -y $package   --fix-missing
            if [ $? -eq 0 ]; then
                echo "apt-get install -y  $package successfully "
            else
                echo "apt-get install -y  $package  fail"
                exit 1
            fi
            search_item+=$package
        fi
    done    
    echo "pip install base pkgs ${pip_packages[@]} ... "
    search_item=""
    #install pip pkgs
    for package in "${pip_packages[@]}"; do
        if [[ $package == $search_item ]];then
            echo "$package has been install"
        else
            sudo pip3 install --user --upgrade --force-reinstall $package
            if [ $? -eq 0 ]; then
                echo "pip install $package successfully "
            else
                echo "pip install $package fail"
                exit 1
            fi
            search_item+=$package
        fi
    done
}


function show_help() {
    echo "Usage: source /usr/share/qirp-setup.sh [OPTION]"
    echo ""
    echo "Options:"
    echo "  -h, --help        Show this help message."
    echo "  --update,         Force update qirp sdk."
    echo ""
    echo "Examples:"
    echo "  source /usr/share/qirp-setup.sh --help"
    echo "  source /usr/share/qirp-setup.sh --update"
}

#--------------main point--------------#
main(){
    
    case "$1" in
        -h|--help)
            show_help
            return 1
            ;;
        --update)
            scripts_env_setup
            if [[ $? -eq 0 ]]; then
                echo "setup qirp sdk successfully!"
            else
                echo "Something went wrong. Please check error info."
                return 1
            fi
            ;;       
        *)
            echo "Setting up QIRP QCLinux for execution on device"
        ;;
    esac
    ubuntu_setup
    echo "Setting up QIRP QCLinux successfully"
}

main $1 $2