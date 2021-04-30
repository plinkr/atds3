#ifndef VENTANAPRINCIPAL_HPP
#define VENTANAPRINCIPAL_HPP

#include "todus.hpp"
#include <QThread>
#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>
#include <QStandardItemModel>


class QCloseEvent;
class DelegacionIconoEstado;
class DelegacionBarraProgreso;
class DelegacionVelocidad;
class QStandardItemModel;
class QListView;
class ModeloCategorias;
class ModeloEntradas;
class QTreeView;
class VentanaAgregarDescarga;
class VentanaAgregarDescargasDesdeArchivos;
class VentanaConfiguracion;
class QLabel;

class VentanaPrincipal : public QMainWindow
{
	Q_OBJECT

	public:
		/**
		 * Modelo del listado de la categoría 'Programas'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaProgramas;

		/**
		 * Modelo del listado de la categoría 'Música'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaMusica;

		/**
		 * Modelo del listado de la categoría 'Videos'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaVideos;

		/**
		 * Modelo del listado de la categoría 'Otros'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaOtros;

		/**
		 * Ventana 'Configuración'
		 */
		QPointer<VentanaConfiguracion> _ventanaConfiguracion;

		VentanaPrincipal(QWidget *parent = nullptr);
		~VentanaPrincipal();

	signals:
		/**
		 * @brief Señal que se dispara cuando se selecciona una categoría
		 * @param modeloActivo Modelo activo
		 */
		void categoriaSeleccionada(QSharedPointer<ModeloEntradas> modeloActivo);

		/**
		 * @brief Señal que se emite cuando la aplicación va a detener su ejecución
		 */
		void detenerEjecucion();

	private slots:
		/**
		 * Agrega la descarga especificada por el usuario en la ventana 'Agrega descarga'
		 */
		void agregarDescarga();

		/**
		 * Agrega las descargas procesadas desde el archivo seleccionado por el usuario en la ventana 'Agrega descargas desde archivo'
		 */
		void agregarDescargasDesdeArchivo();

		/**
		 * @brief Procesa las configuraciones guardadas y ejecuta las acciones adecuadas
		 */
		void configuracionCambiada();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descarga'
		 */
		void eventoAgregarDescarga();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descargas desde archivo'
		 */
		void eventoAgregarDescargasDesdeArchivo();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar descarga'
		 */
		void eventoEliminarDescarga();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar todas las descargas'
		 */
		void eventoEliminarTodasDescargas();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar descarga'
		 */
		void eventoIniciarDescarga();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Pausar descarga'
		 */
		void eventoPausarDescarga();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar todas las descargas'
		 */
		void eventoIniciarTodasDescargas();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar todas las descargas'
		 */
		void eventoPausarTodasDescargas();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Configuración'
		 */
		void eventoConfiguracion();

		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Acerca de'
		 */
		void eventoAcerca();

		/**
		 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de categorías
		 * @param indice Índice del modelo
		 */
		void eventoCategoriaSeleccionada(const QModelIndex &indice);

		/**
		 * @brief Actualiza la etiqueta que refleja el estado de la sesión toDus
		 * @param Nuevo estado
		 */
		void actualizarEstadoTodus(toDus::Estado estado);

	private:
		/**
		 * Modelo del listado de categorías
		 */
		QPointer<ModeloCategorias> _modeloListadoCategorias;

		/**
		 * Modelo del listado de la categoría 'Descargando'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaDescargando;

		/**
		 * Modelo del listado de la categoría 'Finalizadas'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaFinalizadas;

		/**
		 * Elemento que muestra el listado de categorías
		 */
		QPointer<QListView> _listadoCategorias;

		/**
		 * Elemento que representa un icono en el campo 'Estado' dentro del listado de descargas
		 */
		QPointer<DelegacionIconoEstado> _elementoIconoEstado;

		/**
		 * Elemento que representa una barra de progreso en el campo 'Completado' dentro del listado de descargas
		 */
		QPointer<DelegacionBarraProgreso> _elementoBarraProgreso;

		/**
		 * Elemento que representa el texto leíble por el humano de la velocidad de descarga dentro del listado de descargas
		 */
		QPointer<DelegacionVelocidad> _elementoVelocidad;

		/**
		 * Listado de descargas
		 */
		QPointer<QTreeView> _listadoDescargas;

		/**
		 * Ventana 'Agregar descarga'
		 */
		QPointer<VentanaAgregarDescarga> _ventanaAgregarDescarga;

		/**
		 * Ventana 'Agregar descargas desde archivo'
		 */
		QPointer<VentanaAgregarDescargasDesdeArchivos> _ventanaAgregarDescargasDesdeArchivo;

		/**
		 * Etiqueta que representa el estado de la sesión toDus
		 */
		QPointer<QLabel> _estadoSesionTodus;

		/**
		 * @brief Categoría activada por el usuario
		 */
		int _categoriaActiva;

		/**
		 * @brief Indica si se debe ordenar la reconexión de la sesion toDus
		 */
		bool _toDusReconectar;

		void closeEvent(QCloseEvent *evento);

		/**
		 * @brief Construye los botones de la barra de herramientas
		 * @param barraHerramientas Puntero al objeto de la barra de herramientas
		 */
		void construirBotonesBarraHerramientas(QToolBar *barraHerramientas);

		/**
		 * @brief Construye las categorías
		 * @returns Puntero al elemento del listado de categorías
		 */
		QListView *construirListadoCategorias();

		QString _configuracionTelefono;
		QString _configuracionFichaAcceso;
		QString _configuracionIpServidorAutentificacion;
		int _configuracionPuertoServidorAutentificacion;
		QString _configuracionNombreDNSServidorAutentificacion;
		QString _configuracionIpServidorSesion;
		int _configuracionPuertoServidorSesion;
		QString _configuracionNombreDNSServidorSesion;
		QString _configuracionIpServidorS3;
		int _configuracionPuertoServidorS3;
		QString _configuracionNombreDNSServidorS3;

		/**
		 * @brief Inicializa la base de datos
		 */
		void inicializarBaseDatos();

		/**
		 * @brief Construye el elemento del listado de descargas
		 * @return Puntero al elemento del listado de descargas
		 */
		QTreeView *construirListadoDescargas();

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANAPRINCIPAL_HPP
