#ifndef VENTANAPRINCIPAL_HPP
#define VENTANAPRINCIPAL_HPP

#include <QThread>
#include <QMainWindow>
#include <memory>


class VentanaPrincipal;
class DelegacionBarraProgreso;
class QStandardItemModel;
class QListView;
class ModeloEntradas;
class QTreeView;

class VentanaPrincipal : public QMainWindow
{
	Q_OBJECT

	public:
		VentanaPrincipal(QWidget *parent = nullptr);
		~VentanaPrincipal();

	signals:
		/**
		 * @brief Señal que se emite cuando la aplicación va a detener su ejecución
		 */
		void detenerEjecucion();

	private slots:
		void eventoAgregarDescarga();
		void eventoAgregarDescargasDesdeArchivo();
		void eventoEliminarDescarga();
		void eventoEliminarTodasDescargas();
		void eventoIniciarDescarga();
		void eventoPausarDescarga();
		void eventoIniciarTodasDescargas();
		void eventoPausarTodasDescargas();
		void eventoConfiguracion();
		void eventoAcerca();

		/**
		 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de categorías
		 * @param indice Índice del modelo
		 */
		void eventoCategoriaSeleccionada(const QModelIndex &indice);

	private:
		std::unique_ptr<QStandardItemModel> _modeloListadoCategorias;
		std::unique_ptr<QListView> _listadoCategorias;

		/**
		 * Modelo del listado de la categoría 'Descargando'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaDescargando;

		/**
		 * Modelo del listado de la categoría 'Finalizadas'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaFinalizadas;

		/**
		 * Modelo del listado de la categoría 'Programas'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaProgramas;

		/**
		 * Modelo del listado de la categoría 'Música'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaMusica;

		/**
		 * Modelo del listado de la categoría 'Videos'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaVideos;

		/**
		 * Modelo del listado de la categoría 'Otros'
		 */
		std::unique_ptr<ModeloEntradas> _modeloCategoriaOtros;
		std::unique_ptr<DelegacionBarraProgreso> _elementoBarraProgreso;
		std::unique_ptr<QTreeView> _listadoDescargas;

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
