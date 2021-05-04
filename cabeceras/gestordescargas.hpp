#ifndef GESTORDESCARGAS_HPP
#define GESTORDESCARGAS_HPP

#include <QThread>
#include <QPointer>
#include "descarga.hpp"


class GestorDescargas : public QThread {
	Q_OBJECT

	public:
		GestorDescargas(QObject *padre);

		void run() override;

		void agregarDescarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando);
		void detenerDescarga(unsigned int id);

	private slots:
		void procesarTerminacionDescarga(unsigned int id);

	private:
		/**
		 * @brief Configuracion: Total de descargas paralelas
		 */
		int _configuracionTotalDescargasParalelas;

		/**
		 * @brief Total de descargas activas
		 */
		int _totalDescargasActivas;

		/**
		 * @brief Descargas activas
		 */
		QMap<unsigned int, QPointer<Descarga>> _descargasActivas;

		void iniciarProximaDescargaDisponible();

};

#endif // GESTORDESCARGAS_HPP


