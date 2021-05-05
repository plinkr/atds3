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


VentanaAgregarDescarga::VentanaAgregarDescarga(QWidget *padre)
	: QDialog(padre), _tituloVentana("Agregar descarga") {
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
	QIcon iconoAgregar = QIcon(obtenerRutaIcono() + "agregar.svg");

	setWindowIcon(iconoAgregar);
	setWindowTitle(_tituloVentana);
	setWindowFlags(Qt::Dialog | Qt::Popup);
	setStyleSheet("QPushButton, QComboBox { qproperty-iconSize: 24px 24px; } ");

	QVBoxLayout *diseno = new QVBoxLayout();

	QGroupBox *cajaDatos = new QGroupBox();
	cajaDatos->setTitle("Datos de la descarga");
	cajaDatos->setMinimumWidth(500);

	QFormLayout *disenoFormulario = new QFormLayout();
	_enlace = new QLineEdit();
	_nombre = new QLineEdit();
	_categoria = new QComboBox();
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-programas.svg"), "Programas", _ListadoCategorias::Programas);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-musica.svg"), "Musica", _ListadoCategorias::Musica);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-videos.svg"), "Videos", _ListadoCategorias::Videos);
	_categoria->addItem(QIcon(obtenerRutaIcono() + "categoria-otros.svg"), "Otros", _ListadoCategorias::Otros);
	_iniciar = new QCheckBox();
	_iniciar->setChecked(true);

	disenoFormulario->addRow("Enlace:", _enlace);
	disenoFormulario->addRow("Nombre del archivo:", _nombre);
	disenoFormulario->addRow("Categoría:", _categoria);
	disenoFormulario->addRow("Iniciar descarga?", _iniciar);

	cajaDatos->setLayout(disenoFormulario);

	QHBoxLayout *filaBotones = new QHBoxLayout();

	QPushButton *botonAgregar = new QPushButton(iconoAgregar, "Agregar");
	botonAgregar->setDefault(true);
	connect(botonAgregar, &QPushButton::clicked, this, &VentanaAgregarDescarga::eventoAgregarDescarga);

	QPushButton *botonCancelar = new QPushButton(QIcon(obtenerRutaIcono() + "cancelar.svg"), "Cancelar");
	connect(botonCancelar, &QPushButton::clicked, this, &VentanaAgregarDescarga::reject);

	filaBotones->addStretch(1);
	filaBotones->addWidget(botonAgregar);
	filaBotones->addWidget(botonCancelar);

	diseno->addWidget(cajaDatos);
	diseno->addLayout(filaBotones);

	setLayout(diseno);
}
