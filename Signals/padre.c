#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	pid_t pid = fork(); //Creamos un hijo
	if(pid < 0){
		perror("No se pudo crear al hijo"); //Mensaje en caso no se puede crear al hijo
		exit(EXIT_FAILURE);
	}
	else if(pid == 0){
 		execlp("./hijo","hijo",(char*)NULL);
		perror("Hijo no se ha ejecutado\n"); //Si no encuentra hijo
		exit(EXIT_FAILURE);
	}
	else{
		int estadoHijo; // Nos ayudará a saber el estado del Hijo
		waitpid(-1, &estadoHijo, 0);
		if(WIFEXITED(estadoHijo)){ // Si hijo termina correctamente nos dará true
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}
