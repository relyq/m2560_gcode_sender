# m2560_gcode_sender

en vez de que SendGcodeFile y SendGcodeLine sean de mayor prioridad y bloqueen la pantalla tactil, cuando inicia SendGcodeFile debería suspender TouchscreenMenu y reanudarla cuando termine de enviar el archivo

file explorer:<br>se deberia poder explorar adentro de carpetas

hay mejores formas de saber si algun boton fue apretado, por ejemplo https://softwareengineering.stackexchange.com/questions/401661/whats-a-proper-way-to-design-a-gui-event-system#:~:text=It%20is%20better,this%2C%20not%20sure.

siento que ninguna coordenada deberia estar hardcodeada. tengo que pensar en esto. quiza deberian ser constantes en un archivo separado (screen_defs) - esto me puede ayudar a relacionar distintos elementos del UI con la misma coordenada.

tiene que haber algo en la pantalla de configuracion

debería mostrar una pantalla diferente cuando se está realizando el trabajo

puedo hacer que la máquina se mueva al centro de la primera tuerca y haga un probe en el preambulo del gcode

en vez de hacer un homing cycle cada vez que se termina el trabajo solo tengo que volver al punto del homing para no gastar los limit switches

mejorar control de movimiento

agregar control de feedrate

si uso las funciones de SdFat probablemente puedo eliminar las variables globales de los archivos y dejar todo mas tidy

necesito una task que maneje el cambio de pantallas cuando se toca la pantalla (taskTouchscreenMenu) y otra task que maneje la actualización del dashboard - esto esta complicado pq tengo que compartir recursos del display entre tasks

probablemente voy a necesitar una task para elegir el archivo de la sd - realmente deberia hacer esto

una task que de lo único que se encargue es de actualizar el dashboard (no maneje entrada de touchscreen), de forma que cuando dibujo el dashboard la reanudo y cuando dibujo otra pantalla la suspendo. así debería manejar cualquier pantalla que necesite actualizar el display sin interacción. - muy complicado por compartir recursos

tengo que hacer una mejor forma de asegurarme de que no le doy un ptr ilegal a drawHomeScreen()

solo se puede sacar la SD con el programa apagado

por qué drawSDScreen() funciona con char (*filenames)[20]? tengo otra forma de hacer que acepte como parametro un char filenames[][]? - ya lo cambie, lo correcto era usar un char**. aun asi sigo teniendo la duda

<s>debería plantearme hacer un archivo para cada task por cuestión de orden</s>

<s>el programa no debería pasar de setup() si no se encuentra pantalla o tarjeta sd</s>

<s>debería guardar la lista de archivos al principio del programa en vez de contarlos cada vez que lo necesito</s>

<s>puedo hacer que cuando se toca la pantalla de trabajo terminado la máquina haga un homing cycle automáticamente</s>

<s>control de movimiento</s>

<s>la pantalla de SD deberia aceptar todos los archivos que hayan - deberia crear pantallas cuando se necesite para mostrar mas archivos</s>

<s>taskTouchscreenMenu.cpp:184: para saber se existe el archivo en la pantalla sd, filecount % files_per_page me dice cuantos archivos hay en la ultima pagina, además, si no estoy en la ultima pagina siempre van a existir todos los archivos que se pueden mostrar</s>
