#include "descarga.hpp"
#include <QNetworkRequest>
#include <QSettings>


Descarga::Descarga(int id, const QString &enlace, const QString &nombre, const QString &ruta, QObject *padre)
	:  QThread(padre), _id(id), _enlace(enlace), _nombre(nombre), _ruta(ruta) {}

void Descarga::run() {
	QNetworkRequest solicitudRed;
	QSettings configuracion;
	QString autorizacion = QString("Bearer: %1").arg(configuracion.value("todus/fichaAcceso").toString());

	solicitudRed.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("todus/agenteDescarga", "ToDus 0.37.29 HTTP-Download").toString());
	solicitudRed.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	solicitudRed.setRawHeader("Authorization", autorizacion.toLocal8Bit());
}
