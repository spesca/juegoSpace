#include "library.hpp"
#include "utilidades_configuracion.hpp"
#include "claseJugador.hpp"

pid_t padre = getpid();

int main(){

	//Inicializo la semilla del random utilizando el tiempo actual
	srand(time(NULL));
	//Obtengo un numero de llave random para la memoria compartida
 	llave = (key_t)(1 + (rand() % 255));

	//Estructura que contendra los datos de configuracion
	t_config_server configuracion;

	//Leo el archivo de configuracion, en caso de error,
	//el programa se cierra.
	leer_configuracion_server(&configuracion);

	crearMemoriaCompartida(); //Creamos la Memoria Compartida

	//Creamos los semáforos para la sincronización
	semaforoMutex = sem_open( "/semaforoMutex" , O_CREAT | O_EXCL , 0660 , 1);
	if( semaforoMutex == (sem_t*) -1){
		perror( "Error creando el semáforo mutex.\n");
		exit(0);
	}

	inicializarMatriz();

	capturarSeniales(); //Capturamos las señales

	//Creamos un hilo que se encargue de aceptar las conexiones
	if(pthread_create(&hiloEscucha,NULL , aceptarConexiones ,(void*)&configuracion) < 0){
	fprintf(stderr , "Error al crear el hilo que acepta conexiones.\n");
	exit(0);
   	}

	printf("Esperando conexiones . . . \n");

	asignarPartidas();

	//Ejecuta el Padre (Torneo)
	if( padre == getpid() ){
		esperarHijos();
		P(semaforoMutex);
			//mostrarEstadisticas();
			cerrarSocketDeComunicacion();
		V(semaforoMutex);
		cerrarMemoriaCompartida();
		cerrarSemaforos();
	}

	//JOIN DE LOS HILOS DONDE VA?
	pthread_join(hiloEscucha, NULL);

	printf("Termino el Torneo.\n");

	return 0;
}
