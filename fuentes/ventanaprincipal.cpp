#include "ventanaprincipal.hpp"
#include "delegacioniconoestado.hpp"
#include "delegacionbarraprogreso.hpp"
#include "delegacionvelocidad.hpp"
#include "modelocategorias.hpp"
#include "modeloentradas.hpp"
#include "descarga.hpp"
#include "gestordescargas.hpp"
#include "ventanaagregardescarga.hpp"
#include "ventanaagregardescargasdesdearchivo.hpp"
#include "ventanaconfiguracion.hpp"
#include "main.hpp"
#include <QGuiApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include <QDockWidget>
#include <QStatusBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QListView>
#include <QProgressBar>
#include <QTreeView>
#include <QHeaderView>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QSettings>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QMimeData>
#include <QUrl>
#include <QScreen>
#include <iostream>


VentanaPrincipal::VentanaPrincipal(QWidget *parent)
	: QMainWindow(parent), _categoriaActiva(0) {
	Q_INIT_RESOURCE(iconos);

	setAcceptDrops(true);

	construirIU();

	_administradorAccesoRedAvatarTodus = new QNetworkAccessManager(this);
	_administradorAccesoRedAvatarTodus->setProxy(obtenerConfiguracionProxy());

	_toDus = QSharedPointer<toDus>(new toDus());
	connect(_toDus.get(), &toDus::estadoCambiado, this, &VentanaPrincipal::actualizarEstadoTodus);

	_gestorDescargas = new GestorDescargas(this);
	_gestorDescargas->start();

	QList<QSharedPointer<ModeloEntradas>> modelos {_modeloCategoriaProgramas, _modeloCategoriaMusica, _modeloCategoriaVideos, _modeloCategoriaOtros};
	int estado = 0;
	for (auto modelo : modelos) {
		for (int f = 0; f < modelo->rowCount(); f++) {
			estado = modelo->data(modelo->index(f, 1)).toInt();
			if (estado == _ListadoEstados::EnEsperaIniciar || estado == _ListadoEstados::Iniciada) {
				_gestorDescargas->agregarDescarga(f, modelo->data(modelo->index(f, 0)).toUInt(), modelo, _modelocategoriaDescargas);
			}
		}
	}
}
VentanaPrincipal::~VentanaPrincipal() {
	// Emitir la señal de detención de la aplicación para que todos los hilos de descargas cesen su ejecución
	emit detenerEjecucion();
}

void VentanaPrincipal::closeEvent(QCloseEvent *evento) {
	QSettings configuracion;

	// Oculta la ventana
	hide();

	// Guarda los cambios pendientes a la base de datos
	_modeloCategoriaProgramas->submitAll();
	_modeloCategoriaMusica->submitAll();
	_modeloCategoriaVideos->submitAll();
	_modeloCategoriaOtros->submitAll();

	// Desconecta la actualización del estado de la sesión toDus
	disconnect(_toDus.get(), &toDus::estadoCambiado, this, &VentanaPrincipal::actualizarEstadoTodus);

	// Detiene adecuadamente todas las descargas activas
	_gestorDescargas->requestInterruption();
	_gestorDescargas->wait();

	// Desconecta la sesión toDus
	_toDus->desconectar();
	_toDus.clear();

	// Guarda la geometría de la ventana en las configuraciones
	configuracion.setValue("atds3/geometria", saveGeometry());

	// Continua el proceso de cerrado de la aplicación
	evento->accept();
}

void VentanaPrincipal::dragEnterEvent(QDragEnterEvent *evento) {
	const QMimeData *mime = evento->mimeData();

	if (mime->hasUrls()) {
		QList<QUrl> urls = mime->urls();

		for (const auto &url : urls) {
			if (url.toLocalFile().toLower().endsWith(".txt") == true) {
				_listadoArchivosSoltar = urls;
				evento->acceptProposedAction();
				return;
			}
		}
	}
}

void VentanaPrincipal::dragMoveEvent(QDragMoveEvent *evento) {
	for (const auto &url : _listadoArchivosSoltar) {
		if (url.toLocalFile().toLower().endsWith(".txt") == true) {
			evento->acceptProposedAction();
			return;
		}
	}
}

void VentanaPrincipal::dragLeaveEvent(QDragLeaveEvent *evento) {
	_listadoArchivosSoltar.clear();
	evento->accept();
}

void VentanaPrincipal::dropEvent(QDropEvent *) {
	/**
	 * Construye la ventana 'Agregar descargas desde archivo'
	 */
	_ventanaAgregarDescargasDesdeArchivo = new VentanaAgregarDescargasDesdeArchivos(this);
	_ventanaAgregarDescargasDesdeArchivo->setModal(true);
	connect(_ventanaAgregarDescargasDesdeArchivo, &VentanaAgregarDescargasDesdeArchivos::accepted, this, &VentanaPrincipal::agregarDescargasDesdeArchivo);

	for (const auto &url : _listadoArchivosSoltar) {
		if (url.toLocalFile().toLower().endsWith(".txt") == true) {
			_ventanaAgregarDescargasDesdeArchivo->procesarArchivo(url.toLocalFile());
		}
	}

	_listadoArchivosSoltar.clear();

	_ventanaAgregarDescargasDesdeArchivo->show();
}

