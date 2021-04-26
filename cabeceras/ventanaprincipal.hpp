#ifndef VENTANAPRINCIPAL_HPP
#define VENTANAPRINCIPAL_HPP

#include <QMainWindow>


class QStandardItemModel;
class QListView;
class QTreeView;

class VentanaPrincipal : public QMainWindow
{
	Q_OBJECT

	public:
		VentanaPrincipal(QWidget *parent = nullptr);
		~VentanaPrincipal();

	private slots:
		void eventoAgregarDescarga();
		void eventoAgregarDescargasDesdeArchivo();
		void eventoEliminarDescarga();
		void eventoEliminarTodasDescargas();
		void eventoConfiguracion();
		void eventoAcerca();

	private:
		QStandardItemModel *_modeloListadoDescargas;
		QListView *_listadoCategorias;
		QTreeView *_listadoDescargas;
		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANAPRINCIPAL_HPP
