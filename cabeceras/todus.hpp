#ifndef TODUS_HPP
#define TODUS_HPP

#include "configuraciones.hpp"
#include <functional>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QSettings>
#include <QSslSocket>
#include <QQueue>
#include <QtWebSockets/QWebSocket>


class HTTP;

class toDus : public QObject {
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

		toDus(QObject *padre = nullptr);
		~toDus();

//		void iniciar();

		/**
		 * @brief Obtiene el estado de la sesión toDus
		 * @return Devuelve el estado de la sesión toDus
		 */
		Estados obtenerEstado();

		/**
		 * @brief Desconecta de toDus
		 */
		void desconectar();

		/**
		 * @brief Reconectar
		 */
		void reconectar();

		void solicitarFichaSolicitud(const QString &codigo);

		void obtenerEnlaceFirmado(int tipo, int clasificacion, qint64 paquete, qint64 id, const QString &enlace, qint64 tamano = 0);
		void eliminarSolicitudEnlaceFirmado(qint64 id);
		void eliminarTodasSolicitudesEnlaceFirmado(qint64 paquete);

		void iniciarPPF();

	public slots:
		void procesarColaSolicitudesEnlacesFirmados();
		/**
		 * @brief Inicia la sesión en toDus partiendo del número de teléfono o ficha de acceso configurada
		 */
		void iniciarSesion();

	signals:
		void estadoCambiado(Estados estado);
		void enlaceFirmadoObtenido(const QString &noFirmado, const QString &firmado);

	private slots:
		void eventoFinalizadaCodigoSMS();
		void eventoFinalizadaFichaSolicitud();
		void eventoFinalizadaFichaAcceso();
		void eventoCambiarEstado(QAbstractSocket::SocketState estado);
		void eventoConexionLista();
		void eventoDatosRecibidos();
		void eventoPPFConectado();
		void eventoPPFPong();
		void eventoPPFMensajeRecibido(const QString &mensaje);
		void eventoPPFDesconectado();
		void eventoPPFSocaloWebPING();
		void eventoPPFSocaloWebVerificarConexion();

	private:
		enum ProgresoInicioSesion {
			Ninguno = 0x00,
			Saludo = 0x01,
			MecanismosAutentificacion = 0x02,
			Autenficicacion = 0x03,
			SesionIniciada = 0x04
		};
		struct SolicitudEnlaceFirmado {
				int tipo;
				int clasificacion;
				qint64 paquete;
				QString enlace;
				qint64 tamano;
				QString idSesion;
				int reintentos;
		};

		QByteArray _buferCargaCodigoSMS;
		QByteArray _buferDescargaCodigoSMS;
		QPointer<HTTP> _httpCodigoSMS;
		QByteArray _buferCargaFichaSolicitud;
		QByteArray _buferDescargaFichaSolicitud;
		QPointer<HTTP> _httpFichaSolicitud;
		unsigned short _solicitudesFichaAcceso;
		QByteArray _buferCargaFichaAcceso;
		QByteArray _buferDescargaFichaAcceso;
		QByteArray _fichaAccesoActual;
		QPointer<HTTP> _httpFichaAcceso;
		QPointer<QSslSocket> _socaloSSL;
		Estados _estado;
		ProgresoInicioSesion _progresoInicioSesion;
		QString _idSesion;
		Configuraciones _configuraciones;
		unsigned int _contadorComandos;
		QString _jID;
		QString _dominioJID;
		QTimer _temporizadorMantenerSesionActiva;
		bool _desconexionSolicitada;
		bool _fichaAccesoRenovada;
		QMap<QString, std::function<void(const QString &)>> _listadoRetroalimentadores;
		QString _idInicioSesion;
		unsigned int _contadorPING;
		QTimer _temporizadorListadoSolicitudesEnlacesFirmados;
		QMap<qint64, SolicitudEnlaceFirmado> _listadoSolicitudesEnlacesFirmados;
		int _solicitudesEnlacesFirmadosAnterior;
		QWebSocket _socaloWeb;
		Estados _estadoSocaloWeb;
		bool _socaloWebConexionActiva;
		bool _socaloWebMismaFichaRecibida;
		bool _socaloWebFichaAportada;
		int _socaloWebIndiceFichaLocal;
		QTimer _temporizadorSocaloWebPING;
		QTimer _temporizadorSocaloWebVerificarConexion;

		QString generarIDSesion(unsigned int totalCaracteres);
		unsigned int obtenerProximoIDComando();
		void solicitarCodigoSMS(const QString &telefono);
		void solicitarFichaAcceso();
		void iniciarSesionConFichaAcceso();
		void xmppEnviar(const QString &buferAEnviar);
		void xmppSaludar();
		void xmppIniciarSesion();
		void xmppEstablecerSesion();
		void xmppEstablecerTiempoInactividad();
		void xmppEstablecerPresencia();
		void xmppSolicitarUsuariosBloqueados();
		void xmppSolicitarListadoGruposCanales();
		void xmppMantenerSesionActiva();
		void xmppPONG();
		void xmppSolicitarEnlaceDescarga(qint64 id);
		void xmppSolicitarEnlaceSubida(qint64 id, int clasificacion);
		void socaloWebAportarFicha();
		void socaloWebSolicitarFicha();
		QString socaloWebGenerarCadenaAcceso();
};

#endif // TODUS_HPP
