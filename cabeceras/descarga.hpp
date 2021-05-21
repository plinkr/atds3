#ifndef DESCARGA_HPP
#define DESCARGA_HPP

#include <ctime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QSharedPointer>
#include <QFile>


class ModeloEntradas;
class VentanaPrincipal;
class QSslError;

class Descarga : public QNetworkAccessManager {
	Q_OBJECT

	public:
		Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando, QObject *padre = nullptr);

		bool iniciado();

		void detener();

		int fila();
		QSharedPointer<ModeloEntradas> modelo();

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
		bool _iniciado;
		bool _error;
		unsigned int _id;
		int _filaModelo;
		int _filaModeloDescargando;
		QPointer<QNetworkAccessManager> _administradorRed;
		QString _enlaceNoFirmado;
		QString _enlaceFirmado;
		QSharedPointer<ModeloEntradas> _modelo;
		QSharedPointer<ModeloEntradas> _modeloDescargando;
		QNetworkReply *_respuesta;
		QFile _archivo;
		std::time_t _ultimoTiempoRecepcion;
		quint32 _ultimoTamanoRecibido;
		quint64 _tamanoPrevio;

		bool modelosValido();
		unsigned int encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo);
};

#endif // DESCARGA_HPP


