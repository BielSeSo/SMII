#!/bin/bash

set -e  # Detiene el script si cualquier comando falla

# -------------------------------
# Función para compilar un target
# -------------------------------
compilar_target() {
    local nombre=$1
    echo "============================="
    echo "Compilando $nombre..."
    echo "============================="
    echo ""


    if make "$nombre" >/dev/null 2>&1; then
        echo "$nombre compilado correctamente."
    else
        echo "Error al compilar $nombre."
        instalar_dependencias "$nombre"
        echo "Reintentando compilación de $nombre..."
        make "$nombre"
        echo "$nombre compilado correctamente después de instalar dependencias."
    fi
    echo ""
}

# ---------------------------------------------------
# Instala dependencias según el binario que ha fallado
# ---------------------------------------------------
instalar_dependencias() {
    local nombre=$1

    echo "Instalando dependencias para $nombre..."

    case "$nombre" in
        pruebaGL)
            sudo apt update
            sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
            ;;
        pruebaCV)
            sudo apt update
            sudo apt install -y libopencv-dev
            ;;
        pruebaAL)
            sudo apt update
            sudo apt install -y libopenal-dev libalut-dev
            ;;
        *)
            echo "⚠ No hay reglas de instalación para $nombre"
            ;;
    esac
}

# -------------------------
# Compilar en Introducción
# -------------------------
echo "Entrando en Introduccion ..."
echo ""
echo ""
cd Introduccion

compilar_target "pruebaGL"
compilar_target "pruebaCV"
compilar_target "pruebaAL"

echo "===================================="
echo "Todo compilado correctamente."
echo "Ahora compilamos las practicas ..."
echo "===================================="
echo ""
echo ""

cd ..
sleep 5

# --------------------------
# Compilar prácticas 1 a 3
# --------------------------
for dir in Practica_1 Practica_2 Practica_3 Practica_4; do
    echo "========================="
    echo "Entrando en $dir ..."

    cd "$dir"

    echo "Ejecutando make all..."
    echo "========================="
    echo ""

    make all

    cd ..
    echo ""
    echo ""
done

echo "Todas las prácticas compiladas."
