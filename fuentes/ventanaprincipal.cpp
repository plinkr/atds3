#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QVBoxLayout>
#include <QToolBar>
#include <QDockWidget>
#include <QStatusBar>
#include <QStandardItemModel>
#include <QListView>
#include <QTreeView>


VentanaPrincipal::VentanaPrincipal(QWidget *parent)
	: QMainWindow(parent) {
	Q_INIT_RESOURCE(iconos);
	construirIU();
}

VentanaPrincipal::~VentanaPrincipal() {}

void VentanaPrincipal::eventoAgregarDescarga() {
}

void VentanaPrincipal::eventoAgregarDescargasDesdeArchivo() {
}

void VentanaPrincipal::eventoEliminarDescarga() {
}

void VentanaPrincipal::eventoEliminarTodasDescargas() {
}

void VentanaPrincipal::eventoConfiguracion() {
}

void VentanaPrincipal::eventoAcerca() {
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaPrincipal::construirIU() {
	QToolBar *barraHerramientas = new QToolBar();
	barraHerramientas->setIconSize(QSize(48, 48));
	barraHerramientas->setMovable(false);

	QAction *accionAgregarDescarga = new QAction();
	accionAgregarDescarga->setIcon(QIcon(":/iconos/agregar.svg"));
	accionAgregarDescarga->setText("Agregar");
	accionAgregarDescarga->setToolTip("Agrega una descarga al listado");
	accionAgregarDescarga->setStatusTip("Agrega una descarga al listado");
	accionAgregarDescarga->setShortcut(QKeySequence(Qt::Key_Insert));
	connect(accionAgregarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoAgregarDescarga);
	barraHerramientas->addAction(accionAgregarDescarga);

	QAction *accionAgregarDescargasDesdeArchivo = new QAction();
	accionAgregarDescargasDesdeArchivo->setIcon(QIcon(":/iconos/agregar-desde-archivo.svg"));
	accionAgregarDescargasDesdeArchivo->setText("Agregar desde archivo");
	accionAgregarDescargasDesdeArchivo->setToolTip("Agrega un listado de descargas procesados desde un archivo");
	accionAgregarDescargasDesdeArchivo->setStatusTip("Agrega un listado de descargas procesados desde un archivo");
	accionAgregarDescargasDesdeArchivo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Insert));
	connect(accionAgregarDescargasDesdeArchivo, &QAction::triggered, this, &VentanaPrincipal::eventoAgregarDescargasDesdeArchivo);
	barraHerramientas->addAction(accionAgregarDescargasDesdeArchivo);

	QAction *accionEliminarDescarga = new QAction();
	accionEliminarDescarga->setIcon(QIcon(":/iconos/eliminar.svg"));
	accionEliminarDescarga->setText("Eliminar");
	accionEliminarDescarga->setToolTip("Elimina la(s) descarga(s) selecionada(s)");
	accionEliminarDescarga->setStatusTip("Elimina la(s) descarga(s) selecionada(s)");
	accionEliminarDescarga->setShortcut(QKeySequence(Qt::Key_Delete));
	connect(accionEliminarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoEliminarDescarga);
	barraHerramientas->addAction(accionEliminarDescarga);

	QAction *accionEliminarTodasDescargas = new QAction();
	accionEliminarTodasDescargas->setIcon(QIcon(":/iconos/limpiar.svg"));
	accionEliminarTodasDescargas->setText("Eliminar todas");
	accionEliminarTodasDescargas->setToolTip("Elimina todas las descargas del listado");
	accionEliminarTodasDescargas->setStatusTip("Elimina todas las descargas del listado");
	accionEliminarTodasDescargas->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete));
	connect(accionEliminarTodasDescargas, &QAction::triggered, this, &VentanaPrincipal::eventoEliminarTodasDescargas);
	barraHerramientas->addAction(accionEliminarTodasDescargas);

	barraHerramientas->addSeparator();

	QAction *accionConfiguracion = new QAction();
	accionConfiguracion->setIcon(QIcon(":/iconos/configurar.svg"));
	accionConfiguracion->setText("Configuración");
	accionConfiguracion->setToolTip("Configura datos de la aplicación");
	accionConfiguracion->setStatusTip("Configura datos de la aplicación");
	accionConfiguracion->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	connect(accionConfiguracion, SIGNAL(triggered(bool)), this, SLOT(eventoConfiguracion()));
	barraHerramientas->addAction(accionConfiguracion);

	barraHerramientas->addSeparator();

	QAction *accionAcerca = new QAction();
	accionAcerca->setIcon(QIcon(":/iconos/acerca.svg"));
	accionAcerca->setText("Acerca de");
	accionAcerca->setToolTip("Muestra información del creador");
	accionAcerca->setStatusTip("Muestra información del creador");
	connect(accionAcerca, SIGNAL(triggered(bool)), this, SLOT(eventoAcerca()));
	barraHerramientas->addAction(accionAcerca);

	addToolBar(barraHerramientas);

	QDockWidget *puertoCategorias = new QDockWidget();
	puertoCategorias->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	puertoCategorias->setFeatures(QDockWidget::DockWidgetMovable);
	puertoCategorias->setFixedWidth(128);

	QStandardItemModel *modeloListadoCategorias = new QStandardItemModel();

	QStandardItem *elementoDescargando = new QStandardItem();
	elementoDescargando->setIcon(QIcon(":/iconos/descarga.svg"));
	elementoDescargando->setText("Descargando");
	elementoDescargando->setToolTip("Descargas activas");
	modeloListadoCategorias->appendRow(elementoDescargando);
/*
	QStandardItem *elementoSubiendo = new QStandardItem();
	elementoSubiendo->setIcon(QIcon(":/iconos/subida.svg"));
	elementoSubiendo->setText("Subiendo");
	elementoSubiendo->setToolTip("Subidas activas");
	modeloListadoCategorias->appendRow(elementoSubiendo);
*/
	QStandardItem *elementoFinalizado = new QStandardItem();
	elementoFinalizado->setIcon(QIcon(":/iconos/finalizado.svg"));
	elementoFinalizado->setText("Finalizadas");
	elementoFinalizado->setToolTip("Descargas finalizadas");
	modeloListadoCategorias->appendRow(elementoFinalizado);

	_listadoCategorias = new QListView();
	_listadoCategorias->setFlow(QListView::TopToBottom);
	_listadoCategorias->setModel(modeloListadoCategorias);
	_listadoCategorias->setUniformItemSizes(true);
	_listadoCategorias->setStyleSheet("QListView::item { width: 120px; }");
	_listadoCategorias->setWrapping(true);
	_listadoCategorias->setViewMode(QListView::IconMode);
	_listadoCategorias->setCurrentIndex(modeloListadoCategorias->indexFromItem(elementoDescargando));

	puertoCategorias->setWidget(_listadoCategorias);
	addDockWidget(Qt::LeftDockWidgetArea, puertoCategorias);

	_modeloListadoDescargas = new QStandardItemModel();
	_modeloListadoDescargas->setHorizontalHeaderLabels(QStringList({"", "Nombre", "Completado", "Velocidad", "Enlace"}));

	_listadoDescargas = new QTreeView();
	_listadoDescargas->setModel(_modeloListadoDescargas);
	_listadoDescargas->setMinimumSize(QSize(640, (640*9)/16));
	setCentralWidget(_listadoDescargas);
	setContentsMargins(QMargins(5, 5, 5, 5));

	statusBar()->showMessage("Listo");
}
