#include "XmlParser.h"

XmlParser::XmlParser(const char* fileName) {
	TiXmlDocument xmlFile(fileName);
	if (xmlFile.LoadFile()) {
		this->doc = xmlFile;
	}
}

LogLevelType XmlParser::getLogLevel() {
	TiXmlHandle docHandle(&this->doc);
	TiXmlElement *configurationElement = docHandle.FirstChild(kClientTag).FirstChild(kConfigurationTag).ToElement();
	if (configurationElement == NULL)
		return LogLevelTypeOnlyErrors;
	TiXmlElement *logLevelElement = configurationElement->FirstChild(kLogLevelTag)->ToElement();
	if(logLevelElement == NULL)
		return LogLevelTypeOnlyErrors;

	const char* logLevelChar = logLevelElement->GetText();
	std::stringstream logLevelStrValue;
	logLevelStrValue << logLevelChar;
	unsigned int logLevelValue;
	logLevelStrValue >> logLevelValue;

	switch(logLevelValue){
	case 1:
		return LogLevelTypeOnlyErrors;
	case 2:
		return LogLevelTypeEverything;
	default :
		return LogLevelTypeOnlyErrors;
	}
}

int XmlParser::getNumberOfMessages(){
	int contador = 0;
	bool masMensajes = true;
	TiXmlHandle docHandle(&this->doc);
	while(masMensajes){
		TiXmlElement* msj = docHandle.FirstChild(kClientTag).FirstChild(kMessagesTag).Child(kMessageTag, contador).ToElement();
		if(msj)
			contador++;
		else
			masMensajes = false;
	}
	return contador;
}

int XmlParser::getMessage(clientMsj &mensaje, int nroMensaje){
	TiXmlHandle docHandle(&this->doc);
	TiXmlElement* msj = docHandle.FirstChild(kClientTag).FirstChild(kMessagesTag).Child(kMessageTag, nroMensaje).ToElement();
	if (msj){
		string id(msj->FirstChild(kMessageIDTag)->ToElement()->GetText());
		memset(mensaje.id,0,kLongChar);
		strncpy(mensaje.id, id.c_str(), id.size());
		string type(msj->FirstChild(kMessageTypeTag)->ToElement()->GetText());
		memset(mensaje.type,0,kLongChar);
		strncpy(mensaje.type, type.c_str(), type.size());
		string value(msj->FirstChild(kMessageValueTag)->ToElement()->GetText());
		memset(mensaje.value,0,kLongChar);
		strncpy(mensaje.value, value.c_str(), value.size());
		return 0;
	}else{
		cout<<"Error al obtener el mensaje. XML mal escrito.";
		//*archivoErrores<<"Error al obtener el mensaje. Xml mal escrito."<<endl;
		return -1;
	}
}

int XmlParser::getServerPort() {
	TiXmlHandle docHandle(&this->doc);
	TiXmlElement* puertoElem = docHandle.FirstChild(kClientTag).FirstChild(kConnectionTag).FirstChild(kPortTag).ToElement();

	if(puertoElem)
		return atoi(puertoElem->GetText());
	else{
		cout<< "Error al obtener el puerto. XML mal escrito.";
		//*archivoErrores<<"Error al obtener el puerto. XML mal escrito."<<endl;
		return kDefaultPort;
	}
}

string XmlParser::getServerIP(){
	TiXmlHandle docHandle(&this->doc);
	TiXmlElement* ipElem = docHandle.FirstChild(kClientTag).FirstChild(kConnectionTag).FirstChild(kIPTag).ToElement();

	if(ipElem)
		return string(ipElem->GetText());
	else{
		cout<<"Error al obtener la ip. XML mal escrito.";
		//*archivoErrores<<"Error al obtener la ip. Xml mal escrito"<<endl;
		return "";
	}
}

int XmlParser::cantidadMensajes(){
	int contador = 0;
	bool masMensajes = true;
	TiXmlHandle docHandle(&this->doc);
	while(masMensajes){
		TiXmlElement* msj = docHandle.FirstChild(kClientTag).FirstChild(kMessagesTag).Child(kMessageTag, contador).ToElement();
		if(msj)
			contador++;
		else
			masMensajes = false;
	}
	return contador;
}

XmlParser::~XmlParser() {
	// TODO Auto-generated destructor stub
}

