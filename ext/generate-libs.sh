#!/bin/bash

# Directorio destino fijo
DEST_DIR="lib64"

# Lista de archivos a copiar
FILES=(
    "/mingw64/bin/glew32.dll"
    "/mingw64/bin/SDL2.dll"
    "/mingw64/bin/SDL2_image.dll"
    "/mingw64/bin/libcairo-2.dll"
)

# Función para copiar las dependencias
copy_deps() {
    local file="$1"
    local dest="$2"
    local deps=$(ldd "$file" | grep "=>" | awk '{print $3}' | grep -v '^(')

    for dep in $deps; do
        if [ -f "$dep" ]; then
            if [[ "$dep" =~ ^/c/ ]]; then
                # Omitiendo dependencia de sistema
                continue
            fi
            local dep_base=$(basename "$dep")
            if [ ! -f "$dest/$dep_base" ]; then
                cp -v "$dep" "$dest"
                # Recursivamente copiar las dependencias de la dependencia
                copy_deps "$dep" "$dest"
            fi
        fi
    done
}

# Función para iniciar la copia de cada archivo
copy_files() {
    local dest_dir="$DEST_DIR"
    
    # Verifica si el directorio destino existe, si no, lo crea
    if [ ! -d "$dest_dir" ]; then
        echo "El directorio destino $dest_dir no existe, creándolo..."
        mkdir -p "$dest_dir"
    fi

    for file in "${FILES[@]}"; do
        if [ ! -f "$file" ]; then
            echo "El archivo especificado $file no existe."
            continue # Salta al siguiente archivo si este no existe
        fi

        local file_base=$(basename "$file")
        if [ ! -f "$dest_dir/$file_base" ]; then
            cp -v "$file" "$dest_dir"
        fi

        # Copiar las dependencias
        copy_deps "$file" "$dest_dir"
    done
}

rm ${DEST_DIR}/*

# Inicia el proceso de copia
copy_files

# Directorio destino fijo
DEST_DIR="lib32"

# Lista de archivos a copiar
FILES=(
    "/mingw32/bin/glew32.dll"
    "/mingw32/bin/SDL2.dll"
    "/mingw32/bin/SDL2_image.dll"
    "/mingw32/bin/libcairo-2.dll"
)

copy_files

ls lib64/ | xargs -i cp /mingw32/bin/{} lib32/

cp /mingw32/bin/libgcc_s_dw2-1.dll lib32/