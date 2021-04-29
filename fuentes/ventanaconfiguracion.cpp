#include "ventanaconfiguracion.hpp"
#include "modelocategorias.hpp"
#include <QSettings>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListView>
#include <QStackedWidget>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>


VentanaConfiguracion::VentanaConfiguracion(QWidget *padre)
	: QDialog(padre), _tituloVentana("Configuración") {
	construirIU();
}

/**
 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de opciones
 * @param indice Índice del modelo
 */
void VentanaConfiguracion::eventoOpcionSeleccionada(const QModelIndex &indice) {
	_elementosApilados->setCurrentIndex(indice.row());
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar la ruta de las descargas'
 */
void VentanaConfiguracion::eventoSeleccionarRutaDescargas() {
	QString ruta = QFileDialog::getExistingDirectory(this, "Seleccionar directorio");

	if (ruta.size() > 0) {
		_rutaDescargas->setText(ruta);
	}
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Guardar'
 */
void VentanaConfiguracion::eventoGuardarOpciones() {
	QSettings configuracion;

	configuracion.setValue("todus/telefono", _telefono->text().trimmed());
	configuracion.setValue("todus/ipServidorAutentificacion", _ipServidorAutentificacion->text().trimmed());
	configuracion.setValue("todus/nombreDNSServidorAutentificacion", _nombreDNSServidorAutentificacion->text().trimmed());
	configuracion.setValue("todus/ipServidorSesion", _ipServidorSesion->text().trimmed());
	configuracion.setValue("todus/nombreDNSServidorSesion", _nombreDNSServidorSesion->text().trimmed());
	configuracion.setValue("todus/ipServidorS3", _ipServidorS3->text().trimmed());
	configuracion.setValue("todus/nombreDNSServidorS3", _nombreDNSServidorS3->text().trimmed());
	configuracion.setValue("descargas/ruta", _rutaDescargas->text().trimmed());
	configuracion.setValue("descargas/descargasParalelas", _totalDescargasParalelas->value());

	accept();
}

/**
 * @brief Construye el listado de opciones
 */
QListView *VentanaConfiguracion::construirListadoOpciones() {
	_modeloListadoOpciones = new ModeloCategorias();

	QStandardItem *opcionTodus = new QStandardItem();
	opcionTodus->setIcon(QPixmap(":/iconos/todus.png"));
	opcionTodus->setText("toDus");
	opcionTodus->setToolTip("Configuraciones relacionadas con la conexión a la red toDus");
	_modeloListadoOpciones->appendRow(opcionTodus);

	QStandardItem *opcionDescargas = new QStandardItem();
	opcionDescargas->setIcon(QIcon(":/iconos/descarga.svg"));
	opcionDescargas->setText("Descargas");
	opcionDescargas->setToolTip("Configuraciones relacionadas con las descargas");
	_modeloListadoOpciones->appendRow(opcionDescargas);

	QListView *listadoOpciones = new QListView();
	listadoOpciones->setAcceptDrops(false);
	listadoOpciones->setDefaultDropAction(Qt::IgnoreAction);
	listadoOpciones->setDragDropMode(QAbstractItemView::NoDragDrop);
	listadoOpciones->setEditTriggers(QAbstractItemView::NoEditTriggers);
	listadoOpciones->setFixedWidth(124);
	listadoOpciones->setFlow(QListView::TopToBottom);
	listadoOpciones->setModel(_modeloListadoOpciones);
	listadoOpciones->setUniformItemSizes(true);
	listadoOpciones->setStyleSheet("QListView::item { width: 120px; }");
	listadoOpciones->setWrapping(true);
	listadoOpciones->setViewMode(QListView::IconMode);
	listadoOpciones->setCurrentIndex(_modeloListadoOpciones->indexFromItem(opcionTodus));
	connect(listadoOpciones, &QListView::activated, this, &VentanaConfiguracion::eventoOpcionSeleccionada);

	return listadoOpciones;
}

/**
 * @brief Construye los elementos para representar un subtítulo
 */
QWidget *VentanaConfiguracion::construirSubtitulo(const QString &subtitulo) {
	QWidget *elementos = new QWidget();

	QVBoxLayout *diseno = new QVBoxLayout();
	QMargins margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QLabel *subtituloEtiqueta = new QLabel(subtitulo);
	QFont fuente = subtituloEtiqueta->font();
	fuente.setPointSize(fuente.pointSize() + 4);
	fuente.setBold(true);
	subtituloEtiqueta->setFont(fuente);

	QFrame *linea = new QFrame();
	linea->setFrameStyle(QFrame::HLine);
	linea->setLineWidth(1);

	diseno->addWidget(subtituloEtiqueta);
	diseno->addWidget(linea);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de configuracion para la opción 'toDus'
 */
QWidget *VentanaConfiguracion::construirOpcionTodus() {
	QSettings configuracion;
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioTodus = new QFormLayout();
	margenes = formularioTodus->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioTodus->setContentsMargins(margenes);

	_telefono = new QLineEdit();
	_telefono->setPlaceholderText("Número en formato internacional...");
	_telefono->setText(configuracion.value("todus/telefono").toString());

	QLabel *telefonoExplicacion = new QLabel("Este campo es uno de los más importantes de la configuración de DARTS3. Aquí se define el número de teléfono para el inicio de sesión en toDus.");
	telefonoExplicacion->setAlignment(Qt::AlignTop);
	margenes = telefonoExplicacion->contentsMargins();
	margenes.setTop(0);
	telefonoExplicacion->setContentsMargins(margenes);
	telefonoExplicacion->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	telefonoExplicacion->setWordWrap(true);
	QFont fuente = telefonoExplicacion->font();
	fuente.setPointSize(fuente.pointSize() - 1);
	fuente.setItalic(true);
	telefonoExplicacion->setFont(fuente);

	_fichaAcceso = new QLineEdit();
	_fichaAcceso->setPlaceholderText("Ficha de acceso o déjelo en blanco...");
	_fichaAcceso->setText(configuracion.value("todus/fichaAcceso").toString());

	QLabel *fichaAccesoExplicacion = new QLabel("Este campo es opcional, dicho valor se obtiene del proceso de inicio de sesión. Solo debe ser configurado si el inicio de sesión se rompe debido a un cambio en toDus.");
	fichaAccesoExplicacion->setAlignment(Qt::AlignTop);
	margenes = fichaAccesoExplicacion->contentsMargins();
	margenes.setTop(0);
	fichaAccesoExplicacion->setContentsMargins(margenes);
	fichaAccesoExplicacion->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	fichaAccesoExplicacion->setWordWrap(true);
	fuente = fichaAccesoExplicacion->font();
	fuente.setPointSize(fuente.pointSize() - 1);
	fuente.setItalic(true);
	fichaAccesoExplicacion->setFont(fuente);

	formularioTodus->addRow("Teléfono:", _telefono);
	formularioTodus->addRow("", telefonoExplicacion);
	formularioTodus->addRow("Ficha de acceso:", _fichaAcceso);
	formularioTodus->addRow("", fichaAccesoExplicacion);

	QFormLayout *formularioConexion = new QFormLayout();
	margenes = formularioConexion->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioConexion->setContentsMargins(margenes);

	_ipServidorAutentificacion = new QLineEdit();
	_ipServidorAutentificacion->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorAutentificacion->setText(configuracion.value("todus/ipServidorAutentificacion").toString());

	_nombreDNSServidorAutentificacion = new QLineEdit();
	_nombreDNSServidorAutentificacion->setPlaceholderText("Nombre DNS o déjelo en blanco...");
	_nombreDNSServidorAutentificacion->setText(configuracion.value("todus/nombreDNSServidorAutentificacion").toString());

	_ipServidorSesion = new QLineEdit();
	_ipServidorSesion->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorSesion->setText(configuracion.value("todus/ipServidorSesion").toString());

	_nombreDNSServidorSesion = new QLineEdit();
	_nombreDNSServidorSesion->setPlaceholderText("Dirección DNS o déjelo en blanco...");
	_nombreDNSServidorSesion->setText(configuracion.value("todus/nombreDNSServidorSesion").toString());

	_ipServidorS3 = new QLineEdit();
	_ipServidorS3->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorS3->setText(configuracion.value("todus/ipServidorS3").toString());

	_nombreDNSServidorS3 = new QLineEdit();
	_nombreDNSServidorS3->setPlaceholderText("Dirección DNS o déjelo en blanco...");
	_nombreDNSServidorS3->setText(configuracion.value("todus/nombreDNSServidorS3").toString());

	formularioConexion->addRow("IP servidor autentificación:", _ipServidorAutentificacion);
	formularioConexion->addRow("Nombre DNS servidor autentificación:", _nombreDNSServidorAutentificacion);
	formularioConexion->addRow("IP servidor sesión:", _ipServidorSesion);
	formularioConexion->addRow("Nombre DNS servidor sesión:", _nombreDNSServidorSesion);
	formularioConexion->addRow("IP servidor S3:", _ipServidorS3);
	formularioConexion->addRow("Nombre DNS servidor S3:", _nombreDNSServidorS3);

	diseno->addWidget(construirSubtitulo("toDus"));
	diseno->addLayout(formularioTodus);
	diseno->addWidget(construirSubtitulo("Conexión"));
	diseno->addLayout(formularioConexion);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de configuracion para la opción 'toDus'
 */
QWidget *VentanaConfiguracion::construirOpcionDescargas() {
	QSettings configuracion;
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioDescargas = new QFormLayout();
	margenes = formularioDescargas->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioDescargas->setContentsMargins(margenes);

	QHBoxLayout *filaElementosRutaDescargas = new QHBoxLayout();
	filaElementosRutaDescargas->setContentsMargins(0, 0, 0, 0);

	QLabel *rutaDescargasEtiqueta = new QLabel();
	rutaDescargasEtiqueta->setText("Ruta de guardado:");
	rutaDescargasEtiqueta->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	_rutaDescargas = new QLineEdit();
	_rutaDescargas->setReadOnly(true);
	_rutaDescargas->setText(configuracion.value("descargas/ruta").toString());
	QPushButton *botonSeleccionarRutaDescargas = new QPushButton();
	botonSeleccionarRutaDescargas->setIcon(QIcon(":/iconos/seleccionar-directorio.svg"));
	botonSeleccionarRutaDescargas->setText("Seleccionar");
	connect(botonSeleccionarRutaDescargas, &QPushButton::clicked, this, &VentanaConfiguracion::eventoSeleccionarRutaDescargas);

	filaElementosRutaDescargas->addWidget(_rutaDescargas, 1);
	filaElementosRutaDescargas->addWidget(botonSeleccionarRutaDescargas);

	QLabel *rutaDescargasExplicacion = new QLabel("Seleccione la ruta en donde se guardarán las nuevas descargas. Dentro de esta ruta se crearán las carpetas de las categorías y se almacenará la base de datos de la aplicación.");
	rutaDescargasExplicacion->setAlignment(Qt::AlignTop);
	margenes = rutaDescargasExplicacion->contentsMargins();
	margenes.setBottom(margenes.bottom() + 10);
	rutaDescargasExplicacion->setContentsMargins(margenes);
	rutaDescargasExplicacion->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	rutaDescargasExplicacion->setWordWrap(true);
	QFont fuente = rutaDescargasExplicacion->font();
	fuente.setPointSize(fuente.pointSize() - 1);
	fuente.setItalic(true);
	rutaDescargasExplicacion->setFont(fuente);

	_totalDescargasParalelas = new QSpinBox();
	_totalDescargasParalelas->setAlignment(Qt::AlignRight);
	_totalDescargasParalelas->setMinimumWidth(60);
	_totalDescargasParalelas->setValue(configuracion.value("descargas/descargasParalelas").toInt());

	formularioDescargas->addRow(rutaDescargasEtiqueta, filaElementosRutaDescargas);
	formularioDescargas->addRow("", rutaDescargasExplicacion);
	formularioDescargas->addRow("Total de descargas paralelas:", _totalDescargasParalelas);

	diseno->addWidget(construirSubtitulo("Descargas"));
	diseno->addLayout(formularioDescargas);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}
/**
 * @brief Construye la interfaz de usuario
 */
void VentanaConfiguracion::construirIU() {
	QIcon iconoAgregar = QIcon(":/iconos/configurar.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog | Qt::Popup);
	setStyleSheet("QPushButton, QComboBox { qproperty-iconSize: 24px 24px; } ");

	QVBoxLayout *diseno = new QVBoxLayout();

	QHBoxLayout *filaContenido = new QHBoxLayout();

	_elementosApilados = new QStackedWidget();
	_elementosApilados->setMinimumSize(QSize(600, 530));
	_elementosApilados->addWidget(construirOpcionTodus());
	_elementosApilados->addWidget(construirOpcionDescargas());

	filaContenido->addWidget(construirListadoOpciones());
	filaContenido->addWidget(_elementosApilados, 1);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonCerrar = new QPushButton();
	botonCerrar->setIcon(QIcon(":/iconos/finalizado.svg"));
	botonCerrar->setText("&Guardar");
	connect(botonCerrar, &QPushButton::clicked, this, &VentanaConfiguracion::eventoGuardarOpciones);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonCerrar);

	diseno->addLayout(filaContenido);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
