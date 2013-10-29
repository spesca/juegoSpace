#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <list>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <queue>
#include "claseJugador.hpp"

#define TAM 100
#define MAXQ 10
//tamaño maximo del nombre del jugador
#define MAX_LETRAS 15
#define TAMBUF 1024

//Variables Globales
using namespace std;
vector<bool> disponibles;
vector<Jugador> jugadores;
int matrizPartidas[TAM][TAM];

//Variable que contendra un valor random para utilizar en la memoria compartida
key_t llave;
int id_memo;
bool acepto_conexiones = true;
int listen_socket = 0;
pthread_t hiloEscucha;
pthread_mutex_t mutex_acepto_conexiones = PTHREAD_MUTEX_INITIALIZER;
pid_t pid[TAM*TAM];
int partidas=0;
sem_t* semaforoMutex;
//Variable que indica si el server puede seguir aceptando conexiones
bool fin_conexiones = false;


//Prototipos de funciones
void asignarPartidas();
void crearMemoriaCompartida();
void cerrarMemoriaCompartida();
void inicializarMatriz();
void capturarSeniales();
void cerrarSemaforos();
void P(sem_t*);
void esperarHijos();
void inicializarJugador(int id , int numSocket, char * nombre);
void cerrarMemoriaCompartida();
void V(sem_t*);
void cerrarSocketDeComunicacion();
void* aceptarConexiones(void * param);
