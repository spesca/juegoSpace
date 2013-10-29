#include "library.hpp"
#include "utilidades_configuracion.hpp"
#include "claseJugador.hpp"

void asignarPartidas(){
	char cadenallave[30];
	char jug1[2];
	char jug2[2];
	char semaforo[30];
	int i, j;

	while(acepto_conexiones){

	    P(semaforoMutex);
            for( i = 0 ; i < disponibles.size() ; i++ )
	      if( disponibles[i] == true )
	       for( j = 0 ; j < disponibles.size() ; j++)
		 if( i != j )
		   if( disponibles[j]== true)
		     if( matrizPartidas[i][j]== 0 && matrizPartidas[j][i]==0){

			partidas++;

			//Ya encontre los dos jugadores que se van a enfrentar
			//El proceso crea un hijo "Partida"

			disponibles[i] = false;
		      	disponibles[j] = false;

		      	if( i < j )
				matrizPartidas[i][j] = 1;
			else
				matrizPartidas[j][i] = 1;

			printf("Partida asignada a los jugadores: %d (%s) y %d (%s).\n", i , jugadores[i].getNombre(),j,jugadores[i].getNombre());
			if( ( pid[partidas-1] = fork() ) == 0 ){
			//Creamos un Servidor de Partida (Hijo)
			    sprintf( cadenallave , "%d", llave);
			    sprintf( jug1 , "%d", i);
			    sprintf( jug2 , "%d", j);
			    strcpy ( semaforo , "/semaforoMutex");
	 	 	    execlp( "../partida/servidorPartida" , "servidorPartida" , cadenallave , jug1, jug2, semaforo, NULL);
			    perror( "Fallo el exec.\n");
			}

		  } //fin if
           V(semaforoMutex);
	}//fin del while
}

//Creamos la memoria compartida
void crearMemoriaCompartida(){
	id_memo = shmget( llave ,1024 /* sizeof(Partida)* 4 CREAR LA CLASE PARTIDA Y DESCOMENTAR*/, IPC_CREAT | IPC_EXCL | 0660); 
	if(id_memo < 0){
		fprintf(stderr , "Error al crear la memoria compartida.\n");
		exit(0);
	}

	//info = (t_info*) shmat( id_memo , 0, 0); VER COMO TRABAJARLO
}


//Cerramos la memoria compartida
void cerrarMemoriaCompartida(){
	//shmdt(info); VER COMO LO TRABAJO
	shmctl( id_memo, IPC_RMID , 0);
}

//Inicializar matriz de partidas
void inicializarMatriz(){
	int i, j;

	for( i = 0 ; i < TAM ; i++){
		for( j = 0 ; j < TAM ; j++){
			matrizPartidas[i][j] = 0;
		}
	}

}


