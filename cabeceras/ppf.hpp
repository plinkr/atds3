#ifndef PPF_HPP
#define PPF_HPP

#include "configuraciones.hpp"
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QtWebSockets/QWebSocket>


class PPF : public QObject {
	Q_OBJECT

	public:
		enum Estados {
			Desconectado,
			SolicitandoCodigoSMS,
			SolicitandoFichaSolicitud,
			SolicitandoFichaAcceso,
			ResolviendoNombreDNS,
			Conectando,
			Conectado,
			IniciandoSesion,
			Listo,
			Desconectando
		};

		PPF(QObject *padre = nullptr);

		void iniciar();

	private slots:
		void eventoConectado();
		void eventoPong();
		void eventoMensajeRecibido(const QString &mensaje);
		void eventoDesconectado();
		void eventoPING();
		void eventoVerificarConexion();

	private:
		QWebSocket _socaloWeb;
		Estados _estadoSocaloWeb;
		bool _socaloWebConexionActiva;
		bool _socaloWebMismaFichaRecibida;
		bool _socaloWebFichaAportada;
		int _socaloWebIndiceFichaLocal;
		QTimer _temporizadorSocaloWebPING;
		QTimer _temporizadorSocaloWebVerificarConexion;

		void aportarFicha();
		void solicitarFicha();
};

#endif // PPF_HPP
