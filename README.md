# m2560_gcode_sender

necesito una task que maneje el cambio de pantallas cuando se toca la pantalla (taskTouchscreenMenu) y otra task que maneje la actualización del dashboard
probablemente voy a necesitar una task para elegir el archivo de la sd

una task que de lo único que se encargue es de actualizar el dashboard (no maneje entrada de touchscreen), de forma que cuando dibujo el dashboard la reanudo y cuando dibujo otra pantalla la suspendo. así debería manejar cualquier pantalla que necesite actualizar el display sin interacción.

el programa no debería pasar de setup() si no se encuentra pantalla o tarjeta sd

debería guardar la lista de archivos al principio del programa en vez de contarlos cada vez que lo necesito