/**
 * Agrega la descarga especificada por el usuario en la ventana 'Agrega descarga'
 */
void VentanaPrincipal::agregarDescarga() {
	_NuevaDescarga datos = _ventanaAgregarDescarga->obtenerDatosDescarga();
	QSharedPointer<ModeloEntradas> modelo;
	QSettings configuracion;
	QString rutaDescarga = configuracion.value("descargas/ruta").toString();

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	switch (datos.categoria) {
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			rutaDescarga += "/programas";
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			rutaDescarga += "/musica";
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			rutaDescarga += "/videos";
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
			rutaDescarga += "/otros";
			break;
	}

	QSqlRecord registro = modelo->record();
	registro.remove(0); // Campo 'id'
	registro.setValue("categoria", datos.categoria);
	registro.setValue("estado", _ListadoEstados::Pausada);
	registro.setValue("enlace", datos.enlace);
	registro.setValue("ruta", rutaDescarga);
	registro.setValue("nombre", datos.nombre);
	registro.setValue("completado", 0);
	registro.setValue("velocidad", 0);

	modelo->insertRecord(-1, registro);

	modelo->submitAll();
	_modelocategoriaDescargas->select();
	qApp->processEvents();

	if (datos.iniciar == true) {
		_gestorDescargas->agregarDescarga(modelo->rowCount() - 1, modelo->data(modelo->index(modelo->rowCount() - 1, 0)).toUInt(), modelo, _modelocategoriaDescargas);
	}

	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * Agrega las descargas procesadas desde el archivo seleccionado por el usuario en la ventana 'Agrega descargas desde archivo'
 */
void VentanaPrincipal::agregarDescargasDesdeArchivo() {
	QVector<_NuevaDescarga> listadoDescargas = _ventanaAgregarDescargasDesdeArchivo->obtenerDatosDescargas();
	QSharedPointer<ModeloEntradas> modelo;
	QSettings configuracion;
	QString rutaDescarga = configuracion.value("descargas/ruta").toString();

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	switch (listadoDescargas[0].categoria) {
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			rutaDescarga += "/programas";
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			rutaDescarga += "/musica";
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			rutaDescarga += "/videos";
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
			rutaDescarga += "/otros";
	}

	for (_NuevaDescarga &nuevaDescarga : listadoDescargas) {
		QSqlRecord registro = modelo->record();
		registro.remove(0); // Campo 'id'
		registro.setValue("categoria", nuevaDescarga.categoria);
		registro.setValue("estado", _ListadoEstados::Pausada);
		registro.setValue("enlace", nuevaDescarga.enlace);
		registro.setValue("ruta", rutaDescarga);
		registro.setValue("nombre", nuevaDescarga.nombre);
		registro.setValue("completado", 0);
		registro.setValue("velocidad", 0);

		modelo->insertRecord(-1, registro);

		modelo->submitAll();
		_modelocategoriaDescargas->select();
		qApp->processEvents();

		if (nuevaDescarga.iniciar == true) {
			_gestorDescargas->agregarDescarga(modelo->rowCount() - 1, modelo->data(modelo->index(modelo->rowCount() - 1, 0)).toUInt(), modelo, _modelocategoriaDescargas);
		}
	}

	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Procesa las configuraciones guardadas y ejecuta las acciones adecuadas
 */
void VentanaPrincipal::procesarCambiosConfiguracion() {
	QSettings configuracion;
	bool reconexionRequerida = false;

	if (_configuracionTelefono != configuracion.value("todus/telefono").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionFichaAcceso != configuracion.value("todus/fichaAcceso").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorAutentificacion != configuracion.value("avanzadas/ipServidorAutentificacion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorAutentificacion != configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorAutentificacion != configuracion.value("avanzadas/nombreDNSServidorAutentificacion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorSesion != configuracion.value("avanzadas/ipServidorSesion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorSesion != configuracion.value("avanzadas/puertoServidorSesion", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorSesion != configuracion.value("avanzadas/nombreDNSServidorSesion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorS3 != configuracion.value("avanzadas/ipServidorS3").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorS3 != configuracion.value("avanzadas/puertoServidorS3", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorS3 != configuracion.value("avanzadas/nombreDNSServidorS3").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionTipoProxy != configuracion.value("proxy/tipo").toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionServidorProxy != configuracion.value("proxy/servidor", "").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorProxy != configuracion.value("proxy/puerto").toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionUsuarioServidorProxy != configuracion.value("proxy/usuario", "").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionContrasenaServidorProxy != configuracion.value("proxy/contrasena", "").toString()) {
		reconexionRequerida = true;
	}

	if (reconexionRequerida == true) {
		_administradorAccesoRedAvatarTodus->setProxy(obtenerConfiguracionProxy());
		_toDus->reconectar();
	}
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descarga'
 */
void VentanaPrincipal::eventoAgregarDescarga() {
	/**
	 * Construye la ventana 'Agregar descarga'
	 */
	_ventanaAgregarDescarga = new VentanaAgregarDescarga(this);
	_ventanaAgregarDescarga->setModal(true);
	connect(_ventanaAgregarDescarga, &VentanaAgregarDescarga::accepted, this, &VentanaPrincipal::agregarDescarga);
//	_ventanaAgregarDescarga->limpiarCampos();
	_ventanaAgregarDescarga->show();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Agregar descargas desde archivo'
 */
void VentanaPrincipal::eventoAgregarDescargasDesdeArchivo() {

	/**
	 * Construye la ventana 'Agregar descargas desde archivo'
	 */
	_ventanaAgregarDescargasDesdeArchivo = new VentanaAgregarDescargasDesdeArchivos(this);
	_ventanaAgregarDescargasDesdeArchivo->setModal(true);
	connect(_ventanaAgregarDescargasDesdeArchivo, &VentanaAgregarDescargasDesdeArchivos::accepted, this, &VentanaPrincipal::agregarDescargasDesdeArchivo);
//	_ventanaAgregarDescargasDesdeArchivo->limpiarCampos();
	_ventanaAgregarDescargasDesdeArchivo->show();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar descarga'
 */
void VentanaPrincipal::eventoEliminarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modelocategoriaDescargas;
			break;
		case 0x02:
			modelo = _modeloCategoriaFinalizadas;
			break;
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
	}

	if (_listadoDescargas->selectionModel()->selectedRows().isEmpty() == false) {
		for (const auto &i : _listadoDescargas->selectionModel()->selectedRows()) {
			_gestorDescargas->detenerDescarga(modelo->data(modelo->index(i.row(), 0)).toUInt());
			modelo->removeRow(i.row());
			qApp->processEvents();
		}
		modelo->submitAll();
	}

	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar todas las descargas'
 */
void VentanaPrincipal::eventoEliminarTodasDescargas() {
	QSharedPointer<ModeloEntradas> modelo;

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modelocategoriaDescargas;
			break;
		case 0x02:
			modelo = _modeloCategoriaFinalizadas;
			break;
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
	}

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	for (int f = 0; f < modelo->rowCount(); f++) {
		_gestorDescargas->detenerDescarga(modelo->data(modelo->index(f, 0)).toUInt());
		qApp->processEvents();
	}
	modelo->removeRows(0, modelo->rowCount());
	modelo->submitAll();
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

void VentanaPrincipal::eventoSubir() {
	_toDus->solicitarEnlaceFirmadoSubida();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar descarga'
 */
void VentanaPrincipal::eventoIniciarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modelocategoriaDescargas;
			break;
		case 0x02:
			return;
			break;
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
	}

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	if (_listadoDescargas->selectionModel()->selectedRows().isEmpty() == false) {
		for (const auto &i : _listadoDescargas->selectionModel()->selectedRows()) {
			if (modelo->data(modelo->index(i.row(), 1)).toInt() == _ListadoEstados::Pausada) {
				_gestorDescargas->agregarDescarga(i.row(), modelo->data(modelo->index(i.row(), 0)).toUInt(), modelo, _modelocategoriaDescargas);
			}
		}
	}
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Pausar descarga'
 */
void VentanaPrincipal::eventoPausarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modelocategoriaDescargas;
			break;
		case 0x02:
			modelo = _modeloCategoriaFinalizadas;
			break;
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
	}

	if (_listadoDescargas->selectionModel()->selectedRows().isEmpty() == false) {
		for (const auto &i : _listadoDescargas->selectionModel()->selectedRows()) {
			_gestorDescargas->detenerDescarga(modelo->data(modelo->index(i.row(), 0)).toUInt());
		}
	}

	_modelocategoriaDescargas->select();
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar todas las descargas'
 */
void VentanaPrincipal::eventoIniciarTodasDescargas() {
	QSharedPointer<ModeloEntradas> modelo;

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modelocategoriaDescargas;
			return;
		case 0x02:
			return;
		case _ListadoCategorias::Programas:
			modelo = _modeloCategoriaProgramas;
			break;
		case _ListadoCategorias::Musica:
			modelo = _modeloCategoriaMusica;
			break;
		case _ListadoCategorias::Videos:
			modelo = _modeloCategoriaVideos;
			break;
		case _ListadoCategorias::Otros:
			modelo = _modeloCategoriaOtros;
	}

	for (int f = 0; f < modelo->rowCount(); f++) {
		if (modelo->data(modelo->index(f, 1)).toInt() == _ListadoEstados::Pausada) {
			_gestorDescargas->agregarDescarga(f, modelo->data(modelo->index(f, 0)).toUInt(), modelo, _modelocategoriaDescargas);
		}
	}
	_modelocategoriaDescargas->select();
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Pausar todas las descargas'
 */
void VentanaPrincipal::eventoPausarTodasDescargas() {
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	_gestorDescargas->detenerDescargas();
	_modeloCategoriaProgramas->submitAll();
	_modeloCategoriaMusica->submitAll();
	_modeloCategoriaVideos->submitAll();
	_modeloCategoriaOtros->submitAll();
	_modelocategoriaDescargas->select();
	centralWidget()->update();
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Configuración'
 */
void VentanaPrincipal::eventoConfiguracion() {
	QSettings configuracion;

	_configuracionTelefono = configuracion.value("todus/telefono").toString();
	_configuracionFichaAcceso = configuracion.value("todus/fichaAcceso").toString();
	_configuracionIpServidorAutentificacion = configuracion.value("avanzadas/ipServidorAutentificacion").toString();
	_configuracionPuertoServidorAutentificacion = configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	_configuracionNombreDNSServidorAutentificacion = configuracion.value("avanzadas/nombreDNSServidorAutentificacion").toString();
	_configuracionIpServidorSesion = configuracion.value("avanzadas/ipServidorSesion").toString();
	_configuracionPuertoServidorSesion = configuracion.value("avanzadas/puertoServidorSesion", 443).toInt();
	_configuracionNombreDNSServidorSesion = configuracion.value("avanzadas/nombreDNSServidorSesion").toString();
	_configuracionIpServidorS3 = configuracion.value("avanzadas/ipServidorS3").toString();
	_configuracionPuertoServidorS3 = configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	_configuracionNombreDNSServidorS3 = configuracion.value("avanzadas/nombreDNSServidorS3").toString();
	_configuracionTipoProxy = configuracion.value("proxy/tipo").toInt();
	_configuracionServidorProxy = configuracion.value("proxy/servidor").toString();
	_configuracionPuertoServidorProxy = configuracion.value("proxy/puerto").toInt();
	_configuracionUsuarioServidorProxy = configuracion.value("proxy/usuario").toString();
	_configuracionContrasenaServidorProxy = configuracion.value("proxy/contrasena").toString();

	/**
	 * Construye la ventana 'Agregar descargas desde archivo'
	 */
	_ventanaConfiguracion = new VentanaConfiguracion(this);
	_ventanaConfiguracion->setModal(true);
	connect(_ventanaConfiguracion, &VentanaConfiguracion::accepted, this, &VentanaPrincipal::procesarCambiosConfiguracion);
	_ventanaConfiguracion->show();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Acerca de'
 */
void VentanaPrincipal::eventoAcerca() {}

/**
 * @brief Evento que se dispara cuando se selecciona un elemento en el listado de categorías
 * @param indice Índice del modelo
 */
void VentanaPrincipal::eventoCategoriaSeleccionada(const QModelIndex &indice) {
	QSettings configuracion;
	QSharedPointer<ModeloEntradas> modeloActivo;

	_modeloCategoriaProgramas->submitAll();
	_modeloCategoriaMusica->submitAll();
	_modeloCategoriaVideos->submitAll();
	_modeloCategoriaOtros->submitAll();

	configuracion.setValue("atds3/ultimaCategoria", indice.row());

	switch (indice.row()) {
		case 0:
			modeloActivo = _modelocategoriaDescargas;
			break;
		case 1:
			modeloActivo = _modeloCategoriaFinalizadas;
			break;
		case 2:
			modeloActivo = _modeloCategoriaProgramas;
			break;
		case 3:
			modeloActivo = _modeloCategoriaMusica;
			break;
		case 4:
			modeloActivo = _modeloCategoriaVideos;
			break;
		case 5:
			modeloActivo = _modeloCategoriaOtros;
			break;
	}

	modeloActivo->select();
	_listadoDescargas->setModel(modeloActivo.get());
	_categoriaActiva = indice.row() + 1;

	if (indice.row() == 1) {
		_listadoDescargas->hideColumn(4);
	} else {
		_listadoDescargas->showColumn(4);
	}

	emit categoriaSeleccionada(modeloActivo);
}

/**
 * @brief Evento que se dispara cuando se recibe algún dato de la descarga del avatar
 */
void VentanaPrincipal::eventoRecepcionDatosAvatarTodus() {
	_archivoAvatarTodus.write(_respuestaAvatarTodus->readAll());
}

/**
 * @brief Evento que se dispara cuando se finaliza la descarga del avatar
 */
void VentanaPrincipal::eventoDescargaTerminadaAvatarTodus() {
	_archivoAvatarTodus.close();

	if (_respuestaAvatarTodus->error() == QNetworkReply::NetworkError::NoError) {
		_avatarTodus->setPixmap(QIcon(_rutaAvatarTodus).pixmap(QSize(48, 48)));
	}
}

/**
 * @brief Actualiza la etiqueta que refleja el estado de la sesión toDus
 * @param Nuevo estado
 */
void VentanaPrincipal::actualizarEstadoTodus(toDus::Estado estado) {
	QSettings configuracion;

	if (_estadoSesionTodus.isNull() == true) {
		return;
	}

	switch (estado) {
		case toDus::Estado::Desconectado:
			_estadoSesionTodus->setText("Desconectado.");
			break;
		case toDus::Estado::ResolviendoNombreDNS:
			_estadoSesionTodus->setText("Resolviendo nombre DNS...");
			break;
		case toDus::Estado::Conectando:
			_estadoSesionTodus->setText("Conectando...");
			break;
		case toDus::Estado::Conectado:
			_estadoSesionTodus->setText("Conectado.");
			break;
		case toDus::Estado::IniciandoSesion:
			_estadoSesionTodus->setText("Iniciando sesión...");
			break;
		case toDus::Estado::Listo:
			_estadoSesionTodus->setText("<b>" + configuracion.value("todus/nombre", "").toString() + "</b><br/>Listo para usar.");
			actualizarAvatar();
			break;
		case toDus::Estado::Desconectando:
			_estadoSesionTodus->setText("Desconectando...");
			break;
	}
}

/**
 * @brief Construye los botones de la barra de herramientas
 * @param barraHerramientas Puntero al objeto de la barra de herramientas
 */
void VentanaPrincipal::construirBotonesBarraHerramientas(QToolBar *barraHerramientas) {
	QAction *accionAgregarDescarga = new QAction();
	accionAgregarDescarga->setIcon(QIcon(obtenerRutaIcono() + "agregar.svg"));
	accionAgregarDescarga->setText("Agregar");
	accionAgregarDescarga->setToolTip("Agrega una descarga al listado");
	accionAgregarDescarga->setStatusTip("Agrega una descarga al listado");
	accionAgregarDescarga->setShortcut(QKeySequence(Qt::Key_Insert));
	connect(accionAgregarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoAgregarDescarga);
	barraHerramientas->addAction(accionAgregarDescarga);

	QAction *accionAgregarDescargasDesdeArchivo = new QAction();
	accionAgregarDescargasDesdeArchivo->setIcon(QIcon(obtenerRutaIcono() + "agregar-desde-archivo.svg"));
	accionAgregarDescargasDesdeArchivo->setText("Agregar desde archivo");
	accionAgregarDescargasDesdeArchivo->setToolTip("Agrega un listado de descargas procesados desde un archivo");
	accionAgregarDescargasDesdeArchivo->setStatusTip("Agrega un listado de descargas procesados desde un archivo");
	accionAgregarDescargasDesdeArchivo->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Insert));
	connect(accionAgregarDescargasDesdeArchivo, &QAction::triggered, this, &VentanaPrincipal::eventoAgregarDescargasDesdeArchivo);
	barraHerramientas->addAction(accionAgregarDescargasDesdeArchivo);

	QAction *accionEliminarDescarga = new QAction();
	accionEliminarDescarga->setIcon(QIcon(obtenerRutaIcono() + "eliminar.svg"));
	accionEliminarDescarga->setText("Eliminar");
	accionEliminarDescarga->setToolTip("Elimina la(s) descarga(s) selecionada(s)");
	accionEliminarDescarga->setStatusTip("Elimina la(s) descarga(s) selecionada(s)");
	accionEliminarDescarga->setShortcut(QKeySequence(Qt::Key_Delete));
	connect(accionEliminarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoEliminarDescarga);
	barraHerramientas->addAction(accionEliminarDescarga);

	QAction *accionEliminarTodasDescargas = new QAction();
	accionEliminarTodasDescargas->setIcon(QIcon(obtenerRutaIcono() + "limpiar.svg"));
	accionEliminarTodasDescargas->setText("Eliminar todas");
	accionEliminarTodasDescargas->setToolTip("Elimina todas las descargas del listado");
	accionEliminarTodasDescargas->setStatusTip("Elimina todas las descargas del listado");
	accionEliminarTodasDescargas->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Delete));
	connect(accionEliminarTodasDescargas, &QAction::triggered, this, &VentanaPrincipal::eventoEliminarTodasDescargas);
	barraHerramientas->addAction(accionEliminarTodasDescargas);

	barraHerramientas->addSeparator();
/*
	QAction *accionSubir = new QAction();
	accionSubir->setIcon(QIcon(obtenerRutaIcono() + "subida.svg"));
	accionSubir->setText("Subir");
	accionSubir->setToolTip("Subida");
	accionSubir->setStatusTip("Subida");
	accionSubir->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
	connect(accionSubir, &QAction::triggered, this, &VentanaPrincipal::eventoSubir);
	barraHerramientas->addAction(accionSubir);

	barraHerramientas->addSeparator();
*/
	QAction *accionIniciarDescarga = new QAction();
	accionIniciarDescarga->setIcon(QIcon(obtenerRutaIcono() + "iniciar.svg"));
	accionIniciarDescarga->setText("Iniciar descarga");
	accionIniciarDescarga->setToolTip("Inicia la descarga seleccionada");
	accionIniciarDescarga->setStatusTip("Inicia la descarga seleccionada");
	accionIniciarDescarga->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
	connect(accionIniciarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoIniciarDescarga);
	barraHerramientas->addAction(accionIniciarDescarga);

	QAction *accionPausarDescarga = new QAction();
	accionPausarDescarga->setIcon(QIcon(obtenerRutaIcono() + "pausar.svg"));
	accionPausarDescarga->setText("Pausar descarga");
	accionPausarDescarga->setToolTip("Pausa la descarga seleccionada");
	accionPausarDescarga->setStatusTip("Pausa la descarga seleccionada");
	accionPausarDescarga->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
	connect(accionPausarDescarga, &QAction::triggered, this, &VentanaPrincipal::eventoPausarDescarga);
	barraHerramientas->addAction(accionPausarDescarga);

	QAction *accionIniciarTodasDescargas = new QAction();
	accionIniciarTodasDescargas->setIcon(QIcon(obtenerRutaIcono() + "iniciar-todas.svg"));
	accionIniciarTodasDescargas->setText("Iniciar todas las descargas");
	accionIniciarTodasDescargas->setToolTip("Inicia todas las descargas");
	accionIniciarTodasDescargas->setStatusTip("Inicia todas las descargas");
	accionIniciarDescarga->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_I));
	connect(accionIniciarTodasDescargas, &QAction::triggered, this, &VentanaPrincipal::eventoIniciarTodasDescargas);
	barraHerramientas->addAction(accionIniciarTodasDescargas);

	QAction *accionPausarTodasDescargas = new QAction();
	accionPausarTodasDescargas->setIcon(QIcon(obtenerRutaIcono() + "pausar-todas.svg"));
	accionPausarTodasDescargas->setText("Pausar todas las descargas");
	accionPausarTodasDescargas->setToolTip("Pausa todas las descargas");
	accionPausarTodasDescargas->setStatusTip("Pausa todas las descargas");
	accionPausarTodasDescargas->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_D));
	connect(accionPausarTodasDescargas, &QAction::triggered, this, &VentanaPrincipal::eventoPausarTodasDescargas);
	barraHerramientas->addAction(accionPausarTodasDescargas);

	barraHerramientas->addSeparator();

	QAction *accionConfiguracion = new QAction();
	accionConfiguracion->setIcon(QIcon(obtenerRutaIcono() + "configurar.svg"));
	accionConfiguracion->setText("Configuración");
	accionConfiguracion->setToolTip("Configura datos de la aplicación");
	accionConfiguracion->setStatusTip("Configura datos de la aplicación");
	accionConfiguracion->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
	connect(accionConfiguracion, SIGNAL(triggered(bool)), this, SLOT(eventoConfiguracion()));
	barraHerramientas->addAction(accionConfiguracion);
/*
	barraHerramientas->addSeparator();

	QAction *accionAcerca = new QAction();
	accionAcerca->setIcon(QIcon(obtenerRutaIcono() + "acerca.svg"));
	accionAcerca->setText("Acerca de");
	accionAcerca->setToolTip("Muestra información del creador");
	accionAcerca->setStatusTip("Muestra información del creador");
	connect(accionAcerca, SIGNAL(triggered(bool)), this, SLOT(eventoAcerca()));
	barraHerramientas->addAction(accionAcerca);
*/
	QWidget* elementoEspaciador = new QWidget();
	elementoEspaciador->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_estadoSesionTodus = new QLabel();
	QMargins margenes = _estadoSesionTodus->contentsMargins();
	margenes.setRight(margenes.right() + 10);
	_estadoSesionTodus->setContentsMargins(margenes);
	_estadoSesionTodus->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

	_avatarTodus = new QLabel();
	_avatarTodus->setPixmap(QIcon(obtenerRutaIcono() + "usuario.svg").pixmap(QSize(48, 48)));

	barraHerramientas->addWidget(elementoEspaciador);
	barraHerramientas->addWidget(_estadoSesionTodus);
	barraHerramientas->addWidget(_avatarTodus);

	addToolBar(barraHerramientas);
}

