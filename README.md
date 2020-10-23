# OperativeSystems
## Laboratorio 2

Lo que hacemos es crear un proceso padre, que crea un proceso hijo, que crea un proceso nieto. El proceso padre no termina hasta que hijo se termine de ejecutar, por lo que hijo envia 14 señales a nieto listadas en el array senialesEnviar en hijo.c, y nieto recibe señales, listadas en recibirSeniales, si nieto recibe una señal que lo mata, entonces hijo crea otro hijo(que sería un nuevo nieto), sucesivamente hasta terminar de enviar todas las señales para que termine la ejecución.

# Compilar
Para compilar primero ejecute padre.c, luego hijo,c y luego nieto.c
