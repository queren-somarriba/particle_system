#!/bin/bash

IMAGE_NAME="ps-ubuntu"
CONTAINER_NAME="ps-container"

if [[ "$(podman images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
    echo "Construction de l'image de dev..."
    cat <<EOF > Dockerfile.ps
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential zsh git libglfw3-dev \
    libgl1-mesa-dev libx11-dev libxrandr-dev \
    libxinerama-dev libxcursor-dev libxi-dev mesa-utils \
    opencl-headers ocl-icd-opencl-dev clinfo \
    intel-opencl-icd pocl-opencl-icd
EOF
    podman build -t $IMAGE_NAME -f Dockerfile.ps
    rm Dockerfile.ps
fi

if ! podman ps -a --format "{{.Names}}" | grep -q "$CONTAINER_NAME"; then
    echo "Création du conteneur..."
    podman create \
        --name "$CONTAINER_NAME" \
        --env DISPLAY=$DISPLAY \
        --device /dev/dri:/dev/dri \
        --security-opt label=disable \
        --volume /tmp/.X11-unix:/tmp/.X11-unix:ro \
        --volume $XAUTHORITY:/root/.Xauthority:ro \
        --env XAUTHORITY=/root/.Xauthority \
        --volume "$(pwd):$(pwd):Z" \
        --workdir "$(pwd)" \
        --net=host \
        --ipc=host \
        --group-add keep-groups \
        -it $IMAGE_NAME zsh
    fi

podman start "$CONTAINER_NAME"
podman exec -it "$CONTAINER_NAME" zsh