#include "CargadorXML.h"

//Constantes para crear el archivo en caso de que no exista.
#define MAXCLIENTES "7"
#define IP "127.0.0.1"
#define PUERTO "8080"

XMLLoader::XMLLoader() {
	this->errorLogFile.open("ErroresNombreXML", ios_base::app);
}

void XMLLoader::cargarServidor(string nombreArchivo){
	if(!this->xmlDocument.LoadFile(nombreArchivo.c_str()))
	{
		//Escribo en el file de errores.
		this->errorLogFile <<"El archivo '"<<nombreArchivo<< "' no existe. Se creó un archivo con ese nombre.\n";
		//abro un archivo nuevo con valores estander.
		TiXmlDocument archivoNuevo;
		TiXmlElement* servidor = new TiXmlElement("Servidor");
		archivoNuevo.LinkEndChild(servidor);
		TiXmlElement* maxClientes = new TiXmlElement("CantidadMaximaClientes");
		servidor->LinkEndChild(maxClientes);
		TiXmlText* cantidadClientes = new TiXmlText(MAXCLIENTES);
		maxClientes->LinkEndChild(cantidadClientes);
		TiXmlElement* puerto = new TiXmlElement("Puerto");
		TiXmlText* numeroPuerto = new TiXmlText(PUERTO);
		puerto->LinkEndChild(numeroPuerto);
		servidor->LinkEndChild(puerto);

		archivoNuevo.SaveFile(nombreArchivo.c_str());
		this->xmlDocument.LoadFile(nombreArchivo.c_str());
	}
}

void XMLLoader::cargarCliente(string nombreArchivo){
	if(!this->xmlDocument.LoadFile(nombreArchivo.c_str()))
		{
			//Escribo en el file de errores.
			this->errorLogFile <<"El archivo '"<<nombreArchivo<< "' no existe. Se creó un archivo con ese nombre.\n";
			//abro un archivo nuevo con valores estander.
			TiXmlDocument archivoNuevo;
			TiXmlElement* cliente = new TiXmlElement("Cliente");
			archivoNuevo.LinkEndChild(cliente);
			TiXmlElement* conexion = new TiXmlElement("Conexion");
			TiXmlElement* ipElem = new TiXmlElement("Ip");
			TiXmlText* ip = new TiXmlText(IP);
			ipElem->LinkEndChild(ip);
			TiXmlElement* puertoElem = new TiXmlElement("Puerto");
			TiXmlText* puerto = new TiXmlText(PUERTO);
			puertoElem->LinkEndChild(puerto);
			conexion->LinkEndChild(ipElem);
			conexion->LinkEndChild(puertoElem);
			cliente->LinkEndChild(conexion);

			TiXmlElement* mensajes = new TiXmlElement("Mensajes");
			TiXmlElement* mensajeUno = new TiXmlElement("Mensaje");
			TiXmlElement* idElem = new TiXmlElement("Id");
			TiXmlElement* tipoElem = new TiXmlElement("Tipo");
			TiXmlElement* valorElem = new TiXmlElement("Valor");
			TiXmlText* id = new TiXmlText("mensaje1");
			TiXmlText* tipo = new TiXmlText("string");
			TiXmlText* valor = new TiXmlText("Hola mundo");
			idElem->LinkEndChild(id);
			tipoElem->LinkEndChild(tipo);
			valorElem->LinkEndChild(valor);
			mensajeUno->LinkEndChild(idElem);
			mensajeUno->LinkEndChild(tipoElem);
			mensajeUno->LinkEndChild(valorElem);
			mensajes->LinkEndChild(mensajeUno);
			cliente->LinkEndChild(mensajes);
			archivoNuevo.SaveFile(nombreArchivo.c_str());
			this->xmlDocument.LoadFile(nombreArchivo.c_str());
		}else{
			cout<<"carga exitosa\n";
	}
}

TiXmlDocument* XMLLoader::getDocumento(){
	return (&this->xmlDocument);
}

XMLLoader::~XMLLoader() {
	this->xmlDocument.Clear();
	this->errorLogFile.close();
}
