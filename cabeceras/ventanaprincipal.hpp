#ifndef VENTANAPRINCIPAL_HPP
#define VENTANAPRINCIPAL_HPP

#include "todus.hpp"
#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>
#include <QFile>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QSystemTrayIcon>


class QThread;
class QTreeView;
class QStandardItemModel;
class QCloseEvent;
class DelegacionIconoEstado;
class DelegacionBarraProgreso;
class DelegacionTamano;
class QStandardItemModel;
class QListView;
class ModeloCategorias;
class ModeloEntradas;
class VentanaAgregarDescarga;
class VentanaAgregarDescargasDesdeArchivos;
class VentanaConfiguracion;
class QLabel;
class GestorDescargas;
class QNetworkReply;
class HTTP1;

class VentanaPrincipal : public QMainWindow
{
	Q_OBJECT

	public:
		/**
		 * @brief Modelo del listado de la categoría 'Descargando'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaDescargas;

		/**
		 * @brief Modelo del listado de la categoría 'Programas'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaProgramas;

		/**
		 * @brief Modelo del listado de la categoría 'Música'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaMusica;

		/**
		 * @brief Modelo del listado de la categoría 'Videos'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaVideos;

		/**
		 * @brief Modelo del listado de la categoría 'Otros'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaOtros;

		/**
		 * @brief Listado de descargas
		 */
		QPointer<QTreeView> _listadoDescargas;

		/**
		 * @brief Ventana 'Configuración'
		 */
		QPointer<VentanaConfiguracion> _ventanaConfiguracion;

		VentanaPrincipal(QWidget *parent = nullptr);
		~VentanaPrincipal();

	public slots:
		/**
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Configuración'
		 */
		void eventoConfiguracion();

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
		void cerrarAplicacion();

		/**
		 * @brief Agrega la descarga especificada por el usuario en la ventana 'Agrega descarga'
		 */
		void agregarDescarga();

		/**
		 * @brief Agrega las descargas procesadas desde el archivo seleccionado por el usuario en la ventana 'Agrega descargas desde archivo'
		 */
		void agregarDescargasDesdeArchivo();

		/**
		 * @brief Procesa las configuraciones guardadas y ejecuta las acciones adecuadas
		 */
		void procesarCambiosConfiguracion();

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

		void eventoSubir();

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
		 * @brief Evento que se dispara cuando se hace clic en el botón 'Acerca de'
		 */
		void eventoAcerca();

		/**
		 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de categorías
		 * @param indice Índice del modelo
		 */
		void eventoCategoriaSeleccionada(const QModelIndex &indice);

		/**
		 * @brief Evento que se dispara cuando se finaliza la descarga del avatar
		 */
		void eventoDescargaTerminadaAvatarTodus();

		/**
		 * @brief Actualiza la etiqueta que refleja el estado de la sesión toDus
		 * @param Nuevo estado
		 */
		void actualizarEstadoTodus(toDus::Estado estado);

		void eventoIconoSistemaActivado(QSystemTrayIcon::ActivationReason razon);

	private:
		QSettings _configuracion;

		/**
		 * @brief Modelo del listado de categorías
		 */
		QPointer<ModeloCategorias> _modeloListadoCategorias;

		/**
		 * @brief Modelo del listado de la categoría 'Finalizadas'
		 */
		QSharedPointer<ModeloEntradas> _modeloCategoriaFinalizadas;

		/**
		 * @brief Elemento que muestra el listado de categorías
		 */
		QPointer<QListView> _listadoCategorias;

		/**
		 * @brief Elemento que representa un icono en el campo 'Estado' dentro del listado de descargas
		 */
		QPointer<DelegacionIconoEstado> _elementoIconoEstado;

		/**
		 * @brief Elemento que representa una barra de progreso en el campo 'Completado' dentro del listado de descargas
		 */
		QPointer<DelegacionBarraProgreso> _elementoBarraProgreso;

		/**
		 * @brief Elemento que representa el texto leíble por el humano del tamaño de descarga dentro del listado de descargas
		 */
		QPointer<DelegacionTamano> _elementoTamano;

		/**
		 * @brief Elemento que representa el texto leíble por el humano de la velocidad de descarga dentro del listado de descargas
		 */
		QPointer<DelegacionTamano> _elementoVelocidad;

