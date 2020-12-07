#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define SIG_TOT 12 // SIGKILL y SIGSTOP no se pueden redefinir en un handler

int recibirSeniales[SIG_TOT] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGPIPE,
				 SIGALRM, SIGTERM, SIGCHLD, SIGTSTP};

//Funcion que Maneja como se comportará cada señal, muestra la señal recibida
void handler(int sig, siginfo_t *siginfo, void *context){
	printf("Soy %d ", getpid());
	printf("Numero de Senial: %d Codigo de Senial: %d errno value: %d Enviando proceso ID: %d\n", siginfo -> si_signo, siginfo -> si_code, siginfo -> si_errno, siginfo -> si_pid);
	switch(sig){
		case SIGHUP:
			break;
		case SIGINT:
			break;
		case SIGQUIT:
			break;
		case SIGILL:
			break;
		case SIGABRT:
			break;
		case SIGFPE:
			break;
		case SIGSEGV:
			break;
		case SIGPIPE:
			break;
		case SIGALRM:
			break;
		case SIGTERM:
			break;
		case SIGCHLD:
			break;
		case SIGTSTP:
			exit(EXIT_SUCCESS);
			break;
		default:
			printf("Senial no soportada\n");
			exit(EXIT_FAILURE);
			break;
	}
}

//Funcion que instala señales que redefinimos
void installSignalsCatcher(struct sigaction *action){
	for(int i = 0; i < SIG_TOT; i++)
		sigaction(recibirSeniales[i], action ,NULL);
}

int main(){
	struct sigaction act;
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &handler;
	act.sa_flags = SA_SIGINFO;
	installSignalsCatcher(&act);
	while(1){
		kill(getpid(),SIGSTOP);
	}
	return 0;
}
