#ifndef HTTP1_HPP
#define HTTP1_HPP

#include <QSslSocket>
#include <QMutex>
#include <QPointer>
#include <QBuffer>
#include <QDataStream>
#include <memory>


class HTTP1 : public QSslSocket {
	Q_OBJECT

	public:
		enum Metodo {
			OBTENER = 0x01,
			PUBLICAR = 0x02,
			PONER = 0x03
		};

		HTTP1(QObject *padre = nullptr);

		Metodo metodo();
		void establecerMetodo(Metodo metodo);

		QString anfitrion();
		void establecerAnfitrion(const QString &anfitrion);

		std::uint16_t puerto();
		void establecerPuerto(std::uint16_t puerto);

		QString ruta();
		void establecerRuta(const QString &ruta);

		QMap<QString, QString> cabecerasSolicitud();
		void agregarCabeceraSolicitud(const QString &llave, const QString &valor);

		QString cabeceraRespuesta(const QString &cabecera);
		QMap<QString, QString> cabecerasRespuesta();

		void establecerOrigenCarga(QByteArray *origen);
		void establecerOrigenCarga(QIODevice *origen);

		void establecerDestinoDescarga(QByteArray *destino);
		void establecerDestinoDescarga(QIODevice *destino);

		std::uint16_t codigoHTTP();

		bool ejecutar();

	signals:
		void conectado();
		void cabecerasRecibidas();
		void bytesCargados(std::uint32_t total);
		void bytesDescargados(std::uint32_t total);
		void finalizado();

	private:
		bool _error;
		QMutex _mutexMetodo;
		Metodo _metodo;
		QMutex _mutexAnfitrion;
		QString _anfitrion;
		QMutex _mutexPuerto;
		std::uint16_t _puerto;
		QMutex _mutexRuta;
		QString _ruta;
		QMutex _mutexCabecerasSolicitud;
		QMap<QString, QString> _cabecerasSolicitud;
		QMutex _mutexCabecerasRespuesta;
		QMap<QString, QString> _cabecerasRespuesta;
		QMutex _mutexFlujoCarga;
		std::unique_ptr<QDataStream> _flujoCarga;
		QMutex _mutexFlujoDescarga;
		std::unique_ptr<QDataStream> _flujoDescarga;
		bool _cabecerasRecibidas;
		QMutex _mutexCodigoHTTP;
		std::uint16_t _codigoHTTP;
		int _tamanoRecibido;
		std::time_t _tiempoUltimaRecepcion;
		std::uint64_t _tamanoDescargado;

	private slots:
		void eventoConectado();
		void eventoRecibirDatos();
		void eventoDesconectado();
};

#endif // HTTP1_HPP
