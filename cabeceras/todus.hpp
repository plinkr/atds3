#ifndef TODUS_HPP
#define TODUS_HPP

#include <functional>
#include <QThread>
#include <QPointer>
#include <QTimer>
#include <QMutex>
#include <QNetworkReply>


class QNetworkAccessManager;
class QSslSocket;

class toDus : public QThread {
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

		void iniciar();
		void run() override;

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
		 * @brief Solicita a toDus el enlace firmado para obtener un archivo
		 * @param enlace Enlace no firmado
		 */
		void solicitarEnlaceFirmado(const QString &enlace, std::function<void(const QString &)> retroalimentador);

		void solicitarEnlaceFirmadoSubida();

	public slots:
		/**
		 * @brief Inicia la sesión en toDus partiendo del número de teléfono o ficha de acceso configurada
		 */
		void iniciarSesion();

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
		QMutex _mutexContadorComandos;
		QString _jID;
		QString _dominioJID;
		QTimer _temporizadorMantenerSesionActiva;
		bool _desconexionSolicitada;
		bool _fichaAccesoRenovada;
		QMap<QString, std::function<void(const QString &)>> _listadoRetroalimentadores;
		QString _idInicioSesion;
		QNetworkReply *_respuestaCodigoSMS;
		QNetworkReply *_respuestaFichaSolicitud;
		QNetworkReply *_respuestaFichaAcceso;
		QMutex _mutexSolicitarEnlace;

		QString generarIDSesion(unsigned int totalCaracteres);
		unsigned int obtenerProximoIDComando();
		void solicitarCodigoSMS(const QString &telefono);
		void solicitarFichaSolicitud(const QString &codigo);
		void solicitarFichaAcceso();
		void iniciarSesionConFichaAcceso();
		void xmppSaludar();
		void xmppIniciarSesion();
		void xmppEstablecerSesion();
		void xmppMantenerSesionActiva();
		void xmppSolicitarEnlaceDescarga(const QString &enlace, std::function<void(const QString &)> retroalimentador);
		void xmppSolicitarEnlaceSubida();
};

#endif // TODUS_HPP
