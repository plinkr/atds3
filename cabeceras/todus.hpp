#ifndef TODUS_HPP
#define TODUS_HPP

#include <QObject>
#include <QPointer>
#include <QSslSocket>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class toDus : public QObject {
	Q_OBJECT

	public:
		enum Estado {
			Desconectado = 0x01,
			ResolviendoNombreDNS = 0x02,
			Conectando = 0x03,
			Conectado = 0x04,
			IniciandoSesion = 0x05,
			Listo = 0x06,
			Desconectando = 0x07
		};

		toDus(QObject *padre = nullptr);
		~toDus();

		/**
		 * @brief Obtiene el estado de la sesión toDus
		 * @return Devuelve el estado de la sesión toDus
		 */
		Estado obtenerEstado();

		/**
		 * @brief Desconecta de toDus
		 */
		void desconectar();

		/**
		 * @brief Reconectar
		 */
		void reconectar();

		/**
		 * @brief Inicia la sesión en toDus partiendo del número de teléfono o ficha de acceso configurada
		 */
		void iniciarSesion();

		/**
		 * @brief Solicita a toDus el enlace firmado para obtener un archivo
		 * @param enlace Enlace no firmado
		 */
		void solicitarEnlaceFirmado(const QString &enlace);

		void solicitarEnlaceFirmadoSubida();

	signals:
		void estadoCambiado(Estado estado);
		void enlaceFirmadoObtenido(const QString &noFirmado, const QString &firmado);

	private slots:
		void eventoFinalizadaCodigoSMS();
		void eventoRecepcionDatosFichaSolicitud();
		void eventoFinalizadaFichaSolicitud();
		void eventoRecepcionDatosFichaAcceso();
		void eventoFinalizadaFichaAcceso();
		void eventoError(QNetworkReply::NetworkError codigo);
		void eventoCambiarEstado(QAbstractSocket::SocketState estado);
		void eventoConexionLista();
		void eventoDatosRecibidos();

	private:
		enum ProgresoInicioSesion {
			Ninguno = 0x00,
			Saludo = 0x01,
			MecanismosAutentificacion = 0x02,
			Autenficicacion = 0x03,
			SesionIniciada = 0x04
		};

		QPointer<QSslSocket> _socaloSSL;
		QPointer<QNetworkAccessManager> _administradorAccesoRed;
		Estado _estado;
		ProgresoInicioSesion _progresoInicioSesion;
		QString _idSesion;
		unsigned int _contadorComandos;
		QString _jID;
		QString _dominioJID;
		QTimer _temporizadorMantenerSesionActiva;
		bool _reconexionSolicitada;
		bool _fichaAccesoRenovada;
		QMap<QString, QString> _listadoEnlacesFirmados;
		QNetworkReply *_respuestaCodigoSMS;
		QNetworkReply *_respuestaFichaSolicitud;
		QNetworkReply *_respuestaFichaAcceso;

		QString generarIDSesion(unsigned int totalCaracteres);
		void solicitarCodigoSMS(const QString &telefono);
		void solicitarFichaSolicitud(const QString &codigo);
		void solicitarFichaAcceso();
		void iniciarSesionConFichaAcceso();
		void xmppSaludar();
		void xmppIniciarSesion();
		void xmppEstablecerSesion();
		void xmppMantenerSesionActiva();
		void xmppSolicitarEnlaceDescarga(const QString &enlace);
		void xmppSolicitarEnlaceSubida();
};

#endif // TODUS_HPP
