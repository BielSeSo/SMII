import os
import matplotlib
matplotlib.use("TkAgg")   # Necesario en WSL
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

IMAGE_DIR = "bin/marcas"

# Cargar imágenes PNG SIN ORDENAR
images = [
    os.path.join(IMAGE_DIR, f)
    for f in os.listdir(IMAGE_DIR)
    if f.lower().endswith(".png")
]

# Matriz fija 4x6
rows = 4
cols = 4
max_images = rows * cols

images = images[:max_images]  # máximo 24 imágenes

fig, axes = plt.subplots(rows, cols, figsize=(cols * 3, rows * 3))
axes = axes.flatten()

for i, ax in enumerate(axes):
    if i < len(images):
        img = mpimg.imread(images[i])
        ax.imshow(img)
        ax.set_title(os.path.basename(images[i]), fontsize=8)
    ax.axis("off")

plt.tight_layout()
plt.show(block=True)
