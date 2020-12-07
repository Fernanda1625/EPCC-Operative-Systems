/**
 * @author Jefferson Flores Herrera 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 10

struct tiempo{
	int hora, minuto, segundo, milisegundo;
};

void getHora(struct tiempo *tIn){
	struct timeval current_time;
	time_t t;
	int milisecs;
	struct tm* info;
	gettimeofday(&current_time, NULL);
	t = current_time.tv_sec;
	info = localtime(&t);
	milisecs = current_time.tv_usec / 1000;
	tIn -> hora = info -> tm_hour;
	tIn -> minuto = info -> tm_min;
	tIn -> segundo = info -> tm_sec;
	tIn -> milisegundo = milisecs;
}

struct tiempo diff( struct tiempo tInicial, struct tiempo tFinal){
	struct tiempo diffTiempo;
	if( tFinal.milisegundo < tInicial.milisegundo){
		tFinal.segundo -= 1;
		tFinal.milisegundo += 1000;
	}
	if(tFinal.segundo < tInicial.segundo){
		tFinal.minuto -= 1;
		tFinal.segundo += 60;
	}
	if(tFinal.minuto < tInicial.minuto){
		tFinal.hora -= 1;
		tFinal.minuto += 60;
	}
	diffTiempo.hora = tFinal.hora - tInicial.hora;
	diffTiempo.minuto = tFinal.minuto - tInicial.minuto;
	diffTiempo.segundo = tFinal.segundo - tInicial.segundo;
	diffTiempo.milisegundo = tFinal.milisegundo - tInicial.milisegundo;
	return diffTiempo;
}
struct item{
	pid_t pid;
	struct tiempo horaCreacion;
} ;

struct region{
	int in, out;
	struct item buffer[BUFFER_SIZE];
} ;

int main()
{
	srand(time(NULL));
	const int SIZE = sizeof(struct region);
	const char *name = "OS";
	struct region *rptr;
	int fd;

	/* open the shared memory segment */
	fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	rptr = mmap(NULL,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (rptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	/* now read from the shared memory region */
	struct item nextConsumed;
	while(1){
		while(rptr -> in == rptr -> out)
			; // buffer vacio no hacer nada
		nextConsumed = rptr -> buffer[rptr -> out];
		rptr -> out = ((rptr -> out) + 1) % BUFFER_SIZE;

		struct tiempo killTiempo;
		getHora(&killTiempo);	
		kill(nextConsumed.pid, SIGKILL);
		struct tiempo diffTiempo = diff(nextConsumed.horaCreacion,killTiempo);	
		printf("[%d:%d:%d.%d] C(%d): Matando Proceso %d (vivio %d:%d:%d.%d)\n",killTiempo.hora, killTiempo.minuto, killTiempo.segundo, killTiempo.milisegundo, getpid(), nextConsumed.pid, diffTiempo.hora, diffTiempo.minuto, diffTiempo.segundo, diffTiempo.milisegundo );
		int r = rand() % 5 + 1;
		sleep(r);
	}	
	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}

	return 0;
}
