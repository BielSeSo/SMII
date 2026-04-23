# SMII
## Simulador de conducción

### Autores:
- Marco Robert Valverde
- Biel Selma Solans

### Iniciación
1) Antes de ejecutar nada instalar la carpeta `sources/` en la misma que esté la carpeta `code/`.

2) Ejecutar `./automount.sh` para que haga autodiagnostico de la compilación. En caso de que el ejecutable de problemas utilizar ```sudo chmod +x automount.sh```.

3) Una vez el programa haya instalado las líbrerias necesarias se puede o volver a ejecutar el **.sh** o hacer un **make all**.

### Funcionalidades
Por el momento, el juego cuenta con una interfaz funcional que se puede interactuar con teclado y ratón, como feedback recibe sonidos y los cambios de pantalla.

- Teclas funcionales interfaz
    - **1**: Selecciona la primera opción de todas las interfaces, incluye el back de los créditos.
    - **2**: Selecciona la segunda opción de todas las interfaces.
    - **3**: Selecciona la tercera opción de todas las interfaces.
    
- Teclas funcionales del videojuego
    - **W**: Aumenta la velocidad del kart.
    - **S**: Disminuye la velocidad del kart.
    - **D**: Girar a la derecha el kart.
    - **A**: Girar a la izquierda el kart.

- Teclas genéricas
    - **F**: Alterna la pantalla completa y el modo ventana.

- Botones del ratón:
    - **Botón izquierdo**: Sirve para seleccionar los botones de la interfaz, el el juego por el momento no tiene ninguna.
    - **Botón dercho**: Sirve para volver al menú de atrás, por le momento para volver solo está implementado en el ratón.

### Programas

Dentro del proyecto hay varias carpetas:

- `code/`: Contiene todo el código del proyecto. Dentro de este mismo está dividido en varios archivos.
    - `sound_maker.cpp`: Programa encargado de cargar los archivos de audio en los buffers, ejecutar los sonidos y destruir todos los recursos utilizados en el cierre del juego.
    - `interface.cpp`: Programa que genera los botones de la interfaz, que comprueba cual botón ha sido seleccionado por el ratón y que genera las imagenes que se muestran durante la carrera.
    - `game_render.cpp`: Programa que pone ha funcionar las interfaces y el videojuego.
    - `main.cpp`: Programa main que inicia OpenGL, OpenCV y OpenAL necesarios para el proyecto y el que ejecuta todos los loops de OpenGL.
    - `game/`:
        - `load_map.cpp`: Programa que genera el mapa para el juego.
        - `player.cpp`: Programa que genera el kart y el jugador controlable del juego.
        - `load_obj.cpp`: Programa que carga los elementos .obj y genera una lista para representarse por pantalla o obtiene los hitbox para calcular los diferentes límites.
    - `control_mando.cpp`: Programa que con vídeo en vivo busca si hay alguna de las marcas de ArUco del diccionario DICT_ARUCO_ORIGINAL con id del 0 al 4 y reaccionar en consecuencia de la función implementada.
    - `calibracion_camara`:
        - `calibracion_camara.cpp`: Programa basado en los ejemplos proporcionados por OpenCV para ArUco. Sirve para calibrar la cámara y generar un archivo para obtener la matriz intrínseca de esta.
        - `calibracion_camara.yml`: Archivo que contiene la matriz intrínseca de la cámara.

- `source/`:
    - `assets/`:
        - `models/`: Modelos de kart utilizados por el usuario dentro del juego.
        - `maps/`: Contiene todos los mapas del juego.
    - `images/`: Imágenes utilizadas en el proyecto.
    - `audios/`: Audios utilizados en el proyecto.

# TO DO:

Cosas posibles ha implementar:
- Comprobación del kart en la pista
- Animacón de botones de los karts y los mapas
- Hacer un final (Opcional, por el momento)
- Unificar el trabajo
