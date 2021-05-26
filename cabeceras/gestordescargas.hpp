#ifndef GESTORDESCARGAS_HPP
#define GESTORDESCARGAS_HPP

#include <QThread>
#include <QPointer>
#include <QSharedPointer>
#include <QSettings>
#include <QMutex>


class VentanaPrincipal;
class ModeloEntradas;
class Descarga;
struct sqlite3;

class GestorDescargas : public QThread {
	Q_OBJECT

	public:
		/**
		 * @brief Inidica si se están deteniendo todas las descargas
		 */
		bool _deteniendoDescargas;

		GestorDescargas(VentanaPrincipal *padre = nullptr);

		void run() override;

		void agregarDescarga(int fila, unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando);
		void detenerDescarga(unsigned int id);
		void detenerDescargas();

	private slots:
		void procesarTerminacionDescarga(unsigned int id);

	private:
		VentanaPrincipal *_padre;
		sqlite3 *_baseDatos;

		QSettings _configuracion;

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

		struct _descargaEnCola {
				int fila;
				unsigned int id;
				QSharedPointer<ModeloEntradas> modelo;
				QSharedPointer<ModeloEntradas> modeloDescargando;
		};
		/**
		 * @brief Descargas en cola
		 */
		QMap<unsigned int, _descargaEnCola> _descargasEnCola;
		QMutex _mutexDescargasEnCola;

		void iniciarProximaDescargaDisponible();

};

#endif // GESTORDESCARGAS_HPP


