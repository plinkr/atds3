#include "ventanaagregardescargasdesdearchivos.hpp"
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
}

/**
 * @brief Limpia los campos
 */
void VentanaAgregarDescargasDesdeArchivos::limpiarCampos() {
	_listadoElementosProcesados.clear();
	_modeloElementosProcesados->clear();
	_modeloElementosProcesados->setColumnCount(2);
	_modeloElementosProcesados->setHeaderData(0, Qt::Horizontal, "Nombre");
	_modeloElementosProcesados->setHeaderData(1, Qt::Horizontal, "Enlace");
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descargas'
 */
void VentanaAgregarDescargasDesdeArchivos::eventoAgregarDescargas() {
	for (_NuevaDescarga &nuevaDescarga : _listadoElementosProcesados) {
		nuevaDescarga.categoria = _categoria->currentData().toInt();
		nuevaDescarga.iniciar = _iniciar->isChecked();
	}

	emit accept();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Seleccionar archivo y procesar'
 */
void VentanaAgregarDescargasDesdeArchivos::eventoSeleccionarArchivoAProcesar() {
	QFileDialog dialogoSeleccion(this);
	QStringList listadoArchivos;

	dialogoSeleccion.setFileMode(QFileDialog::ExistingFiles);
	dialogoSeleccion.setNameFilter(tr("Archivos de texto(*.txt)"));
	if (dialogoSeleccion.exec() == true) {
		listadoArchivos = dialogoSeleccion.selectedFiles();

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

	limpiarCampos();

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
					nuevaDescarga.nombre.remove(0, campos[0].lastIndexOf("/"));
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
	QIcon iconoAgregar = QIcon(":/iconos/agregar-desde-archivo.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog | Qt::Popup);
	setStyleSheet("QPushButton, QComboBox { qproperty-iconSize: 24px 24px; } ");

	QVBoxLayout *diseno = new QVBoxLayout();

	QGroupBox *cajaDatos = new QGroupBox();
	cajaDatos->setTitle("Datos de las descargas");
	cajaDatos->setMinimumWidth(600);

	QFormLayout *disenoFormulario = new QFormLayout();

	QPushButton *botonSeleccionarArchivo = new QPushButton();
	botonSeleccionarArchivo->setIcon(QIcon(":/iconos/importar.svg"));
	botonSeleccionarArchivo->setText("Seleccionar archivo y procesar");
	connect(botonSeleccionarArchivo, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::eventoSeleccionarArchivoAProcesar);

	_modeloElementosProcesados = std::make_unique<QStandardItemModel>();
	QTreeView *elementosProcesados = new QTreeView(this);
	elementosProcesados->setAlternatingRowColors(true);
	elementosProcesados->setEditTriggers(QAbstractItemView::NoEditTriggers);
	elementosProcesados->setItemsExpandable(false);
	elementosProcesados->setModel(_modeloElementosProcesados.get());
	elementosProcesados->setRootIsDecorated(false);
	elementosProcesados->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	elementosProcesados->header()->setStretchLastSection(true);

	_categoria = std::make_unique<QComboBox>();
	_categoria->addItem(QIcon(":/iconos/categoria-programas.svg"), "Programas", _ListadoCategorias::Programas);
	_categoria->addItem(QIcon(":/iconos/categoria-musica.svg"), "Musica", _ListadoCategorias::Musica);
	_categoria->addItem(QIcon(":/iconos/categoria-videos.svg"), "Videos", _ListadoCategorias::Videos);
	_categoria->addItem(QIcon(":/iconos/categoria-otros.svg"), "Otros", _ListadoCategorias::Otros);

	_iniciar = std::make_unique<QCheckBox>();
	_iniciar->setChecked(true);

	disenoFormulario->addRow("", botonSeleccionarArchivo);
	disenoFormulario->addRow("Elementos procesados:", elementosProcesados);
	disenoFormulario->addRow("Categoría:", _categoria.get());
	disenoFormulario->addRow("Iniciar descargas?", _iniciar.get());

	cajaDatos->setLayout(disenoFormulario);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonAgregar = new QPushButton(iconoAgregar, "Agregar descargas");
	botonAgregar->setDefault(true);
	connect(botonAgregar, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::eventoAgregarDescargas);

	QPushButton *botonCancelar = new QPushButton(QIcon(":/iconos/cancelar.svg"), "Cancelar");
	connect(botonCancelar, &QPushButton::clicked, this, &VentanaAgregarDescargasDesdeArchivos::reject);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonAgregar);
	filaBotones->addWidget(botonCancelar);

	diseno->addWidget(cajaDatos);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
