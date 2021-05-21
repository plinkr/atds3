#ifndef VENTANACONFIGURACION_HPP
#define VENTANACONFIGURACION_HPP

#include <QDialog>
#include <QPointer>


class ModeloCategorias;
class QListView;
class QStackedWidget;
class QGroupBox;
class QLineEdit;
class QPlainTextEdit;
class QSpinBox;
class QLabel;
class QCheckBox;
class QComboBox;

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
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar la ruta del programa 7Zip
		 */
		void eventoSeleccionarRuta7Zip();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Guardar'
		 */
		void guardarOpciones();

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
		QPointer<QPlainTextEdit> _fichaAcceso;

		/**
		 * @brief Ruta de guardado de las descargas
		 */
		QPointer<QLineEdit> _rutaDescargas;

		/**
		 * @brief Total de descargas paralelas
		 */
		QPointer<QSpinBox> _totalDescargasParalelas;

		/**
		 * @brief Descomprimir al finalizarla descarga
		 */
		QPointer<QCheckBox> _descomprimirAlFinalizarDescarga;

		/**
		 * @brief Eliminar del listado al finalizar la descarga
		 */
		QPointer<QCheckBox> _eliminarAlFinalizarDescarga;

		/**
		 * @brief Total de subidas paralelas
		 */
		QPointer<QSpinBox> _totalSubidasParalelas;

		/**
		 * @brief Ruta de guardado de las descargas
		 */
		QPointer<QCheckBox> _comprimirArchivosAntesSubir;

		/**
		 * @brief Dividir los archivos antes de subir?
		 */
		QPointer<QCheckBox> _dividirArchivosAntesSubir;

		/**
		 * @brief Tamaño de la división de los archivos
		 */
		QPointer<QSpinBox> _tamanoDivisionArchivos;

		/**
		 * @brief Eliminar del listado al finalizar la subida
		 */
		QPointer<QCheckBox> _eliminarAlFinalizarSubida;

		/**
		 * @brief Ruta del programa 7Zip
		 */
		QPointer<QLineEdit> _ruta7Zip;

		/**
		 * @brief Tipo de servidor proxy
		 */
		QPointer<QComboBox> _tipoServidorProxy;

		/**
		 * @brief IP o Nombre DNS del servidor proxy
		 */
		QPointer<QLineEdit> _servidorProxy;

		/**
		 * @brief Puerto del servidor proxy
		 */
		QPointer<QSpinBox> _puertoServidorProxy;

		/**
		 * @brief Usuario del servidor proxy
		 */
		QPointer<QLineEdit> _usuarioServidorProxy;

		/**
		 * @brief Contraseña del usuario del servidor proxy
		 */
		QPointer<QLineEdit> _contrasenaServidorProxy;

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
		 * @brief Agente de usuario de toDus
		 */
		QPointer<QLineEdit> _agenteUsuario;

		/**
		 * @brief Construye los elementos para representar un subtítulo
		 */
		QWidget *construirTitulo(const QString &titulo);

		/**
		 * @brief Construye los elementos para representar un subtítulo
		 */
		QLabel *construirSubtitulo(const QString &subtitulo);

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
		 * @brief Construye los elementos de configuracion para la opción 'Subidas'
		 */
		QWidget *construirOpcionSubidas();

		/**
		 * @brief Construye los elementos de configuracion para la opción 'Herramientas'
		 */
		QWidget *construirOpcionHerramientas();

		/**
		 * @brief Construye los elementos de configuracion para la opción 'Proxy'
		 */
		QWidget *construirOpcionProxy();

		/**
		 * @brief Construye los elementos de configuracion para la opción 'Avanzadas'
		 */
		QWidget *construirOpcionAvanzadas();

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANACONFIGURACION_HPP

