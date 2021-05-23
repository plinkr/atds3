#ifndef GESTORDESCARGAS_HPP
#define GESTORDESCARGAS_HPP

#include <QThread>
#include <QPointer>
#include <QSharedPointer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QReadWriteLock>
#include <QSettings>
#include "descarga.hpp"


class VentanaPrincipal;

class GestorDescargas : public QThread {
	Q_OBJECT

	public:
		QPointer<QNetworkAccessManager> _administradorAccesoRed;

		GestorDescargas(VentanaPrincipal *padre = nullptr);

		void iniciar();
		void run() override;

		void agregarDescarga(int fila, unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando);
		void detenerDescarga(unsigned int id);
		void detenerDescargas();

	private slots:
		void procesarTerminacionDescarga(unsigned int id);

	private:
		VentanaPrincipal *_padre;

		QSettings _configuracion;

		/**
		 * @brief Configuracion: Total de descargas paralelas
		 */
		int _configuracionTotalDescargasParalelas;

		/**
		 * @brief Total de descargas activas
		 */
		int _totalDescargasActivas;
		QReadWriteLock _mutexTotalDescargasActivas;

		/**
		 * @brief Descargas activas
		 */
		QMap<unsigned int, QPointer<Descarga>> _descargasActivas;

		struct _descargaEnCola {
				int fila;
				QSharedPointer<ModeloEntradas> modelo;
				QSharedPointer<ModeloEntradas> modeloDescargando;
		};
		/**
		 * @brief Descargas en cola
		 */
		QMap<unsigned int, _descargaEnCola> _descargasEnCola;
		QReadWriteLock _mutexDescargasEnCola;

		/**
		 * @brief Inidica si se están deteniendo todas las descargas
		 */
		bool _deteniendoDescargas;

		void iniciarProximaDescargaDisponible();

};

#endif // GESTORDESCARGAS_HPP


