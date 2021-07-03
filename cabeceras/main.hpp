#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkProxy>
#include <iostream>


enum TiposRegistro {
	Ninguno,
	Critico,
	Error,
	Informacion,
	Depuracion
};

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
	public:
		QNetworkAccessManager *create(QObject *padre) override;
};

extern QString _organizacionNombre;
extern QString _organizacionDominio;
extern QString _aplicacionNombreCorto;
extern QString _aplicacionTitulo;
extern QString _aplicacionVersion;
extern QString _agenteUsuarioTodus;
extern QString _numeroVersionTodus;
extern QString _rutaDescargaPredeterminada;
extern QObject *_qmlRaiz;

extern QNetworkProxy _obtenerProxy();
extern std::ostream &emitirRegistro(TiposRegistro tipo, const QString &modulo);

#endif // _MAIN_HPP
