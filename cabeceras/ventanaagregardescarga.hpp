#ifndef VENTANAAGREGARDESCARGA_HPP
#define VENTANAAGREGARDESCARGA_HPP

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <memory>


struct _NuevaDescarga;

class VentanaAgregarDescarga : public QDialog
{
	Q_OBJECT

	public:
		VentanaAgregarDescarga(QWidget *parent = nullptr);

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
		/**
		 * Título de la ventana
		 */
		QString _tituloVentana;

		/**
		 * Enlace de la descarga
		 */
		std::unique_ptr<QLineEdit> _enlace;

		/**
		 * Nombre del archivo a guardar
		 */
		std::unique_ptr<QLineEdit> _nombre;

		/**
		 * Categoría a donde asignar la descarga
		 */
		std::unique_ptr<QComboBox> _categoria;

		/**
		 * Iniciar la descarga inmediatamente?
		 */
		std::unique_ptr<QCheckBox> _iniciar;

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANAAGREGARDESCARGA_HPP
