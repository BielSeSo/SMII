#!/bin/bash
echo "============================================"
echo "          Iniciando compilación             "
echo "============================================"
echo ""

make all

if [ $? -ne 0 ]; then
    echo "!!! Error en la compilación !!!!"
    sleep 1

    echo "============================================"
    echo "    Descargando librerias requeridas        "
    echo "============================================"
    echo ""

    sudo apt update
    sudo apt install -y \
        libassimp-dev \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        freeglut3-dev \
        libopencv-dev \
        libopenal-dev \
        libalut-dev

    echo ""
    echo ""
    echo "========================================================================"
fi

echo ""
echo "============================================"
echo "           Programa compilado               "
echo "============================================"
echo ""
