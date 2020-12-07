/**
 * @author Jefferson Flores Herrera 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

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
	
	fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); 
	ftruncate(fd,SIZE);
	rptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(rptr == MAP_FAILED){
		printf("Map Failed\n");
		return -1;
	}
	
	rptr -> in	= 0;
	rptr -> out = 0;

	struct item nextProduced;
	struct tiempo tempTiempo;
	pid_t pid;	
	
	while(1){
		pid = fork();
		getHora(&tempTiempo);

		if(pid < 0){
			fprintf(stderr, "fork fallo");
			return 1;//error
		} else if(pid == 0){
			nextProduced.pid = getpid();
			nextProduced.horaCreacion = tempTiempo;
			struct tiempo messageTiempo;
			getHora(&messageTiempo);
			printf("[%d:%d:%d.%d] P(%d): Creando Proceso %d (%d:%d:%d.%d)\n", messageTiempo.hora, messageTiempo.minuto, messageTiempo.segundo, messageTiempo.milisegundo , getppid(), nextProduced.pid, tempTiempo.hora, tempTiempo.minuto, tempTiempo.segundo, tempTiempo.milisegundo);
		} else{
			wait(NULL);
		}

		while((((rptr -> in ) + 1) % BUFFER_SIZE) == (rptr -> out))
			;//buffer lleno no hacer nada
		rptr -> buffer[rptr -> in] = nextProduced;
		rptr -> in = ((rptr -> in) + 1) % BUFFER_SIZE;

		int r = rand() % 5 + 1;
		sleep(r);
	}
	
	return 0;
}