/**
 * @brief Construye las categorías
 * @return Puntero al elemento del listado de categorías
 */
QListView *VentanaPrincipal::construirListadoCategorias() {
	QSettings configuracion;
	_modeloListadoCategorias = new ModeloCategorias();

	QStandardItem *categoriaDescargas = new QStandardItem();
	categoriaDescargas->setIcon(QIcon(obtenerRutaIcono() + "descarga.svg"));
	categoriaDescargas->setText("Descargas");
	categoriaDescargas->setToolTip("Descargas activas, inactivas y finalizadas");
	_modeloListadoCategorias->appendRow(categoriaDescargas);

	QStandardItem *categoriaFinalizadas = new QStandardItem();
	categoriaFinalizadas->setIcon(QIcon(obtenerRutaIcono() + "finalizado.svg"));
	categoriaFinalizadas->setText("Finalizadas");
	categoriaFinalizadas->setToolTip("Descargas finalizadas");
	_modeloListadoCategorias->appendRow(categoriaFinalizadas);

	QStandardItem *categoriaProgramas = new QStandardItem();
	categoriaProgramas->setIcon(QIcon(obtenerRutaIcono() + "categoria-programas.svg"));
	categoriaProgramas->setText("Programas");
	categoriaProgramas->setToolTip("Categoría 'Programas'");
	_modeloListadoCategorias->appendRow(categoriaProgramas);

	QStandardItem *categoriaMusica = new QStandardItem();
	categoriaMusica->setIcon(QIcon(obtenerRutaIcono() + "categoria-musica.svg"));
	categoriaMusica->setText("Música");
	categoriaMusica->setToolTip("Categoría 'Música'");
	_modeloListadoCategorias->appendRow(categoriaMusica);

	QStandardItem *categoriaVideo = new QStandardItem();
	categoriaVideo->setIcon(QIcon(obtenerRutaIcono() + "categoria-videos.svg"));
	categoriaVideo->setText("Videos");
	categoriaVideo->setToolTip("Categoría 'Videos'");
	_modeloListadoCategorias->appendRow(categoriaVideo);

	QStandardItem *categoriaOtros = new QStandardItem();
	categoriaOtros->setIcon(QIcon(obtenerRutaIcono() + "categoria-otros.svg"));
	categoriaOtros->setText("Otros");
	categoriaOtros->setToolTip("Categoría 'Otros'");
	_modeloListadoCategorias->appendRow(categoriaOtros);

	_listadoCategorias = new QListView();
	_listadoCategorias->setAcceptDrops(false);
	_listadoCategorias->setDefaultDropAction(Qt::IgnoreAction);
	_listadoCategorias->setDragDropMode(QAbstractItemView::NoDragDrop);
	_listadoCategorias->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_listadoCategorias->setFlow(QListView::TopToBottom);
	_listadoCategorias->setModel(_modeloListadoCategorias);
	_listadoCategorias->setMovement(QListView::Static);
	_listadoCategorias->setUniformItemSizes(true);
	_listadoCategorias->setStyleSheet("QListView::item { width: 120px; }");
	_listadoCategorias->setItemAlignment(Qt::AlignCenter);
	_listadoCategorias->setWrapping(true);
	_listadoCategorias->setViewMode(QListView::IconMode);
	connect(_listadoCategorias, &QListView::activated, this, &VentanaPrincipal::eventoCategoriaSeleccionada);
	connect(_listadoCategorias, &QListView::pressed, this, &VentanaPrincipal::eventoCategoriaSeleccionada);

	return _listadoCategorias;
}

/**
 * @brief Inicializa la base de datos
 */
void VentanaPrincipal::inicializarBaseDatos() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QString archivoBaseDatos = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + _aplicacionNombreCorto.toLower() + "/" + _aplicacionNombreCorto + ".db";

	db.setDatabaseName(archivoBaseDatos);
	db.open();

	/**
	 * Crea las estructuras de las tablas
	 */
	QSqlQuery solicitudSQL;
	solicitudSQL.exec("CREATE TABLE IF NOT EXISTS entradas (id INTEGER PRIMARY KEY AUTOINCREMENT, estado INTEGER NOT NULL, nombre TEXT NOT NULL, completado INTEGER DEFAULT 0, velocidad INTEGER DEFAULT 0, ruta TEXT NOT NULL, categoria INTEGER NOT NULL, enlace TEXT NOT NULL, totalADescargar INTEGER DEFAULT 0, totalDescargado INTEGER DEFAULT 0)");
}

/**
 * @brief Construye el elemento del listado de descargas
 * @return Puntero al elemento del listado de descargas
 */
QTreeView *VentanaPrincipal::construirListadoDescargas() {
	/**
	 * Ancho y alto mínimo de la ventana. Inicialmente toma 1/3 de los valores actuales
	 * de la pantalla y les fuerza un mínimo sin son inferiores a los valores que se
	 * necesitan para que la ventana se visualice completamente.
	 */
	int ancho = qApp->primaryScreen()->availableSize().width() * 0.33;
	int alto = qApp->primaryScreen()->availableSize().height() * 0.33;

	if (ancho < 600) {
		ancho = 600;
	}
	if (alto < 400) {
		alto = 400;
	}

	/**
	 * Modelo del listado de la categoría 'Descargando'
	 */
	_modelocategoriaDescargas = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modelocategoriaDescargas->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modelocategoriaDescargas->setFilter(QString("categoria != %1").arg(_ListadoCategorias::Subidas));
	_modelocategoriaDescargas->select();

	/**
	 * Modelo del listado de la categoría 'Finalizadas'
	 */
	_modeloCategoriaFinalizadas = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modeloCategoriaFinalizadas->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modeloCategoriaFinalizadas->setFilter(QString("estado = %1").arg(_ListadoEstados::Finalizada));
	_modeloCategoriaFinalizadas->select();

	/**
	 * Modelo del listado de la categoría 'Programas'
	 */
	_modeloCategoriaProgramas = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modeloCategoriaProgramas->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modeloCategoriaProgramas->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Programas));
	_modeloCategoriaProgramas->select();

	/**
	 * Modelo del listado de la categoría 'Música'
	 */
	_modeloCategoriaMusica = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modeloCategoriaMusica->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modeloCategoriaMusica->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Musica));
	_modeloCategoriaMusica->select();

	/**
	 * Modelo del listado de la categoría 'Videos'
	 */
	_modeloCategoriaVideos = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modeloCategoriaVideos->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modeloCategoriaVideos->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Videos));
	_modeloCategoriaVideos->select();

	/**
	 * Modelo del listado de la categoría 'Otros'
	 */
	_modeloCategoriaOtros = QSharedPointer<ModeloEntradas>(new ModeloEntradas());
	_modeloCategoriaOtros->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_modeloCategoriaOtros->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Otros));
	_modeloCategoriaOtros->select();

	_listadoDescargas = new QTreeView();
	_listadoDescargas->setAlternatingRowColors(true);
	_listadoDescargas->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_listadoDescargas->setItemsExpandable(false);
	_listadoDescargas->setModel(_modelocategoriaDescargas.get());
	_listadoDescargas->setMinimumSize(QSize(ancho, alto));
	_listadoDescargas->setRootIsDecorated(false);
	_listadoDescargas->setSelectionBehavior(QAbstractItemView::SelectRows);
	_listadoDescargas->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_listadoDescargas->setSortingEnabled(false);
	_listadoDescargas->setStyleSheet("QTreeView::item { height: 28px; }");
	_listadoDescargas->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	//_listadoDescargas->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_listadoDescargas->header()->setStretchLastSection(true);

	_listadoDescargas->hideColumn(0);
	for(int i = 6; i < 10; i++) {
		_listadoDescargas->hideColumn(i);
	}

	_elementoIconoEstado = new DelegacionIconoEstado();
	_elementoBarraProgreso = new DelegacionBarraProgreso();
	_elementoVelocidad = new DelegacionVelocidad();
	_listadoDescargas->setItemDelegateForColumn(1, _elementoIconoEstado);
	_listadoDescargas->setItemDelegateForColumn(3, _elementoBarraProgreso);
	_listadoDescargas->setItemDelegateForColumn(4, _elementoVelocidad);

	_categoriaActiva = 0x01;

	return _listadoDescargas;
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaPrincipal::construirIU() {
	QSettings configuracion;

	setWindowIcon(QIcon(":/iconos/atds3.svg"));

	// Restaura la geometría de la ventana previamente guardado
	if (configuracion.value("atds3/geometria").toByteArray().size() > 0) {
		restoreGeometry(configuracion.value("atds3/geometria").toByteArray());
	}

	/**
	 * Barra de herramientas
	 */
	QToolBar *barraHerramientasPrincipal = new QToolBar("Principal");
	barraHerramientasPrincipal->setIconSize(QSize(48, 48));
	barraHerramientasPrincipal->setMovable(false);

	construirBotonesBarraHerramientas(barraHerramientasPrincipal);

	/**
	 * Listado de categorías
	 */
	QDockWidget *puertoCategorias = new QDockWidget();
	puertoCategorias->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	puertoCategorias->setFeatures(QDockWidget::DockWidgetMovable);
	puertoCategorias->setFixedWidth(128);

	puertoCategorias->setWidget(construirListadoCategorias());

	addDockWidget(Qt::LeftDockWidgetArea, puertoCategorias);

	/**
	 * Inicializa la base de datos
	 */
	inicializarBaseDatos();

	/**
	 * Construir elemento central: listado de descargas
	 */
	setCentralWidget(construirListadoDescargas());
	//setContentsMargins(QMargins(5, 5, 5, 5));

	// Restaura la última categoría seleccionada
	QModelIndex indiceSeleccionado = _modeloListadoCategorias->index(configuracion.value("atds3/ultimaCategoria", 0).toInt(), 0);
	_listadoCategorias->setCurrentIndex(indiceSeleccionado);
	eventoCategoriaSeleccionada(indiceSeleccionado);

	statusBar()->showMessage("Listo");
}

