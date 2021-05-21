#ifndef VENTANAAGREGARDESCARGASDESDEARCHIVO_HPP
#define VENTANAAGREGARDESCARGASDESDEARCHIVO_HPP

#include "main.hpp"
#include <QDialog>
#include <QPointer>
#include <QStandardItemModel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSettings>


class VentanaAgregarDescargasDesdeArchivos : public QDialog
{
	Q_OBJECT

	public:
		VentanaAgregarDescargasDesdeArchivos(QWidget *parent = nullptr);

		/**
		 * @brief Limpia los campos
		 */
		void limpiarCampos();

		/**
		 * @brief Procesa un archivo en búsqueda de entradas compatibles
		 */
		void procesarArchivo(const QString &archivo);

		/**
		 * @brief Devuelve los datos de las descargas que el usuario especificó
		 * @return Datos del listado de las descargas procesadas
		 */
		QVector<_NuevaDescarga> obtenerDatosDescargas();

	private slots:
		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descargas'
		 */
		void eventoAgregarDescargas();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar archivoos y procesarlos'
		 */
		void eventoSeleccionarArchivosAProcesar();

	private:
		QSettings _configuracion;

		/**
		 * Título de la ventana
		 */
		QString _tituloVentana;

		/**
		 * Botón para seleccionar los archivos a procesar
		 */
		QPointer<QPushButton> _botonSeleccionarArchivos;

		/**
		 * Listado de elementos procesados
		 */
		QVector<_NuevaDescarga> _listadoElementosProcesados;

		/**
		 * Modelo del listado de elementos procesados
		 */
		QPointer<QStandardItemModel> _modeloElementosProcesados;

		/**
		 * Categoría a donde asignar la descarga
		 */
		QPointer<QComboBox> _categoria;

		/**
		 * Iniciar la descarga inmediatamente?
		 */
		QPointer<QCheckBox> _iniciar;

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANAAGREGARDESCARGASDESDEARCHIVO_HPP
