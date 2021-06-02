#ifndef DESCARGA_HPP
#define DESCARGA_HPP

#include <QPointer>
#include <QSharedPointer>
#include <QFile>


class ModeloEntradas;
class GestorDescargas;
class QTimer;
class HTTP1;

class Descarga : public QObject {
	Q_OBJECT

	public:
		Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargas, GestorDescargas *padre = nullptr);

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
		void eventoDescargaIniciada();
		void eventoRecepcionDatos(uint32_t total);
		void eventoProgresoDescarga(qint64 recibidos, qint64 total);
		void eventoMonitorizarDescarga();
		void eventoDescargaTerminada();
		void eventoProcesarRespuestaDesdeTodus(const QString &firmado);
		void iniciarDescarga();

	private:
		GestorDescargas *_padre;
		bool _iniciado;
		bool _deteniendo;
		bool _error;
		unsigned int _id;
		int _filaModelo;
		int _filaModeloDescargas;
		QPointer<HTTP1> _http;
		QString _enlaceNoFirmado;
		QString _enlaceFirmado;
		QSharedPointer<ModeloEntradas> _modelo;
		QSharedPointer<ModeloEntradas> _modeloDescargas;
		QFile _archivo;
		qint64 _bytesRecibidos;
		qint64 _bytesTotal;
		std::time_t _ultimoTiempoRecepcion;
		quint32 _ultimoTamanoRegistrado;
		QPointer<QTimer> _temporizadorMonitorizacionDescarga;

		void corregirFilaModelos();
		unsigned int encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo);
};

#endif // DESCARGA_HPP


