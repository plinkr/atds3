#ifndef VENTANACONFIGURACION_HPP
#define VENTANACONFIGURACION_HPP

#include <QDialog>
#include <QPointer>


class ModeloCategorias;
class QListView;
class QStackedWidget;
class QGroupBox;
class QLineEdit;
class QSpinBox;

class VentanaConfiguracion : public QDialog
{
	Q_OBJECT

	public:
		VentanaConfiguracion(QWidget *padre = nullptr);

	private slots:
		/**
		 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de opciones
		 * @param indice Índice del modelo
		 */
		void eventoOpcionSeleccionada(const QModelIndex &indice);

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar la ruta de las descargas'
		 */
		void eventoSeleccionarRutaDescargas();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Guardar'
		 */
		void eventoGuardarOpciones();

	private:
		/**
		 * Título de la ventana
		 */
		QString _tituloVentana;

		/**
		 * Modelo del listado de opciones
		 */
		QPointer<ModeloCategorias> _modeloListadoOpciones;

		/**
		 * @brief Elementos apilados
		 */
		QPointer<QStackedWidget> _elementosApilados;

		/**
		 * @brief Número de teléfono
		 */
		QPointer<QLineEdit> _telefono;

		/**
		 * @brief Ficha de acceso
		 */
		QPointer<QLineEdit> _fichaAcceso;

		/**
		 * @brief IP del servidor de autentificación
		 */
		QPointer<QLineEdit> _ipServidorAutentificacion;

		/**
		 * @brief Puerto del servidor de autentificación
		 */
		QPointer<QSpinBox> _puertoServidorAutentificacion;

		/**
		 * @brief Nombre DNS del servidor de autentificación
		 */
		QPointer<QLineEdit> _nombreDNSServidorAutentificacion;

		/**
		 * @brief IP del servidor de sesion
		 */
		QPointer<QLineEdit> _ipServidorSesion;

		/**
		 * @brief Puerto del servidor de sesion
		 */
		QPointer<QSpinBox> _puertoServidorSesion;

		/**
		 * @brief Nombre DNS del servidor de sesion
		 */
		QPointer<QLineEdit> _nombreDNSServidorSesion;

		/**
		 * @brief IP del servidor S3
		 */
		QPointer<QLineEdit> _ipServidorS3;

		/**
		 * @brief Puerto del servidor S3
		 */
		QPointer<QSpinBox> _puertoServidorS3;

		/**
		 * @brief Nombre DNS del servidor S3
		 */
		QPointer<QLineEdit> _nombreDNSServidorS3;

		/**
		 * @brief Ruta de guardado de las descargas
		 */
		QPointer<QLineEdit> _rutaDescargas;

		/**
		 * @brief Total de descargas paralelas
		 */
		QPointer<QSpinBox> _totalDescargasParalelas;

		/**
		 * @brief Construye los elementos para representar un subtítulo
		 */
		QWidget *construirSubtitulo(const QString &subtitulo);

		/**
		 * @brief Construye el listado de opciones
		 */
		QListView *construirListadoOpciones();

		/**
		 * @brief Construye los elementos de configuracion para la opción 'toDus'
		 */
		QWidget *construirOpcionTodus();

		/**
		 * @brief Construye los elementos de configuracion para la opción 'Descargas'
		 */
		QWidget *construirOpcionDescargas();

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANACONFIGURACION_HPP

