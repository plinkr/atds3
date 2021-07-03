#ifndef HTTP_HPP
#define HTTP_HPP

#include <QNetworkAccessManager>
#include <QNetworkReply>


class QDataStream;

class HTTP : public QNetworkAccessManager {
	Q_OBJECT

	public:
		enum Metodo {
			OBTENER = 0x01,
			PUBLICAR = 0x02,
			PONER = 0x03
		};

		HTTP(QObject *padre = nullptr);

		void establecerId(int id);

		bool error();

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

		std::int64_t bytesCargados();
		void establecerBytesCargados(std::int64_t bytes);
		std::int64_t bytesDescargados();
		void establecerBytesDescargados(std::int64_t bytes);

		void establecerDatosPublicacion(const QByteArray &datos);
		void establecerArchivoPublicar(QIODevice *archivo);

		void establecerOrigenCarga(QByteArray *origen);
		void establecerOrigenCarga(QIODevice *origen);

		void establecerDestinoDescarga(QByteArray *destino);
		void establecerDestinoDescarga(QIODevice *destino);

		std::uint16_t codigoHTTP();

		bool ejecutar();

		void detener();

	signals:
		void conectado(int id);
		void cabecerasRecibidas(int id);
		void bytesCargaTransferidos(int id, std::uint32_t total);
		void bytesDescargaTransferidos(int id, std::uint32_t total);
		void detenido(int id);
		void detenidoParaReiniciar(int id);
		void finalizado(int id);

	private:
		int _id;
		bool _error;
		bool _detenido;
		bool _detenidoParaReiniciar;
		bool _ejecutado;
		bool _cabecerasRecibidas;
		Metodo _metodo;
		QString _anfitrion;
		std::uint16_t _puerto;
		QString _ruta;
		QMap<QString, QString> _cabecerasSolicitud;
		QMap<QString, QString> _cabecerasRespuesta;
		QByteArray *_datosPublicar;
		QIODevice *_archivoPublicar;
		QByteArray *_datosDescarga;
		QIODevice *_archivoDescarga;
		std::uint16_t _codigoHTTP;
//		int _tamanoRecibido;
		std::time_t _tiempoUltimaCarga;
		std::time_t _tiempoUltimaDescarga;
		qint64 _bytesCargados;
		qint64 _bytesCargadosInicialmente;
		qint64 _bytesDescargados;
		qint64 _bytesDescargadosInicialmente;
		QNetworkReply *_respuesta;

	private slots:
		void eventoCabecerasDescargadas();
		void eventoProgresoCargarDatos(qint64 enviado, qint64 total);
		void eventoDescargarDatos();
		void eventoFinalizado();
/*
		void eventoConectado();
		void eventoCargarDatos(qint64 bytes);
*/
		void eventoError(QNetworkReply::NetworkError errorSocalo);
};

#endif // HTTP_HPP
