#include "ventanaconfiguracion.hpp"
#include "modelocategorias.hpp"
#include "main.hpp"
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
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
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

void VentanaConfiguracion::eventoSeleccionarRuta7Zip() {

}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Guardar'
 */
void VentanaConfiguracion::guardarOpciones() {
	QString telefono = _telefono->text().trimmed();

	if (telefono.size() > 0) {
		if (telefono.size() <= 8) {
			telefono = "+53" + telefono;
		} else if (telefono[0] != '+') {
			telefono = "+" + telefono;
		}
		_configuracion.setValue("todus/telefono", telefono);
	} else {
		_configuracion.remove("todus/telefono");
	}

	if (_fichaAcceso->toPlainText().trimmed().size() > 0) {
		_configuracion.setValue("todus/fichaAcceso", _fichaAcceso->toPlainText().trimmed());
	} else {
		_configuracion.remove("todus/fichaAcceso");
	}

	if (_rutaDescargas->text().trimmed().size() > 0) {
		_configuracion.setValue("descargas/ruta", _rutaDescargas->text().trimmed());
	} else {
		_configuracion.remove("descargas/ruta");
	}

	if (_totalDescargasParalelas->text().trimmed().size() > 0) {
		_configuracion.setValue("descargas/descargasParalelas", _totalDescargasParalelas->text().trimmed());
	} else {
		_configuracion.remove("descargas/descargasParalelas");
	}

	//_configuracion.setValue("descargas/descomprimirAlFinalizar", _descomprimirAlFinalizarDescarga->isChecked());

	_configuracion.setValue("descargas/eliminarAlFinalizar", _eliminarAlFinalizarDescarga->isChecked());
/*
	if (_totalSubidasParalelas->text().trimmed().size() > 0) {
		_configuracion.setValue("subidas/subidasParalelas", _totalSubidasParalelas->text().trimmed());
	} else {
		_configuracion.remove("subidas/subidasParalelas");
	}

	_configuracion.setValue("subidas/comprimirArchivosAntesSubir", _comprimirArchivosAntesSubir->isChecked());

	_configuracion.setValue("subidas/dividirArchivosAntesSubir", _dividirArchivosAntesSubir->isChecked());

	_configuracion.setValue("subidas/tamanoDivisionArchivos", _tamanoDivisionArchivos->value());

	_configuracion.setValue("subidas/eliminarAlFinalizar", _eliminarAlFinalizarSubida->isChecked());

	if (_ruta7Zip->text().trimmed().size() > 0) {
		_configuracion.setValue("herramientas/ruta7Zip", _ruta7Zip->text().trimmed());
	} else {
		_configuracion.remove("herramientas/ruta7Zip");
	}
*/
	_configuracion.setValue("proxy/tipo", _tipoServidorProxy->currentIndex());

	if (_tipoServidorProxy->currentIndex() > 0) {
		if (_servidorProxy->text().trimmed().size() > 0) {
			_configuracion.setValue("proxy/servidor", _servidorProxy->text().trimmed());
		} else {
			_configuracion.remove("proxy/servidor");
		}

		_configuracion.setValue("proxy/puerto", _puertoServidorProxy->value());

		if (_usuarioServidorProxy->text().trimmed().size() > 0) {
			_configuracion.setValue("proxy/usuario", _usuarioServidorProxy->text().trimmed());
		} else {
			_configuracion.remove("proxy/usuario");
		}

		if (_contrasenaServidorProxy->text().trimmed().size() > 0) {
			_configuracion.setValue("proxy/contrasena", cifrarTexto(_contrasenaServidorProxy->text().trimmed().toLocal8Bit(), _usuarioServidorProxy->text().trimmed() + "@" + _servidorProxy->text().trimmed() + ":" + QString::number(_puertoServidorProxy->value())).toBase64());
		} else {
			_configuracion.remove("proxy/contrasena");
		}
	}

	if (_ipServidorAutentificacion->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/ipServidorAutentificacion", _ipServidorAutentificacion->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/ipServidorAutentificacion");
	}

	if (_puertoServidorAutentificacion->value() != 443) {
		_configuracion.setValue("avanzadas/puertoServidorAutentificacion", _puertoServidorAutentificacion->value());
	} else {
		_configuracion.remove("avanzadas/puertoServidorAutentificacion");
	}

	if (_nombreDNSServidorAutentificacion->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/nombreDNSServidorAutentificacion", _nombreDNSServidorAutentificacion->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/nombreDNSServidorAutentificacion");
	}

	if (_ipServidorSesion->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/ipServidorSesion", _ipServidorSesion->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/ipServidorSesion");
	}

	if (_puertoServidorSesion->value() != 1756) {
		_configuracion.setValue("avanzadas/puertoServidorSesion", _puertoServidorSesion->value());
	} else {
		_configuracion.remove("avanzadas/puertoServidorSesion");
	}

	if (_nombreDNSServidorSesion->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/nombreDNSServidorSesion", _nombreDNSServidorSesion->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/nombreDNSServidorSesion");
	}

	if (_ipServidorS3->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/ipServidorS3", _ipServidorS3->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/ipServidorS3");
	}

	if (_puertoServidorS3->value() != 443) {
		_configuracion.setValue("avanzadas/puertoServidorS3", _puertoServidorS3->value());
	} else {
		_configuracion.remove("avanzadas/puertoServidorS3");
	}

	if (_nombreDNSServidorS3->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/nombreDNSServidorS3", _nombreDNSServidorS3->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/nombreDNSServidorS3");
	}

	if (_agenteUsuario->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/agenteUsuario", _agenteUsuario->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/agenteUsuario");
	}

	if (_numeroVersion->text().trimmed().size() > 0) {
		_configuracion.setValue("avanzadas/numeroVersion", _numeroVersion->text().trimmed());
	} else {
		_configuracion.remove("avanzadas/numeroVersion");
	}

	emit accept();
}

