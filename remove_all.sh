#!/bin/bash

set -e  # Detiene el script si algún comando falla

for dir in Practica_1 Practica_2 Practica_3; do
    echo "Entrando en $dir ..."
    cd "$dir"

    echo "Limpiando directorio"
    make clean

    cd ..
done

echo "Todas las prácticas limpias"