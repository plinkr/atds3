#ifndef DESCARGA_HPP
#define DESCARGA_HPP

#include <QPointer>
#include <QSharedPointer>
#include <QNetworkReply>
#include <QFile>


class ModeloEntradas;
class GestorDescargas;
class QNetworkAccessManager;
struct sqlite3;

class Descarga : public QObject {
	Q_OBJECT

	public:
		Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargas, GestorDescargas *padre = nullptr);
		~Descarga();

		bool iniciado();

		void detener();

		unsigned int id();
		int fila();
		int filaDescargas();
		QSharedPointer<ModeloEntradas> modelo();
		QSharedPointer<ModeloEntradas> modeloDescargas();

		bool error();

	public slots:
		void iniciar();

	signals:
		void terminada(unsigned int id);

	private slots:
		void descargaIniciada();
		void eventoRecepcionDatos();
		void progresoDescarga(qint64 recibidos, qint64 total);
		void eventoError(QNetworkReply::NetworkError codigo);
		void descargaTerminada();
		void procesarRespuestaDesdeTodus(const QString &firmado);
		void iniciarDescarga();

	private:
		GestorDescargas *_padre;
		sqlite3 *_baseDatos;
		bool _iniciado;
		bool _deteniendo;
		bool _error;
		unsigned int _id;
		int _filaModelo;
		int _filaModeloDescargas;
		QPointer<QNetworkAccessManager> _administradorAccesoRed;
		QString _enlaceNoFirmado;
		QString _enlaceFirmado;
		QSharedPointer<ModeloEntradas> _modelo;
		QSharedPointer<ModeloEntradas> _modeloDescargas;
		QNetworkReply *_respuesta;
		QFile _archivo;
		std::time_t _ultimoTiempoRecepcion;
		quint32 _ultimoTamanoRecibido;
		quint64 _tamanoPrevio;

		void corregirFilaModelos();
		unsigned int encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo);
};

#endif // DESCARGA_HPP


