#ifndef TODUS_HPP
#define TODUS_HPP

#include <QSslSocket>
#include <QTimer>


class toDus : public QSslSocket {
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

	signals:
		void estadoCambiado(Estado estado);

	private slots:
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

		Estado _estado;
		ProgresoInicioSesion _progresoInicioSesion;
		QString _idSesion;
		unsigned int _contadorComandos;
		QString _jID;
		QString _dominioJID;
		QTimer _temporizadorMantenerSesionActiva;
		bool _reconexionSolicitada;

		void generarIDSesion();
		void iniciarSesionConTelefono();
		void iniciarSesionConFicha();
		void xmppSaludar();
		void xmppIniciarSesion();
		void xmppEstablecerSesion();
		void mantenerSesionActiva();
};

#endif // TODUS_HPP
