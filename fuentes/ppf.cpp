#include "ppf.hpp"
#include "main.hpp"
#include <ctime>
#include <QByteArray>
#include <QSettings>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>


PPF::PPF(QObject *padre)
	: QObject(padre) {
	_temporizadorSocaloWebPING.setInterval(15000);
	connect(&_temporizadorSocaloWebPING, &QTimer::timeout, this, &PPF::eventoPING);

	_temporizadorSocaloWebVerificarConexion.setInterval(20000);
	connect(&_temporizadorSocaloWebVerificarConexion, &QTimer::timeout, this, &PPF::eventoVerificarConexion);

	_socaloWeb.ignoreSslErrors();
	connect(&_socaloWeb, &QWebSocket::connected, this, &PPF::eventoConectado);
	connect(&_socaloWeb, &QWebSocket::pong, this, &PPF::eventoPong);
	connect(&_socaloWeb, &QWebSocket::textMessageReceived, this, &PPF::eventoMensajeRecibido);
	connect(&_socaloWeb, &QWebSocket::disconnected, this, &PPF::eventoDesconectado);
}

void PPF::iniciar() {
	QString servidorPPF = "atds3.herokuapp.com";

	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Conectando a " << servidorPPF.toStdString() << std::endl;

	_socaloWebConexionActiva = false;
	_socaloWebMismaFichaRecibida = false;
	_temporizadorSocaloWebPING.start();
	_socaloWeb.setProxy(_obtenerProxy());
	_socaloWeb.open(QUrl("wss://" + servidorPPF));
	_estadoSocaloWeb = Estados::Conectando;
}

void PPF::eventoConectado() {
	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Conectado" << std::endl;

	_socaloWebConexionActiva = true;
	_estadoSocaloWeb = Estados::Conectado;

	if (_fichaAccesoActual == _configuraciones.valor("todus/fichaAcceso", "").toByteArray() && _fichaAccesoActual.size() > 0) {
		socaloWebAportarFicha();
		return;
	}

	if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
		_socaloWeb.close();
	}
}

void PPF::eventoPong() {
	_socaloWebConexionActiva = true;
}

void PPF::eventoMensajeRecibido(const QString &mensaje) {
	QJsonDocument json = QJsonDocument::fromJson(mensaje.toUtf8());
	QByteArray ficha;

	if (json.object().value("accion").toString() == "entregarFicha") {
		ficha = json.object().value("ficha").toString().toUtf8();
		_estado = Estados::Desconectado;

		if (ficha != _fichaAccesoActual && ficha != _configuraciones.valor("todus/fichaAcceso", "").toByteArray()) {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Recibida una ficha de acceso nueva" << std::endl;

			_socaloWebMismaFichaRecibida = false;
			_fichaAccesoActual = ficha;
		} else {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Recibida la misma ficha de acceso, ignorando" << std::endl;

			if (_socaloWebMismaFichaRecibida == false) {
				_socaloWebMismaFichaRecibida = true;
			}
		}
	}
}

void PPF::eventoDesconectado() {
	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Desconectado" << std::endl;

	_socaloWebConexionActiva = false;
	_temporizadorSocaloWebPING.stop();
	_estadoSocaloWeb = Estados::Desconectado;
}

void PPF::eventoSocaloWebPING() {
	if (_estadoSocaloWeb == Estados::Conectado) {
		if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
			_socaloWeb.close();
		} else {
			_socaloWeb.ping();
		}
	}
}

void PPF::eventoSocaloWebVerificarConexion() {
	if (_socaloWebConexionActiva == false) {
		if (_estadoSocaloWeb == Estados::Conectado) {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Detectada una conexión fantasma, cerrando" << std::endl;
			_socaloWeb.close();
		}
	} else {
		_socaloWebConexionActiva = false;
	}
}

void PPF::AportarFicha() {
	if (_estadoSocaloWeb == Estados::Conectado) {
		emitirRegistro(TiposRegistro::Informacion, "PPF") << "Aportando la ficha de acceso del usuario configurado" << std::endl;

		_socaloWeb.sendTextMessage(QString("{\"accion\":\"aportarFicha\",\"ficha\":\"%1\",\"expiracion\":%2}").arg(_configuraciones.valor("todus/fichaAcceso").toString()).arg(_configuraciones.valor("todus/fichaAccesoTiempoExpiracion").toLongLong()));
		_socaloWebFichaAportada = true;

		if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
			_socaloWeb.close();
		}
	}
}

void PPF::SolicitarFicha() {
	if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
		if (_configuraciones.valor("todus/programaPiscinaFichasLocal", "").toString().size() > 0) {
			QStringList listadoFichasAcceso = _configuraciones.valor("todus/programaPiscinaFichasLocal", "").toString().split("\n");

			for (int i = 0; i < listadoFichasAcceso.size();) {
				if (listadoFichasAcceso[i].trimmed().isEmpty() == true) {
					listadoFichasAcceso.removeAt(i);
					continue;
				}
				i++;
			}
			if (_configuraciones.valor("todus/fichaAcceso", "").toByteArray().isEmpty() == false) {
				listadoFichasAcceso.push_front(_configuraciones.valor("todus/fichaAcceso", "").toByteArray());
			}
			if (listadoFichasAcceso.size() > 0) {
				if (_socaloWebIndiceFichaLocal >= listadoFichasAcceso.size() && _configuraciones.valor("todus/programaPiscinaFichasInternet", true).toBool() == false) {
					_socaloWebIndiceFichaLocal = 0;
				}
				if (_fichaAccesoActual != listadoFichasAcceso[_socaloWebIndiceFichaLocal]) {
					_fichaAccesoActual = listadoFichasAcceso[_socaloWebIndiceFichaLocal++].toUtf8();

					emitirRegistro(TiposRegistro::Informacion, "PPF") << "Tomada una ficha de acceso nueva desde la Piscina de Fichas de Acceso local" << std::endl;
				}
			}
		}
		if (_configuraciones.valor("todus/programaPiscinaFichasInternet", true).toBool() == true) {
			if (_estadoSocaloWeb == Estados::Conectado) {
				_estado = Estados::SolicitandoFichaAcceso;
				desconectar();

				emitirRegistro(TiposRegistro::Informacion, "PPF") << "Solicitando una ficha de acceso nueva" << std::endl;

				for (qint64 id : _listadoSolicitudesEnlacesFirmados.keys()) {
					_listadoSolicitudesEnlacesFirmados[id].reintentos = 0;
				}

				_socaloWeb.sendTextMessage(QString("{\"accion\":\"solicitarFicha\"}"));
			} else {
				iniciarPPF();
			}
		}
	}
}
