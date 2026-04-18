#!/bin/bash

set -e  # Detiene el script si algún comando falla

for dir in Introduccion Practica_1 Practica_2 Practica_3 Practica_4; do
    echo "=========================="
    echo "Entrando en $dir ..."
    cd "$dir"

    echo "Limpiando directorio"
    make clean
    echo "=========================="

    cd ..
    echo ""
    echo ""
done

echo "Todas las prácticas limpias"