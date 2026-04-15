import os
import sys
import matplotlib
matplotlib.use("TkAgg")  # Necesario en WSL
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

IMAGE_DIR = "bin/marcas"

try:
    # -----------------------------
    # Leer argumentos: filas y columnas
    # -----------------------------
    if len(sys.argv) != 3:
        print("Uso: python3 show_images_dict.py <filas> <columnas>")
        sys.exit(1)

    rows = int(sys.argv[1])
    cols = int(sys.argv[2])

    # -----------------------------
    # Cargar imágenes PNG SIN ordenar
    # -----------------------------
    images = [
        os.path.join(IMAGE_DIR, f)
        for f in os.listdir(IMAGE_DIR)
        if f.lower().endswith(".png")
    ]

    max_images = rows * cols
    images = images[:max_images]

    # -----------------------------
    # Tamaño fijo por imagen
    # (hace que la separación visual sea constante)
    # -----------------------------
    img_size = 3  # pulgadas por imagen
    fig_width = cols * img_size
    fig_height = rows * img_size

    fig, axes = plt.subplots(rows, cols, figsize=(fig_width, fig_height))
    axes = axes.flatten()

    # -----------------------------
    # Dibujar imágenes
    # -----------------------------
    for i, ax in enumerate(axes):
        if i < len(images):
            img = mpimg.imread(images[i])
            ax.imshow(img, cmap="gray")            
            ax.set_title(os.path.basename(images[i]), fontsize=8)
        ax.axis("off")

    # -----------------------------
    # Espaciado CONSTANTE
    # -----------------------------
    plt.subplots_adjust(
        left=0.05,
        right=0.95,
        top=0.95,
        bottom=0.05,
        wspace=0.1,   # separación horizontal fija
        hspace=0.2    # separación vertical fija
    )

    plt.show(block=True)

except KeyboardInterrupt:
    print("Finalizando...")