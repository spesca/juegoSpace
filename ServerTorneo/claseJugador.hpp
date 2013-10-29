#ifndef JUGADOR_HPP
#define JUGADOR_HPP

#include <iostream>
#include <fcntl.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class Jugador
{
	public:
		
		//METODOS
		Jugador();
		char * getNombre() {return nombre;}
		int getFileDescriptor() {return fileDescriptor;}

		
	private:
		int id; //id de caja jugador
		int fileDescriptor;
		char * nombre;
		int puntosPartida;
		int puntosTorneo;
		int disponible;	
};

#endif /* JUGADOR_HPP */
