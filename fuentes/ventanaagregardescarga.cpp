#include "ventanaagregardescarga.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>


VentanaAgregarDescarga::VentanaAgregarDescarga(QWidget *parent)
	: QDialog(parent), _tituloVentana("Agregar descarga") {
	construirIU();
}

/**
 * @brief Limpia los campos
 */
void VentanaAgregarDescarga::limpiarCampos() {
	_enlace->setText("");
	_enlace->setFocus();
	_nombre->setText("");
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar'
 */
void VentanaAgregarDescarga::eventoAgregarDescarga() {
	if (_enlace->text().trimmed().size() == 0) {
		return;
	}

	emit accept();
}

/**
 * @brief Devuelve los datos de la descarga que el usuario especificó
 * @return Datos de la descarga especificada
 */
_NuevaDescarga VentanaAgregarDescarga::obtenerDatosDescarga() {
	_NuevaDescarga datos;

	datos.enlace = _enlace->text().trimmed();
	if (_nombre->text().trimmed().size() > 0) {
		datos.nombre = _nombre->text().trimmed();
	} else {
		datos.nombre = datos.enlace;
	}
	datos.categoria = _categoria->currentData().toInt();
	datos.iniciar = _iniciar->isChecked();

	return datos;
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaAgregarDescarga::construirIU() {
	QIcon iconoAgregar = QIcon(":/iconos/agregar.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog | Qt::Popup);

	QVBoxLayout *diseno = new QVBoxLayout();

	QGroupBox *cajaDatos = new QGroupBox();
	cajaDatos->setTitle("Datos de la descarga");
	cajaDatos->setMinimumWidth(500);

	QFormLayout *disenoFormulario = new QFormLayout();
	_enlace = std::make_unique<QLineEdit>();
	_nombre = std::make_unique<QLineEdit>();
	_categoria = std::make_unique<QComboBox>();
	_categoria->addItem(QIcon(":/iconos/categoria-programas.svg"), "Programas", _ListadoCategorias::Programas);
	_categoria->addItem(QIcon(":/iconos/categoria-musica.svg"), "Musica", _ListadoCategorias::Musica);
	_categoria->addItem(QIcon(":/iconos/categoria-videos.svg"), "Videos", _ListadoCategorias::Videos);
	_categoria->addItem(QIcon(":/iconos/categoria-otros.svg"), "Otros", _ListadoCategorias::Otros);
	_iniciar = std::make_unique<QCheckBox>();
	_iniciar->setChecked(true);

	disenoFormulario->addRow("Enlace:", _enlace.get());
	disenoFormulario->addRow("Nombre del archivo:", _nombre.get());
	disenoFormulario->addRow("Categoría:", _categoria.get());
	disenoFormulario->addRow("Iniciar descarga?", _iniciar.get());

	cajaDatos->setLayout(disenoFormulario);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonAgregar = new QPushButton(iconoAgregar, "Agregar");
	botonAgregar->setDefault(true);
	connect(botonAgregar, &QPushButton::clicked, this, &VentanaAgregarDescarga::eventoAgregarDescarga);

	QPushButton *botonCancelar = new QPushButton(QIcon(":/iconos/cancelar.svg"), "Cancelar");
	connect(botonCancelar, &QPushButton::clicked, this, &VentanaAgregarDescarga::reject);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonAgregar);
	filaBotones->addWidget(botonCancelar);

	diseno->addWidget(cajaDatos);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
