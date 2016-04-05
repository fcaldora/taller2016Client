#include "XmlParser.h"

XmlParser::XmlParser(TiXmlDocument* doc, ofstream* archivoErrores) {
	this->doc = doc;
	this->archivoErrores = archivoErrores;
	this->archivoErrores->open("erroresXml", ios_base::out);
}

int XmlParser::cantidadMensajes(){
	int contador = 0;
	bool masMensajes = true;
	TiXmlHandle docHandle(this->doc);
	while(masMensajes){
		TiXmlElement* msj = docHandle.FirstChild("Cliente").FirstChild("Mensajes").Child("Mensaje", contador).ToElement();
		if(msj)
			contador++;
		else
			masMensajes = false;
	}
	return contador;
}

int XmlParser::obtenerMensaje(clientMsj &mensaje, int nroMensaje){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* msj = docHandle.FirstChild("Cliente").FirstChild("Mensajes").Child("Mensaje", nroMensaje).ToElement();
	if (msj){
		string id(msj->FirstChild("Id")->ToElement()->GetText());
		memset(mensaje.id,0,LONGCHAR);
		strncpy(mensaje.id, id.c_str(), id.size());
		string type(msj->FirstChild("Tipo")->ToElement()->GetText());
		memset(mensaje.type,0,LONGCHAR);
		strncpy(mensaje.type, type.c_str(), type.size());
		string value(msj->FirstChild("Valor")->ToElement()->GetText());
		memset(mensaje.value,0,LONGCHAR);
		strncpy(mensaje.value, value.c_str(), value.size());
		return 0;
	}else{
		cout<<"Error al obtener el mensaje. XML mal escrito.";
		*archivoErrores<<"Error al obtener el mensaje. Xml mal escrito."<<endl;
		return -1;
	}
}

void XmlParser::getServerPort(int &puerto){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* puertoElem = docHandle.FirstChild("Servidor").FirstChild("Puerto").ToElement();

	if(puertoElem)
		puerto = atoi(puertoElem->GetText());
	else
		cout<<"Error al obtener el puerto. XML mal escrito.";
}

int XmlParser::getClientPort() {
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* puertoElem = docHandle.FirstChild("Cliente").FirstChild("Conexion").FirstChild("Puerto").ToElement();

	if(puertoElem)
		return atoi(puertoElem->GetText());
	else{
		cout<< "Error al obtener el puerto. XML mal escrito.";
		*archivoErrores<<"Error al obtener el puerto. XML mal escrito."<<endl;
		return PUERTO_PREDEF;
	}
}

string XmlParser::getIP(){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* ipElem = docHandle.FirstChild("Cliente").FirstChild("Conexion").FirstChild("Ip").ToElement();

	if(ipElem)
		return string(ipElem->GetText());
	else{
		cout<<"Error al obtener la ip. XML mal escrito.";
		*archivoErrores<<"Error al obtener la ip. Xml mal escrito"<<endl;
		return "";
	}
}

void XmlParser::getMaxNumberOfClients(int &maxClientes){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* maxElem = docHandle.FirstChild("Servidor").FirstChild("CantidadMaximaClientes").ToElement();

	if (maxElem)
		maxClientes = atoi(maxElem->GetText());
	else
		cout<<"Error al obtener el numero maximo de clientes";
}

XmlParser::~XmlParser() {
	// TODO Auto-generated destructor stub
}