//Capturamos las señales
void capturarSeniales(){
	//signal(SIGINT,SIG_IGN); COLOCAR EL HANDLER
	signal(SIGTERM,SIG_IGN);
	signal(SIGABRT,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	signal(SIGPIPE,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGSEGV,SIG_IGN);
	signal(SIGUSR1,SIG_IGN);
	signal(SIGUSR2,SIG_IGN);
	signal(SIGALRM,SIG_IGN);
}

//Esperamos a los Procesos Hijos (Partidas)
void esperarHijos(){
	int i;
	for( i = 0 ; i < partidas ; i++ )
		waitpid ( pid[i] , NULL , 0 );
}


//Disasociamos los punteros y eliminamos los semáforos
void cerrarSemaforos(){
	sem_close(semaforoMutex);
	sem_unlink("/semaforoMutex");
}


//Primitiva P()
void P(sem_t * semaforo){
	//sem_wait(semaforo);
}


//Primitiva V()
void V(sem_t * semaforo){
	//sem_post(semaforo);
}

//Cerramos los Socket de Comunicación
void cerrarSocketDeComunicacion(){
	char buffer[TAMBUF];
	int i;
	for( i = 0 ; i < jugadores.size() ; i++){
		strncpy( buffer , "Se ha cerrado el servidor" , TAMBUF);
		send( jugadores[i].getFileDescriptor() , buffer, TAMBUF ,0);
		close( jugadores[i].getFileDescriptor());
	}
}


//Hilo que se encarga de Aceptar Conexiones
void* aceptarConexiones(void * param){

	t_config_server* configuracion = (t_config_server*) param;

	//Creamos el Socket de Escucha
	int comm_socket = 0;
	unsigned short int listen_port = 0;
	unsigned long int listen_ip_address = 0;
	struct sockaddr_in listen_address, con_address;
	socklen_t con_addr_len;
	char buffer[TAMBUF];
	int optval = 1 , i = 0;

	listen_socket = socket(AF_INET,SOCK_STREAM,0);
	if(listen_socket < 0){
		fprintf(stderr , "Error al crear el Socket de Escucha.\n");
		exit(0);
	}

	setsockopt(listen_socket , SOL_SOCKET , SO_REUSEADDR , &optval, sizeof(optval));

	//Asigno dirección y configuracion.puerto_escucha
	bzero(&listen_address , sizeof(listen_address));
	listen_address.sin_family = AF_INET;
	listen_port = htons(configuracion->puerto_escucha);
	listen_address.sin_port = listen_port;
	listen_ip_address = htonl(INADDR_ANY);
	listen_address.sin_addr.s_addr = listen_ip_address;

	bzero(&(listen_address.sin_zero),8);

      if((bind(listen_socket,(struct sockaddr *)&listen_address,sizeof(struct sockaddr)))<0){
		fprintf(stderr , "Error al asignar la Dirección IP.\n");
		exit(0);
    	}

	//Comenzamos a escuchar conexiones
	if( (listen(listen_socket,MAXQ)) < 0 ){
		fprintf(stderr , "Error al escuchar conexiones.\n");
		exit(0);
	}

	bzero(&con_address,sizeof(con_address));
	con_addr_len = sizeof(struct sockaddr_in);

	//Aceptamos conexiones
	while(!fin_conexiones){
		comm_socket = accept(listen_socket,(struct sockaddr *)&con_address, &con_addr_len);

		pthread_mutex_lock( &mutex_acepto_conexiones );

		if( acepto_conexiones ){


			//Le enviamos esto al Cliente
			strncpy( buffer , "Te has conectado al servidor" , TAMBUF);
			send( comm_socket , buffer, TAMBUF ,0);

			//Si ya hay 2 jugadores, comienza a correr el tiempo del torneo
			P(semaforoMutex);
				if(jugadores.size() == 2){
					printf("Comienza el torneo.\n");
				}
			V(semaforoMutex);
			
			strcpy(buffer,"\0");
			if(recv(comm_socket, buffer, MAX_LETRAS ,0) > 0){
				//Inicializamos los datos del nuevo jugador
				P(semaforoMutex);
					inicializarJugador(i , comm_socket, buffer /*contiene nombre del jugador*/);
				V(semaforoMutex);

				printf("%s:, cantidad de conectados %d\n", inet_ntoa(con_address.sin_addr) , jugadores.size());

				cout << "se ha conectado : " << jugadores[i].getNombre() <<endl;
				
			}
		}
		else {
			strncpy( buffer , "No se aceptan más conexiones" , TAMBUF);
			send( jugadores[i].getFileDescriptor() , buffer, TAMBUF ,0);
		}

		pthread_mutex_unlock( &mutex_acepto_conexiones );

	i++;

	} //fin while */
	pthread_exit(NULL);
}

//Inicializamos los datos del Cliente que acaba de conectarse
void inicializarJugador(int id , int numSocket, char * nombre){
	//GUARDAR DATOS EN EL OBJETO DEL CLIENTE
}
