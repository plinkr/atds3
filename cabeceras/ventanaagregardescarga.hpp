#ifndef VENTANAAGREGARDESCARGA_HPP
#define VENTANAAGREGARDESCARGA_HPP

#include <QDialog>
#include <QPointer>
#include <QSettings>


struct _NuevaDescarga;
class QLineEdit;
class QComboBox;
class QCheckBox;

class VentanaAgregarDescarga : public QDialog
{
	Q_OBJECT

	public:
		VentanaAgregarDescarga(QWidget *padre = nullptr);

		/**
		 * @brief Limpia los campos
		 */
		void limpiarCampos();

		/**
		 * @brief Devuelve los datos de la descarga que el usuario especificó
		 * @return Datos de la descarga especificada
		 */
		_NuevaDescarga obtenerDatosDescarga();

	private slots:
		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar'
		 */
		void eventoAgregarDescarga();

	private:
		QSettings _configuracion;

		/**
		 * Título de la ventana
		 */
		QString _tituloVentana;

		/**
		 * Enlace de la descarga
		 */
		QPointer<QLineEdit> _enlace;

		/**
		 * Nombre del archivo a guardar
		 */
		QPointer<QLineEdit> _nombre;

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

#endif // VENTANAAGREGARDESCARGA_HPP
