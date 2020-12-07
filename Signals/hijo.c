#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <setjmp.h>

#define SIG_TOT 14 //Son 14 señales las que enviaremos
jmp_buf env; //Si nuestro proceso nieto muere usaremos este
int i = 0; //Para iterar por las señales

//Señales que enviaremos
int señalesEnviar[SIG_TOT] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGKILL, SIGSEGV, SIGPIPE, SIGALRM, SIGTERM, SIGCHLD, SIGSTOP, SIGTSTP};
				 
//Funcion para enviar Señales
void enviarSenial(int pidReceiver){
	if(i < 0 || i >=SIG_TOT){
		perror("No corresponde con la señal"); //Si pasa de 14
		exit(EXIT_FAILURE);
	}
	kill(pidReceiver,SIGCONT);
	int w;
	int estado;
	int estadoSeñalEnviada; // Para verificar si se envio correctamente la señal al pid indicado
	do{
		w = waitpid(pidReceiver, &estado, WCONTINUED);
	}while( !WIFCONTINUED(estado));
	do{
		printf("Enviando %d", señalesEnviar[i]);
		if(señalesEnviar[i] == SIGKILL){ //Esto lo hacemos porque SIGKILL no es modificable
			i++; // Por eso pasamos a la otra señal a ser enviada
			kill(pidReceiver, SIGKILL);
			break;
		}
		estadoSeñalEnviada = kill(pidReceiver,señalesEnviar[i]);
	}while(estadoSeñalEnviada!=0); 
	i++;
}

//Función para manejar el catcher de la señal SIGCHLD
void handlerSIGCHLD(int sig, siginfo_t *siginfo, void *context){
	if(sig == SIGCHLD){
		if(siginfo -> si_code == CLD_KILLED){ //si_code nos informa si un hijo ha sido matado por una señal
			printf("Mi hijo ha sido asesinado crare uno nuevo y seguire enviando seniales\n");
			longjmp(env,100);
		}
	}
}

int main(){
	struct sigaction act; // para manejar SIGCHILD
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = &handlerSIGCHLD;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGCHLD, &act, NULL);
	int estado;
	setjmp(env); //en caso niesto sea matado
	pid_t w,pid = fork();
	if(pid < 0){
		perror("No se pudo crear al nieto desde hijo\n");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0){
		printf("Creando nieto\n");
		execlp("./nieto","nieto",(char*)NULL);
		perror("Nieto no se ha ejecutado\n");
		exit(EXIT_FAILURE);
	}
	else{
	 	do{ //bucle para poder enviar todas las señales
			w = waitpid(pid, &estado, WUNTRACED);
			if(w == -1){ //si pid no existe
				continue;
				exit(EXIT_FAILURE);
			}
			else if(w == pid){
				if(WIFSTOPPED(estado)){
					enviarSenial(pid);
				}
			}
		}while(!WIFEXITED(estado));
	}
	return 0;
}