		/**
		 * @brief Ventana 'Agregar descarga'
		 */
		QPointer<VentanaAgregarDescarga> _ventanaAgregarDescarga;

		/**
		 * @brief Ventana 'Agregar descargas desde archivo'
		 */
		QPointer<VentanaAgregarDescargasDesdeArchivos> _ventanaAgregarDescargasDesdeArchivo;

		/**
		 * @brief Etiqueta que representa el estado de la sesión toDus
		 */
		QPointer<QLabel> _estadoSesionTodus;

		/**
		 * @brief Avatar del usuario en la red toDus
		 */
		QPointer<QLabel> _avatarTodus;
		QString _rutaAvatarTodus;
		QFile _archivoAvatarTodus;
		QPointer<HTTP1> _httpAvatar;
		QByteArray _buferDescargaAvatar;

		/**
		 * @brief Categoría activada por el usuario
		 */
		int _categoriaActiva;

		/**
		 * @brief Configuracion: Telefono
		 */
		QString _configuracionTelefono;

		/**
		 * @brief Configuracion: Ficha de acceso
		 */
		QString _configuracionFichaAcceso;

		/**
		 * @brief Configuracion: IP del servidor de autentificación
		 */
		QString _configuracionIpServidorAutentificacion;

		/**
		 * @brief Configuracion: Puerto del servidor de autentificación
		 */
		int _configuracionPuertoServidorAutentificacion;

		/**
		 * @brief Configuracion: Nombre DNS del servidor de autentificación
		 */
		QString _configuracionNombreDNSServidorAutentificacion;

		/**
		 * @brief Configuracion: IP del servidor de sesión
		 */
		QString _configuracionIpServidorSesion;

		/**
		 * @brief Configuracion: Puerto del servidor de sesión
		 */
		int _configuracionPuertoServidorSesion;

		/**
		 * @brief Configuracion: Nombre DNS del servidor de sesión
		 */
		QString _configuracionNombreDNSServidorSesion;

		/**
		 * @brief Configuracion: IP del servidor de S3
		 */
		QString _configuracionIpServidorS3;

		/**
		 * @brief Configuracion: Puerto del servidor de S3
		 */
		int _configuracionPuertoServidorS3;

		/**
		 * @brief Configuracion: Nombre DNS del servidor de S3
		 */
		QString _configuracionNombreDNSServidorS3;

		/**
		 * @brief Configuracion: Tipo de proxy
		 */
		int _configuracionTipoProxy;

		/**
		 * @brief Configuracion: Servidor proxy
		 */
		QString _configuracionServidorProxy;

		/**
		 * @brief Configuracion: Puerto del servidor proxy
		 */
		int _configuracionPuertoServidorProxy;

		/**
		 * @brief Configuracion: Usuario del servidor proxy
		 */
		QString _configuracionUsuarioServidorProxy;

		/**
		 * @brief Configuracion: Contraseña del usuario del servidor proxy
		 */
		QString _configuracionContrasenaServidorProxy;

		/**
		 * @brief Gestor de las descargas
		 */
		QPointer<GestorDescargas> _gestorDescargas;

		QList<QUrl> _listadoArchivosSoltar;

		QPointer<QSystemTrayIcon> _iconoSistema;

		/**
		 * @brief Evento que se produce cuando el usuario cierra la ventana principal
		 * @param evento Evento
		 */
		void closeEvent(QCloseEvent *evento);

		void dragEnterEvent(QDragEnterEvent* evento);
		void dragMoveEvent(QDragMoveEvent* evento);
		void dragLeaveEvent(QDragLeaveEvent* evento);
		void dropEvent(QDropEvent *);

		/**
		 * @brief Inicializa la base de datos en memoria
		 */
		void inicializarBaseDatos();

		/**
		 * @brief Sincroniza la base de datos física
		 */
		void sincronizarBaseDatos();

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

		/**
		 * @brief Construye el elemento del listado de descargas
		 * @return Puntero al elemento del listado de descargas
		 */
		QTreeView *construirListadoDescargas();

		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();

		/**
		 * @brief Descarga el avatar del usuario desde la red toDus y lo muestra en la barra de herramientas
		 */
		void actualizarAvatar();

		QSharedPointer<ModeloEntradas> obtenerModeloDesdeCategoria(int categoria);
};

#endif // VENTANAPRINCIPAL_HPP
