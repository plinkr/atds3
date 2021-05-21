#include "ventanaagregardescargasdesdearchivo.hpp"
#include "ventanaprincipal.hpp"
#include <QDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QTreeView>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QStandardItemModel>


VentanaAgregarDescargasDesdeArchivos::VentanaAgregarDescargasDesdeArchivos(QWidget *parent)
	: QDialog(parent), _tituloVentana("Agregar descargas desde archivo") {
	construirIU();
	limpiarCampos();
}

/**
 * @brief Limpia los campos
 */
void VentanaAgregarDescargasDesdeArchivos::limpiarCampos() {
	_botonSeleccionarArchivos->setFocus();
	_listadoElementosProcesados.clear();
	_modeloElementosProcesados->clear();
	_modeloElementosProcesados->setColumnCount(2);
	_modeloElementosProcesados->setHeaderData(0, Qt::Horizontal, "Nombre");
	_modeloElementosProcesados->setHeaderData(1, Qt::Horizontal, "Enlace");

	// Restaura la última categoría seleccionada
	_categoria->setCurrentIndex(_configuracion.value("atds3/ultimaCategoriaAgregarDescarga", 0).toInt());
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descargas'
 */
void VentanaAgregarDescargasDesdeArchivos::eventoAgregarDescargas() {
	if (_listadoElementosProcesados.size() == 0) {
		return;
	}

	for (_NuevaDescarga &nuevaDescarga : _listadoElementosProcesados) {
		nuevaDescarga.categoria = _categoria->currentData().toInt();
		nuevaDescarga.iniciar = _iniciar->isChecked();
	}

	// Guarda la categoría seleccionada
	_configuracion.setValue("atds3/ultimaCategoriaAgregarDescarga", _categoria->currentIndex());

	// Guardar el valor del campo 'Iniciar descarga?'
	_configuracion.setValue("atds3/iniciarDescargaAgregarDescarga", _iniciar->isChecked());

	emit accept();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar archivos y procesarlos'
 */
void VentanaAgregarDescargasDesdeArchivos::eventoSeleccionarArchivosAProcesar() {
	QFileDialog dialogoSeleccion(this);
	QStringList listadoArchivos;

	// Restaurar la última ruta utilizada
	dialogoSeleccion.setDirectory(_configuracion.value("atds3/ultimaRutaAgregarDescarga", QDir::homePath()).toString());
	dialogoSeleccion.setFileMode(QFileDialog::ExistingFiles);
	dialogoSeleccion.setNameFilter(tr("Archivos de texto(*.txt)"));
	if (dialogoSeleccion.exec() == QFileDialog::Accepted) {
		listadoArchivos = dialogoSeleccion.selectedFiles();

		// Guarda la ruta
		_configuracion.setValue("atds3/ultimaRutaAgregarDescarga", dialogoSeleccion.directory().absolutePath());

		// Procesar los archivos seleccionados
		for (const auto &archivo : listadoArchivos) {
			procesarArchivo(archivo);
		}
	}
}

/**
 * @brief Devuelve los datos de las descargas que el usuario especificó
 * @return Datos del listado de las descargas procesadas
 */
QVector<_NuevaDescarga> VentanaAgregarDescargasDesdeArchivos::obtenerDatosDescargas() {
	return _listadoElementosProcesados;
}

/**
 * @brief Procesa un archivo en búsqueda de entradas compatibles
 */
void VentanaAgregarDescargasDesdeArchivos::procesarArchivo(const QString &archivo) {
	QFile lector(archivo);

	if (lector.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
		return;
	}

	while (lector.atEnd() == false) {
		QList<QByteArray> campos = lector.readLine().split('\t');
		QString nombre, enlace;

		if (campos.size() > 0) {
			if (campos[0].startsWith("https://s3.todus.cu/") == true) {
				_NuevaDescarga nuevaDescarga;

				if (campos.size() > 1) {
					nuevaDescarga.nombre = QString::fromLocal8Bit(campos[1]).trimmed();
					nuevaDescarga.enlace = QString::fromLocal8Bit(campos[0]).trimmed();
				} else {
					nuevaDescarga.nombre = QString::fromLocal8Bit(campos[0]).trimmed();
					nuevaDescarga.nombre.remove(0, campos[0].lastIndexOf("/") + 1);
					nuevaDescarga.enlace = QString::fromLocal8Bit(campos[0]).trimmed();
				}
				_modeloElementosProcesados->appendRow(QList<QStandardItem *>{new QStandardItem(nuevaDescarga.nombre), new QStandardItem(nuevaDescarga.enlace)});
				_listadoElementosProcesados.push_back(nuevaDescarga);
			}
		}
	}
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaAgregarDescargasDesdeArchivos::construirIU() {
	QIcon iconoAgregar = QIcon(obtenerRutaIcono() + "agregar-desde-archivo.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog);
	setStyleSheet("QPushButton, QComboBox { qproperty-iconSize: 24px 24px; } ");

	QVBoxLayout *diseno = new QVBoxLayout();

	QGroupBox *cajaDatos = new QGroupBox();
	cajaDatos->setTitle("Datos de las descargas");
	cajaDatos->setMinimumWidth(600);

	QFormLayout *disenoFormulario = new QFormLayout();

	_botonSeleccionarArchivos = new QPushButton();
	_botonSeleccionarArchivos->setIcon(QIcon(obtenerRutaIcono() + "importar.svg"));
	_botonSeleccionarArchivos->setText("Seleccionar los archivos y procesarlos");
	connect(_botonSeleccionarArchivos, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::eventoSeleccionarArchivosAProcesar);

	_modeloElementosProcesados = new QStandardItemModel();
	QTreeView *elementosProcesados = new QTreeView(this);
	elementosProcesados->setAlternatingRowColors(true);
	elementosProcesados->setEditTriggers(QAbstractItemView::NoEditTriggers);
	elementosProcesados->setItemsExpandable(false);
	elementosProcesados->setModel(_modeloElementosProcesados);
	elementosProcesados->setRootIsDecorated(false);
	elementosProcesados->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	elementosProcesados->header()->setStretchLastSection(true);

	_categoria = new QComboBox();
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-programas.svg"), "Programas", _ListadoCategorias::Programas);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-musica.svg"), "Musica", _ListadoCategorias::Musica);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-videos.svg"), "Videos", _ListadoCategorias::Videos);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-otros.svg"), "Otros", _ListadoCategorias::Otros);

	_iniciar = new QCheckBox();
	_iniciar->setChecked(_configuracion.value("atds3/iniciarDescargaAgregarDescarga", true).toBool());

	disenoFormulario->addRow("", _botonSeleccionarArchivos);
	disenoFormulario->addRow("Elementos procesados:", elementosProcesados);
	disenoFormulario->addRow("Categoría:", _categoria);
	disenoFormulario->addRow("Iniciar descargas?", _iniciar);

	cajaDatos->setLayout(disenoFormulario);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonAgregar = new QPushButton(iconoAgregar, "Agregar descargas");
	botonAgregar->setDefault(true);
	connect(botonAgregar, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::eventoAgregarDescargas);

	QPushButton *botonCancelar = new QPushButton(QIcon(obtenerRutaIcono() + "cancelar.svg"), "Cancelar");
	connect(botonCancelar, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::reject);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonAgregar);
	filaBotones->addWidget(botonCancelar);

	diseno->addWidget(cajaDatos);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