/**
 * @brief Construye el listado de opciones
 */
QListView *VentanaConfiguracion::construirListadoOpciones() {
	_modeloListadoOpciones = new ModeloCategorias();

	QStandardItem *opcionTodus = new QStandardItem();
	opcionTodus->setIcon(QIcon(":/iconos/todus.png"));
	opcionTodus->setText("toDus");
	opcionTodus->setToolTip("_configuraciones relacionadas con la conexión a la red toDus");
	_modeloListadoOpciones->appendRow(opcionTodus);

	QStandardItem *opcionDescargas = new QStandardItem();
	opcionDescargas->setIcon(QIcon(obtenerRutaIcono() + "descarga.svg"));
	opcionDescargas->setText("Descargas");
	opcionDescargas->setToolTip("_configuraciones relacionadas con las descargas");
	_modeloListadoOpciones->appendRow(opcionDescargas);
/*
	QStandardItem *opcionSubidas = new QStandardItem();
	opcionSubidas->setIcon(QIcon(obtenerRutaIcono() + "subida.svg"));
	opcionSubidas->setText("Subidas");
	opcionSubidas->setToolTip("_configuraciones relacionadas con las subidas");
	_modeloListadoOpciones->appendRow(opcionSubidas);

	QStandardItem *opcionHerramientas = new QStandardItem();
	opcionHerramientas->setIcon(QIcon(obtenerRutaIcono() + "categoria-programas.svg"));
	opcionHerramientas->setText("Herramientas");
	opcionHerramientas->setToolTip("_configuraciones relacionadas con las herramientas");
	_modeloListadoOpciones->appendRow(opcionHerramientas);
*/
	QStandardItem *opcionProxy = new QStandardItem();
	opcionProxy->setIcon(QPixmap(obtenerRutaIcono() + "proxy.svg"));
	opcionProxy->setText("Proxy");
	opcionProxy->setToolTip("_configuraciones relacionadas con el proxy");
	_modeloListadoOpciones->appendRow(opcionProxy);

	QStandardItem *opcionAvanzadas = new QStandardItem();
	opcionAvanzadas->setIcon(QPixmap(obtenerRutaIcono() + "configurar.svg"));
	opcionAvanzadas->setText("Avanzadas");
	opcionAvanzadas->setToolTip("_configuraciones avanzadas");
	_modeloListadoOpciones->appendRow(opcionAvanzadas);

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
	connect(listadoOpciones, &QListView::pressed, this, &VentanaConfiguracion::eventoOpcionSeleccionada);

	return listadoOpciones;
}

/**
 * @brief Construye los elementos para representar un título
 */
