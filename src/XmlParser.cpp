#include "XmlParser.h"

XmlParser::XmlParser(TiXmlDocument* doc) {
	this->doc = doc;
}

void XmlParser::obtenerMensaje(clientMsj &mensaje, int nroMensaje){
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
	}else
		cout<<"Error al obtener el mensaje";
}

void XmlParser::obtenerPuertoSv(int &puerto){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* puertoElem = docHandle.FirstChild("Servidor").FirstChild("Puerto").ToElement();

	if(puertoElem)
		puerto = atoi(puertoElem->GetText());
	else
		cout<<"Error al obtener el puerto";
}

void XmlParser::obtenerPuertoCl(int &puerto){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* puertoElem = docHandle.FirstChild("Cliente").FirstChild("Conexion").FirstChild("Puerto").ToElement();

	if(puertoElem)
		puerto = atoi(puertoElem->GetText());
	else
		cout<< "Error al obtener el puerto";

}

void XmlParser::obtenerIp(string &ip){
	TiXmlHandle docHandle(this->doc);
	TiXmlElement* ipElem = docHandle.FirstChild("Cliente").FirstChild("Conexion").FirstChild("Ip").ToElement();

	if(ipElem)
		ip = string(ipElem->GetText());
	else
		cout<<"Error al obtener la ip";
}

void XmlParser::obtenerMaxClientes(int &maxClientes){
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