/**
 * @brief Descarga el avatar del usuario desde la red toDus y lo muestra en la barra de herramientas
 */
void VentanaPrincipal::actualizarAvatar() {
	QSettings configuracion;
	QString enlaceAvatar = configuracion.value("todus/enlaceAvatar").toString();

	if (enlaceAvatar.size() == 0) {
		return;
	}

	QString ipServidorS3 = configuracion.value("avanzadas/ipServidorS3", "").toString();
	QString nombreDNSServidorS3 = configuracion.value("avanzadas/nombreDNSServidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	QUrl url = QUrl(enlaceAvatar);
	QNetworkRequest solicitud;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();
	QString autorizacion = "Bearer " + configuracion.value("todus/fichaAcceso").toString();

	_rutaAvatarTodus = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + _aplicacionNombreCorto + "_avatar.jpg";

	configuracionSSL.setPeerVerifyMode(QSslSocket::VerifyNone);
	solicitud.setSslConfiguration(configuracionSSL);
	solicitud.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
	solicitud.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

	if (ipServidorS3.size() > 0) {
		url.setHost(ipServidorS3);
	}
	url.setPort(puertoServidorS3);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Download");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	solicitud.setRawHeader("Authorization", autorizacion.toLocal8Bit());
	solicitud.setRawHeader("Host", nombreDNSServidorS3.toLocal8Bit());

	_archivoAvatarTodus.setFileName(_rutaAvatarTodus);
	_archivoAvatarTodus.open(QIODevice::WriteOnly);

	_respuestaAvatarTodus = _administradorAccesoRedAvatarTodus->get(solicitud);
	connect(_respuestaAvatarTodus, &QIODevice::readyRead, this, &VentanaPrincipal::eventoRecepcionDatosAvatarTodus);
	connect(_respuestaAvatarTodus, &QNetworkReply::finished, this, &VentanaPrincipal::eventoDescargaTerminadaAvatarTodus);
}