QWidget *VentanaConfiguracion::construirTitulo(const QString &titulo) {
	QWidget *elementos = new QWidget();

	QVBoxLayout *diseno = new QVBoxLayout();
	QMargins margenes = diseno->contentsMargins();
	margenes.setTop(0);
	margenes.setBottom(margenes.bottom() + 10);
	diseno->setContentsMargins(margenes);

	QLabel *subtituloEtiqueta = new QLabel(titulo);
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
 * @brief Construye los elementos para representar un subtítulo
 */
QLabel *VentanaConfiguracion::construirSubtitulo(const QString &subtitulo) {
	QLabel *elemento = new QLabel(subtitulo);

	QMargins margenes = elemento->contentsMargins();
	margenes.setBottom(margenes.bottom() + 10);

	elemento->setAlignment(Qt::AlignTop);
	elemento->setContentsMargins(margenes);
	elemento->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	elemento->setWordWrap(true);

	QFont fuente = elemento->font();
	fuente.setPointSize(fuente.pointSize() - 1);
	fuente.setItalic(true);
	elemento->setFont(fuente);

	return elemento;
}

/**
 * @brief Construye los elementos de _configuracion para la opción 'toDus'
 */
QWidget *VentanaConfiguracion::construirOpcionTodus() {
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
	_telefono->setPlaceholderText("Número telefónico...");
	_telefono->setText(_configuracion.value("todus/telefono").toString());

	_fichaAcceso = new QPlainTextEdit();
	_fichaAcceso->setMaximumHeight(100);
	_fichaAcceso->setPlaceholderText("Ficha de acceso o déjelo en blanco...");
	_fichaAcceso->setPlainText(_configuracion.value("todus/fichaAcceso").toString());

	formularioTodus->addRow("Teléfono:", _telefono);
	formularioTodus->addRow("", construirSubtitulo("Este campo es uno de los más importantes de la configuración de ATDS3. Aquí se define el número de teléfono para el inicio de sesión en toDus."));
	formularioTodus->addRow("Ficha de acceso:", _fichaAcceso);
	formularioTodus->addRow("", construirSubtitulo("Este campo es opcional, dicho valor se obtiene del proceso de inicio de sesión. Solo debe ser configurado si el inicio de sesión se rompe debido a un cambio en toDus."));

	diseno->addWidget(construirTitulo("toDus"));
	diseno->addLayout(formularioTodus);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de _configuracion para la opción 'toDus'
 */
QWidget *VentanaConfiguracion::construirOpcionDescargas() {
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
	_rutaDescargas->setText(_configuracion.value("descargas/ruta").toString());
	QPushButton *botonSeleccionarRutaDescargas = new QPushButton();
	botonSeleccionarRutaDescargas->setIcon(QIcon(obtenerRutaIcono() + "seleccionar-directorio.svg"));
	botonSeleccionarRutaDescargas->setText("Seleccionar");
	connect(botonSeleccionarRutaDescargas, &QPushButton::clicked, this, &VentanaConfiguracion::eventoSeleccionarRutaDescargas);

	filaElementosRutaDescargas->addWidget(_rutaDescargas, 1);
	filaElementosRutaDescargas->addWidget(botonSeleccionarRutaDescargas);

	_totalDescargasParalelas = new QSpinBox();
	_totalDescargasParalelas->setAlignment(Qt::AlignRight);
	_totalDescargasParalelas->setMinimumWidth(60);
	_totalDescargasParalelas->setValue(_configuracion.value("descargas/descargasParalelas").toInt());

	formularioDescargas->addRow(rutaDescargasEtiqueta, filaElementosRutaDescargas);
	formularioDescargas->addRow("", construirSubtitulo("Define la ruta en donde se guardarán las nuevas descargas. Dentro de esta ruta se crearán las carpetas de las categorías y se almacenará la base de datos de la aplicación."));
	formularioDescargas->addRow("Total de descargas paralelas:", _totalDescargasParalelas);
	formularioDescargas->addRow("", construirSubtitulo("Define el total de tareas de descargas que pueden estar activas de forma simultánea."));

	QFormLayout *formularioAccionAlFinalizarDescarga = new QFormLayout();
	margenes = formularioAccionAlFinalizarDescarga->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioAccionAlFinalizarDescarga->setContentsMargins(margenes);

	_descomprimirAlFinalizarDescarga = new QCheckBox();
	_descomprimirAlFinalizarDescarga->setChecked(_configuracion.value("descargas/descomprimirAlFinalizar").toBool());

	_eliminarAlFinalizarDescarga = new QCheckBox();
	_eliminarAlFinalizarDescarga->setChecked(_configuracion.value("descargas/eliminarAlFinalizar").toBool());

	//formularioAccionAlFinalizarDescarga->addRow("Descomprimir?", _descomprimirAlFinalizarDescarga);
	//formularioAccionAlFinalizarDescarga->addRow("", construirSubtitulo("Define si se debe intentar descomprimir el archivo o las partes del achivo comprimido tras finalizar la descarga. Al finalizar la descompresión se eliminarán los archivos comprimidos."));
	formularioAccionAlFinalizarDescarga->addRow("Eliminar del listado?", _eliminarAlFinalizarDescarga);
	formularioAccionAlFinalizarDescarga->addRow("", construirSubtitulo("Define si se debe eliminar del listado la descarga tras finalizar."));

	diseno->addWidget(construirTitulo("Descargas"));
	diseno->addLayout(formularioDescargas);
	diseno->addWidget(construirTitulo("Acción al finalizar una descarga"));
	diseno->addLayout(formularioAccionAlFinalizarDescarga);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de _configuracion para la opción 'Subidas'
 */
QWidget *VentanaConfiguracion::construirOpcionSubidas() {
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioSubidas = new QFormLayout();
	margenes = formularioSubidas->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioSubidas->setContentsMargins(margenes);

	_totalSubidasParalelas = new QSpinBox();
	_totalSubidasParalelas->setAlignment(Qt::AlignRight);
	_totalSubidasParalelas->setMinimumWidth(60);
	_totalSubidasParalelas->setValue(_configuracion.value("subidas/subidasParalelas").toInt());

	formularioSubidas->addRow("Total de subidas paralelas:", _totalSubidasParalelas);
	formularioSubidas->addRow("", construirSubtitulo("Define el total de tareas de subidas que pueden estar activas de forma simultánea."));

	QFormLayout *formularioAccionAntesSubir = new QFormLayout();
	margenes = formularioAccionAntesSubir->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioAccionAntesSubir->setContentsMargins(margenes);

	_comprimirArchivosAntesSubir = new QCheckBox();
	_comprimirArchivosAntesSubir->setChecked(_configuracion.value("subidas/comprimirArchivosAntesSubir").toBool());

	_dividirArchivosAntesSubir = new QCheckBox();
	_dividirArchivosAntesSubir->setChecked(_configuracion.value("subidas/dividirArchivosAntesSubir").toBool());

	QHBoxLayout *filaElementosTamanoDivisionArchivos= new QHBoxLayout();
	filaElementosTamanoDivisionArchivos->setContentsMargins(0, 0, 0, 0);

	_tamanoDivisionArchivos = new QSpinBox();
	_tamanoDivisionArchivos->setAlignment(Qt::AlignRight);
	_tamanoDivisionArchivos->setMinimumWidth(60);
	_tamanoDivisionArchivos->setValue(_configuracion.value("subidas/tamanoDivisionArchivos").toInt());

	filaElementosTamanoDivisionArchivos->addWidget(_tamanoDivisionArchivos);
	filaElementosTamanoDivisionArchivos->addWidget(new QLabel("MiB"));

	formularioAccionAntesSubir->addRow("Comprimir?", _comprimirArchivosAntesSubir);
	formularioAccionAntesSubir->addRow("", construirSubtitulo("Define si se deben comprimir o no los archivos antes de subirlos."));
	formularioAccionAntesSubir->addRow("Dividir?", _dividirArchivosAntesSubir);
	formularioAccionAntesSubir->addRow("", construirSubtitulo("Define si se deben dividir o no los archivos antes de subirlos."));
	formularioAccionAntesSubir->addRow("Tamaño de la división de los archivos:", filaElementosTamanoDivisionArchivos);
	formularioAccionAntesSubir->addRow("", construirSubtitulo("Define el tamaño a utilizar para dividir los archivos antes de subirlos."));

	QFormLayout *formularioAccionAlFinalizar = new QFormLayout();
	margenes = formularioAccionAlFinalizar->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioAccionAlFinalizar->setContentsMargins(margenes);

	_eliminarAlFinalizarSubida = new QCheckBox();
	_eliminarAlFinalizarSubida->setChecked(_configuracion.value("subidas/eliminarAlFinalizar").toBool());

	formularioAccionAlFinalizar->addRow("Eliminar del listado?", _eliminarAlFinalizarSubida);
	formularioAccionAlFinalizar->addRow("", construirSubtitulo("Define si se debe eliminar del listado la subida tras finalizar."));

	diseno->addWidget(construirTitulo("Subidas"));
	diseno->addLayout(formularioSubidas);
	diseno->addWidget(construirTitulo("Acción antes de subir un archivo"));
	diseno->addLayout(formularioAccionAntesSubir);
	diseno->addWidget(construirTitulo("Acción al finalizar una subida"));
	diseno->addLayout(formularioAccionAlFinalizar);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de _configuracion para la opción 'Herramientas'
 */
QWidget *VentanaConfiguracion::construirOpcionHerramientas() {
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioHerramientas = new QFormLayout();
	margenes = formularioHerramientas->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioHerramientas->setContentsMargins(margenes);

	QHBoxLayout *filaElementosRuta7Zip = new QHBoxLayout();
	filaElementosRuta7Zip->setContentsMargins(0, 0, 0, 0);

	QLabel *ruta7ZipEtiqueta = new QLabel();
	ruta7ZipEtiqueta->setText("Ruta de 7-Zip:");
	ruta7ZipEtiqueta->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	_ruta7Zip = new QLineEdit();
	_ruta7Zip->setReadOnly(true);
	_ruta7Zip->setText(_configuracion.value("herramientas/ruta7Zip").toString());
	QPushButton *botonSeleccionarRuta7Zip = new QPushButton();
	botonSeleccionarRuta7Zip->setIcon(QIcon(obtenerRutaIcono() + "seleccionar-directorio.svg"));
	botonSeleccionarRuta7Zip->setText("Seleccionar");
	connect(botonSeleccionarRuta7Zip, &QPushButton::clicked, this, &VentanaConfiguracion::eventoSeleccionarRuta7Zip);

	filaElementosRuta7Zip->addWidget(_ruta7Zip, 1);
	filaElementosRuta7Zip->addWidget(botonSeleccionarRuta7Zip);

	formularioHerramientas->addRow(ruta7ZipEtiqueta, filaElementosRuta7Zip);
	formularioHerramientas->addRow("", construirSubtitulo("Define la ruta en donde reside el archivo ejecutable del programa 7-Zip. Este programa se utilizará para descomprimir los archivos descargados y los archivos que se van a subir. Esta opción no surtirá efecto si no se puede ejecutar el programa. Extensiones soportadas: arj, bz2, bzip2, gz, gzip, tgz, tpz, lzh, lha, lzma, lzma86, rar, tar, taz, tbz2, tbz, xz, txz, z, zip."));

	diseno->addWidget(construirTitulo("Herramientas"));
	diseno->addLayout(formularioHerramientas);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye los elementos de _configuracion para la opción 'Proxy'
 */
QWidget *VentanaConfiguracion::construirOpcionProxy() {
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioProxy = new QFormLayout();
	margenes = formularioProxy->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioProxy->setContentsMargins(margenes);

	QHBoxLayout *disenoElementosServidorProxy = new QHBoxLayout();

	_tipoServidorProxy = new QComboBox();
	_tipoServidorProxy->addItems(QStringList({"Sin proxy", "HTTP(S)", "SOCKS 4/5"}));
	_tipoServidorProxy->setCurrentIndex(_configuracion.value("proxy/tipo").toInt());

	_servidorProxy = new QLineEdit();
	_servidorProxy->setPlaceholderText("Dirección IP o nombre DNS del servidor proxy...");
	_servidorProxy->setText(_configuracion.value("proxy/servidor").toString());

	_puertoServidorProxy = new QSpinBox();
	_puertoServidorProxy->setAlignment(Qt::AlignRight);
	_puertoServidorProxy->setMinimum(0x01);
	_puertoServidorProxy->setMaximum(0xFFFF);
	_puertoServidorProxy->setMinimumWidth(75);
	_puertoServidorProxy->setValue(_configuracion.value("proxy/puerto", 3128).toInt());

	disenoElementosServidorProxy->addWidget(_servidorProxy);
	disenoElementosServidorProxy->addWidget(new QLabel("Puerto:"));
	disenoElementosServidorProxy->addWidget(_puertoServidorProxy);

	_usuarioServidorProxy = new QLineEdit();
	_usuarioServidorProxy->setPlaceholderText("Nombre de usuario o déjelo en blanco...");
	_usuarioServidorProxy->setText(_configuracion.value("proxy/usuario").toString());

	_contrasenaServidorProxy = new QLineEdit();
	_contrasenaServidorProxy->setPlaceholderText("Contraseña o déjelo en blanco...");
	_contrasenaServidorProxy->setEchoMode(QLineEdit::Password);
	if (_configuracion.contains("proxy/contrasena") == true) {
		_contrasenaServidorProxy->setText("********");
	}

	formularioProxy->addRow("Tipo:", _tipoServidorProxy);
	formularioProxy->addRow("Servidor:", disenoElementosServidorProxy);
	formularioProxy->addRow("Usuario:", _usuarioServidorProxy);
	formularioProxy->addRow("Contraseña:", _contrasenaServidorProxy);

	diseno->addWidget(construirTitulo("Proxy"));
	diseno->addLayout(formularioProxy);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;

}

/**
 * @brief Construye los elementos de _configuracion para la opción 'Avanzadas'
 */
QWidget *VentanaConfiguracion::construirOpcionAvanzadas() {
	QWidget *elementos = new QWidget();
	QMargins margenes = elementos->contentsMargins();
	margenes.setLeft(margenes.left() + 5);
	elementos->setContentsMargins(margenes);

	QVBoxLayout *diseno = new QVBoxLayout();
	margenes = diseno->contentsMargins();
	margenes.setTop(0);
	diseno->setContentsMargins(margenes);

	QFormLayout *formularioConexion = new QFormLayout();
	margenes = formularioConexion->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioConexion->setContentsMargins(margenes);

	QHBoxLayout *disenoElementosServidorAutentificacion = new QHBoxLayout();

	_ipServidorAutentificacion = new QLineEdit();
	_ipServidorAutentificacion->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorAutentificacion->setText(_configuracion.value("avanzadas/ipServidorAutentificacion").toString());

	_puertoServidorAutentificacion = new QSpinBox();
	_puertoServidorAutentificacion->setAlignment(Qt::AlignRight);
	_puertoServidorAutentificacion->setMinimum(0x01);
	_puertoServidorAutentificacion->setMaximum(0xFFFF);
	_puertoServidorAutentificacion->setMinimumWidth(75);
	_puertoServidorAutentificacion->setValue(_configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt());

	disenoElementosServidorAutentificacion->addWidget(_ipServidorAutentificacion);
	disenoElementosServidorAutentificacion->addWidget(new QLabel("Puerto:"));
	disenoElementosServidorAutentificacion->addWidget(_puertoServidorAutentificacion);

	_nombreDNSServidorAutentificacion = new QLineEdit();
	_nombreDNSServidorAutentificacion->setPlaceholderText("Nombre DNS o déjelo en blanco...");
	_nombreDNSServidorAutentificacion->setText(_configuracion.value("avanzadas/nombreDNSServidorAutentificacion").toString());

	QHBoxLayout *disenoElementosServidorSesion = new QHBoxLayout();

	_ipServidorSesion = new QLineEdit();
	_ipServidorSesion->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorSesion->setText(_configuracion.value("avanzadas/ipServidorSesion").toString());

	_puertoServidorSesion = new QSpinBox();
	_puertoServidorSesion->setAlignment(Qt::AlignRight);
	_puertoServidorSesion->setMinimum(0x01);
	_puertoServidorSesion->setMaximum(0xFFFF);
	_puertoServidorSesion->setMinimumWidth(75);
	_puertoServidorSesion->setValue(_configuracion.value("avanzadas/puertoServidorSesion", 1756).toInt());

	disenoElementosServidorSesion->addWidget(_ipServidorSesion);
	disenoElementosServidorSesion->addWidget(new QLabel("Puerto:"));
	disenoElementosServidorSesion->addWidget(_puertoServidorSesion);

	_nombreDNSServidorSesion = new QLineEdit();
	_nombreDNSServidorSesion->setPlaceholderText("Dirección DNS o déjelo en blanco...");
	_nombreDNSServidorSesion->setText(_configuracion.value("avanzadas/nombreDNSServidorSesion").toString());

	QHBoxLayout *disenoElementosServidorS3 = new QHBoxLayout();

	_ipServidorS3 = new QLineEdit();
	_ipServidorS3->setPlaceholderText("Dirección IP o déjelo en blanco...");
	_ipServidorS3->setText(_configuracion.value("avanzadas/ipServidorS3").toString());

	_puertoServidorS3 = new QSpinBox();
	_puertoServidorS3->setAlignment(Qt::AlignRight);
	_puertoServidorS3->setMinimum(0x01);
	_puertoServidorS3->setMaximum(0xFFFF);
	_puertoServidorS3->setMinimumWidth(75);
	_puertoServidorS3->setValue(_configuracion.value("avanzadas/puertoServidorS3", 443).toInt());

	disenoElementosServidorS3->addWidget(_ipServidorS3);
	disenoElementosServidorS3->addWidget(new QLabel("Puerto:"));
	disenoElementosServidorS3->addWidget(_puertoServidorS3);

	_nombreDNSServidorS3 = new QLineEdit();
	_nombreDNSServidorS3->setPlaceholderText("Dirección DNS o déjelo en blanco...");
	_nombreDNSServidorS3->setText(_configuracion.value("avanzadas/nombreDNSServidorS3").toString());

	formularioConexion->addRow("IP servidor autentificación:", disenoElementosServidorAutentificacion);
	formularioConexion->addRow("Nombre DNS servidor autentificación:", _nombreDNSServidorAutentificacion);
	formularioConexion->addRow("IP servidor sesión:", disenoElementosServidorSesion);
	formularioConexion->addRow("Nombre DNS servidor sesión:", _nombreDNSServidorSesion);
	formularioConexion->addRow("IP servidor S3:", disenoElementosServidorS3);
	formularioConexion->addRow("Nombre DNS servidor S3:", _nombreDNSServidorS3);

	QFormLayout *formularioOtras = new QFormLayout();
	margenes = formularioOtras->contentsMargins();
	margenes.setLeft(margenes.left() + 20);
	formularioOtras->setContentsMargins(margenes);

	_agenteUsuario = new QLineEdit();
	_agenteUsuario->setPlaceholderText("Agente de usuario o déjelo en blanco...");
	_agenteUsuario->setText(_configuracion.value("avanzadas/agenteUsuario").toString());

	_numeroVersion = new QLineEdit();
	_numeroVersion->setPlaceholderText("Número de versión de toDus o déjelo en blanco...");
	_numeroVersion->setText(_configuracion.value("avanzadas/numeroVersion").toString());

	formularioOtras->addRow("Agente usuario:", _agenteUsuario);
	formularioOtras->addRow("Número de versión:", _numeroVersion);

	diseno->addWidget(construirTitulo("Conexión"));
	diseno->addLayout(formularioConexion);
	diseno->addWidget(construirTitulo("Otras"));
	diseno->addLayout(formularioOtras);
	diseno->addStretch(1);

	elementos->setLayout(diseno);

	return elementos;
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaConfiguracion::construirIU() {
	QIcon iconoAgregar = QIcon(obtenerRutaIcono() + "configurar.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog);
	setStyleSheet("QPushButton, QComboBox { qproperty-iconSize: 24px 24px; } ");

	QVBoxLayout *diseno = new QVBoxLayout();

	QHBoxLayout *filaContenido = new QHBoxLayout();

	_elementosApilados = new QStackedWidget();
	_elementosApilados->setMinimumWidth(650);
	_elementosApilados->addWidget(construirOpcionTodus());
	_elementosApilados->addWidget(construirOpcionDescargas());
	//_elementosApilados->addWidget(construirOpcionSubidas());
	//_elementosApilados->addWidget(construirOpcionHerramientas());
	_elementosApilados->addWidget(construirOpcionProxy());
	_elementosApilados->addWidget(construirOpcionAvanzadas());

	filaContenido->addWidget(construirListadoOpciones());
	filaContenido->addWidget(_elementosApilados, 1);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonCerrar = new QPushButton();
	botonCerrar->setIcon(QIcon(obtenerRutaIcono() + "finalizado.svg"));
	botonCerrar->setText("&Guardar");
	connect(botonCerrar, &QPushButton::clicked, this, &VentanaConfiguracion::guardarOpciones);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonCerrar);

	diseno->addLayout(filaContenido);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
