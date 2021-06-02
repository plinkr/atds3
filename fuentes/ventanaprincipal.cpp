#include "ventanaprincipal.hpp"
#include "http1.hpp"
#include "main.hpp"
#include "delegacioniconoestado.hpp"
#include "delegacionbarraprogreso.hpp"
#include "delegaciontamano.hpp"
#include "modelocategorias.hpp"
#include "modeloentradas.hpp"
#include "descarga.hpp"
#include "gestordescargas.hpp"
#include "ventanaagregardescarga.hpp"
#include "ventanaagregardescargasdesdearchivo.hpp"
#include "ventanaconfiguracion.hpp"
#include <QGuiApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include <QDockWidget>
#include <QStatusBar>
#include <QSqlTableModel>
#include <QListView>
#include <QProgressBar>
#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QSettings>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QMimeData>
#include <QUrl>
#include <QScreen>
#include <QTimer>
#include <QStandardPaths>
#include <QNetworkProxy>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMenu>
#include <QInputDialog>


VentanaPrincipal::VentanaPrincipal(QWidget *parent)
	: QMainWindow(parent), _categoriaActiva(0) {
	Q_INIT_RESOURCE(iconos);

	inicializarBaseDatos();

	setAcceptDrops(true);

	construirIU();

	_toDus = new toDus(this);
	connect(_toDus, &toDus::estadoCambiado, this, &VentanaPrincipal::actualizarEstadoTodus);
	QTimer::singleShot(0, _toDus, &toDus::iniciar);

	if (_configuracion.value("todus/fichaAcceso").toString().size() > 0 || _configuracion.value("todus/fichaSolicitud").toString().size() > 0 || _configuracion.value("todus/telefono").toString().size() > 0) {
		QTimer::singleShot(1000, _toDus, &toDus::iniciarSesion);
	} else {
		eventoConfiguracion();
	}

	_gestorDescargas = new GestorDescargas(this);
	_gestorDescargas->start();

	QList<QSharedPointer<ModeloEntradas>> modelos {_modeloCategoriaProgramas, _modeloCategoriaMusica, _modeloCategoriaVideos, _modeloCategoriaOtros};
	int estado = 0;
	for (auto modelo : modelos) {
		for (int f = 0; f < modelo->rowCount(); f++) {
			estado = modelo->data(modelo->index(f, 2)).toInt();
			if (estado == _ListadoEstados::EnEsperaIniciar || estado == _ListadoEstados::Iniciada) {
				modelo->setData(modelo->index(f, 2), _ListadoEstados::EnEsperaIniciar);
				_modeloCategoriaDescargas->setData(_modeloCategoriaDescargas->index(f, 2), _ListadoEstados::EnEsperaIniciar);
				QTimer::singleShot(1000, std::bind(&GestorDescargas::agregarDescarga, _gestorDescargas, f, modelo->data(modelo->index(f, 0)).toUInt(), modelo, _modeloCategoriaDescargas));
			}
		}
	}
}
VentanaPrincipal::~VentanaPrincipal() {
	// Emitir la señal de detención de la aplicación para que todos los hilos de descargas cesen su ejecución
	emit detenerEjecucion();
}

void VentanaPrincipal::closeEvent(QCloseEvent *evento) {
	// Oculta la ventana
	setVisible(false);

	if (_gestorDescargas->totalDescargas() > 0) {
		if (_configuracion.value("atds3/notificacionIconoSistema", false).toBool() == false) {
			_configuracion.setValue("atds3/notificacionIconoSistema", true);
			_iconoSistema->showMessage(_aplicacionTitulo, "ATDS3 continuará funcionando en segundo plano mientras se realizan las descargan. Puede volver a la ventana haciendo clic en el icono del sistema.");
		}
	} else {
		evento->accept();
		cerrarAplicacion();
	}

	// Ignora el evento
	evento->ignore();
}

void VentanaPrincipal::cerrarAplicacion() {
	setVisible(false);
	_iconoSistema->hide();

	_modeloCategoriaDescargas->submitAll();
	sincronizarBaseDatos();

	// Detiene adecuadamente todas las descargas activas
	_gestorDescargas->requestInterruption();
	_gestorDescargas->wait();

	// Desconecta la sesión toDus
	disconnect(_toDus, &toDus::estadoCambiado, this, &VentanaPrincipal::actualizarEstadoTodus);

	// Guarda la geometría de la ventana en las configuraciones
	_configuracion.setValue("atds3/geometria", saveGeometry());

	qApp->exit();
}

void VentanaPrincipal::dragEnterEvent(QDragEnterEvent *evento) {
	const QMimeData *mime = evento->mimeData();
	QString extension;

	if (mime->hasUrls()) {
		QList<QUrl> urls = mime->urls();

		for (const auto &url : urls) {
			extension = url.toLocalFile().toLower().right(4);
			if (extension == ".txt" || extension == ".s3p") {
				_listadoArchivosSoltar = urls;
				evento->acceptProposedAction();
				return;
			}
		}
	}
}

void VentanaPrincipal::dragMoveEvent(QDragMoveEvent *evento) {
	QString extension;

	for (const auto &url : _listadoArchivosSoltar) {
		extension = url.toLocalFile().toLower().right(4);
		if (extension == ".txt" || extension == ".s3p") {
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
	QString extension;

	/**
	 * Construye la ventana 'Agregar descargas desde archivo'
	 */
	_ventanaAgregarDescargasDesdeArchivo = new VentanaAgregarDescargasDesdeArchivos(this);
	_ventanaAgregarDescargasDesdeArchivo->setModal(true);
	connect(_ventanaAgregarDescargasDesdeArchivo, &VentanaAgregarDescargasDesdeArchivos::accepted, this, &VentanaPrincipal::agregarDescargasDesdeArchivo);

	for (const auto &url : _listadoArchivosSoltar) {
		extension = url.toLocalFile().toLower().right(4);
		if (extension == ".txt" || extension == ".s3p") {
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
	QString rutaDescarga = _configuracion.value("descargas/ruta").toString();
	QSqlQuery solicitudSQL;
	QString instruccionSQL;
	QTextStream flujo(&instruccionSQL);

	setCursor(Qt::WaitCursor);

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

	flujo << "INSERT INTO entradas (categoria, estado, enlace, nombre, ruta) VALUES (" << datos.categoria << ", " << (datos.iniciar == true ? _ListadoEstados::EnEsperaIniciar : _ListadoEstados::Pausada) << ", \"" << datos.enlace << "\", \"" << datos.nombre << "\", \"" << rutaDescarga << "\") RETURNING id";
	solicitudSQL.exec(instruccionSQL);
	solicitudSQL.next();

	modelo->select();
	_modeloCategoriaDescargas->select();

	if (datos.iniciar == true) {
		_gestorDescargas->agregarDescarga(modelo->rowCount() - 1, solicitudSQL.record().value(0).toUInt(), modelo, _modeloCategoriaDescargas);
	}

	unsetCursor();
}

/**
 * Agrega las descargas procesadas desde el archivo seleccionado por el usuario en la ventana 'Agrega descargas desde archivo'
 */
void VentanaPrincipal::agregarDescargasDesdeArchivo() {
	QVector<_NuevaDescarga> listadoDescargas = _ventanaAgregarDescargasDesdeArchivo->obtenerDatosDescargas();
	QSharedPointer<ModeloEntradas> modelo;
	QString rutaDescarga = _configuracion.value("descargas/ruta").toString();
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "INSERT INTO entradas (categoria, estado, enlace, nombre, ruta) VALUES";
	QTextStream flujo(&instruccionSQL);
	int filaModelo = 0;

	setCursor(Qt::WaitCursor);

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
		nuevaDescarga.nombre.replace("\"", "\\\"");
		flujo << " (" << QString::number(nuevaDescarga.categoria) << ", " << (nuevaDescarga.iniciar == true ? _ListadoEstados::EnEsperaIniciar : _ListadoEstados::Pausada) << ", \"" << nuevaDescarga.enlace << "\", \"" << nuevaDescarga.nombre << "\", \"" << rutaDescarga << "\"),";
	}

	instruccionSQL.chop(1);
	solicitudSQL.exec(instruccionSQL);
	modelo->select();
	_modeloCategoriaDescargas->select();

	for (int f = 0; f < listadoDescargas.size(); f++) {
		if (listadoDescargas[f].iniciar == true) {
			filaModelo = modelo->rowCount() - listadoDescargas.size() + f;
			_gestorDescargas->agregarDescarga(filaModelo, modelo->data(modelo->index(filaModelo, 0)).toUInt(), modelo, _modeloCategoriaDescargas);
			qApp->processEvents();
		}
	}

	unsetCursor();
}

/**
 * @brief Procesa las configuraciones guardadas y ejecuta las acciones adecuadas
 */
void VentanaPrincipal::procesarCambiosConfiguracion() {
	bool reconexionRequerida = false;

	if (_configuracionTelefono != _configuracion.value("todus/telefono").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionFichaAcceso != _configuracion.value("todus/fichaAcceso").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorAutentificacion != _configuracion.value("avanzadas/ipServidorAutentificacion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorAutentificacion != _configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorAutentificacion != _configuracion.value("avanzadas/nombreDNSServidorAutentificacion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorSesion != _configuracion.value("avanzadas/ipServidorSesion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorSesion != _configuracion.value("avanzadas/puertoServidorSesion", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorSesion != _configuracion.value("avanzadas/nombreDNSServidorSesion").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionIpServidorS3 != _configuracion.value("avanzadas/ipServidorS3").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorS3 != _configuracion.value("avanzadas/puertoServidorS3", 443).toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionNombreDNSServidorS3 != _configuracion.value("avanzadas/nombreDNSServidorS3").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionTipoProxy != _configuracion.value("proxy/tipo").toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionServidorProxy != _configuracion.value("proxy/servidor", "").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionPuertoServidorProxy != _configuracion.value("proxy/puerto").toInt()) {
		reconexionRequerida = true;
	}
	if (_configuracionUsuarioServidorProxy != _configuracion.value("proxy/usuario", "").toString()) {
		reconexionRequerida = true;
	}
	if (_configuracionContrasenaServidorProxy != _configuracion.value("proxy/contrasena", "").toString()) {
		reconexionRequerida = true;
	}

	if (reconexionRequerida == true) {
		QTimer::singleShot(0, _toDus, &toDus::reconectar);
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
	_ventanaAgregarDescargasDesdeArchivo->show();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar descarga'
 */
void VentanaPrincipal::eventoEliminarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;
	unsigned int id = 0;
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "DELETE FROM entradas WHERE (";
	QTextStream flujo(&instruccionSQL);

	setCursor(Qt::WaitCursor);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
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
			id = modelo->data(modelo->index(i.row(), 0)).toUInt();
			_gestorDescargas->detenerDescarga(id);
			flujo << "id = " << QString::number(id) << " OR ";
			qApp->processEvents();
		}
		instruccionSQL.chop(4);
		flujo << ")";
		solicitudSQL.exec(instruccionSQL);
		modelo->select();
	}

	unsetCursor();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Eliminar todas las descargas'
 */
void VentanaPrincipal::eventoEliminarTodasDescargas() {
	QSharedPointer<ModeloEntradas> modelo;
	QSqlQuery solicitudSQL;

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
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

	setCursor(Qt::WaitCursor);

	_gestorDescargas->_deteniendoDescargas = true;
	for (int f = 0; f < modelo->rowCount(); f++) {
		_gestorDescargas->detenerDescarga(modelo->data(modelo->index(f, 0)).toUInt());
		qApp->processEvents();
	}
	_gestorDescargas->_deteniendoDescargas = false;

	solicitudSQL.exec("DELETE FROM entradas WHERE (" + modelo->filter() + ")");
	modelo->select();

	unsetCursor();
}

void VentanaPrincipal::eventoSubir() {
	bool ok = false;
	QString tamano = QInputDialog::getText(nullptr, "Solicitar enlace de subida", "Tamaño del archivo:", QLineEdit::Normal, "", &ok);

	if (ok == true) {
		_toDus->solicitarEnlaceFirmadoSubida(tamano);
	}
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar descarga'
 */
void VentanaPrincipal::eventoIniciarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;
	unsigned int id = 0;
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::EnEsperaIniciar) + " WHERE (";
	QTextStream flujo(&instruccionSQL);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
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

	setCursor(Qt::WaitCursor);

	if (_listadoDescargas->selectionModel()->selectedRows().isEmpty() == false) {
		for (const auto &i : _listadoDescargas->selectionModel()->selectedRows()) {
			if (modelo->data(modelo->index(i.row(), 2)).toInt() == _ListadoEstados::Pausada) {
				id = modelo->data(modelo->index(i.row(), 0)).toUInt();
				flujo << "id = " << QString::number(id) << " OR ";
				modelo->setData(modelo->index(i.row(), 2), _ListadoEstados::EnEsperaIniciar);
				_gestorDescargas->agregarDescarga(i.row(), id, obtenerModeloDesdeCategoria(modelo->data(modelo->index(i.row(), 1)).toInt()), _modeloCategoriaDescargas);
				qApp->processEvents();
			}
		}
	}

	instruccionSQL.chop(4);
	flujo << ")";
	solicitudSQL.exec(instruccionSQL);

	unsetCursor();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Pausar descarga'
 */
void VentanaPrincipal::eventoPausarDescarga() {
	QSharedPointer<ModeloEntradas> modelo;
	unsigned int id = 0;
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Pausada) + " WHERE (";
	QTextStream flujo(&instruccionSQL);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
			break;
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

	setCursor(Qt::WaitCursor);

	if (_listadoDescargas->selectionModel()->selectedRows().isEmpty() == false) {
		for (const auto &i : _listadoDescargas->selectionModel()->selectedRows()) {
			if (modelo->data(modelo->index(i.row(), 2)).toInt() == _ListadoEstados::EnEsperaIniciar || modelo->data(modelo->index(i.row(), 2)).toInt() == _ListadoEstados::Iniciada) {
				id = modelo->data(modelo->index(i.row(), 0)).toUInt();
				flujo << "id = " << QString::number(id) << " OR ";
				modelo->setData(modelo->index(i.row(), 2), _ListadoEstados::Pausada);
				_gestorDescargas->detenerDescarga(id);
				qApp->processEvents();
			}
		}
	}

	instruccionSQL.chop(4);
	flujo << ")";
	solicitudSQL.exec(instruccionSQL);

	unsetCursor();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Iniciar todas las descargas'
 */
void VentanaPrincipal::eventoIniciarTodasDescargas() {
	QSharedPointer<ModeloEntradas> modelo;
	unsigned int id = 0;
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::EnEsperaIniciar) + " WHERE (";
	QTextStream flujo(&instruccionSQL);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
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

	setCursor(Qt::WaitCursor);

	for (int f = 0; f < modelo->rowCount(); f++) {
		if (modelo->data(modelo->index(f, 2)).toInt() == _ListadoEstados::Pausada) {
			id = modelo->data(modelo->index(f, 0)).toUInt();
			flujo << "id = " << QString::number(id) << " OR ";
			modelo->setData(modelo->index(f, 2), _ListadoEstados::EnEsperaIniciar);
			_gestorDescargas->agregarDescarga(f, id, obtenerModeloDesdeCategoria(modelo->data(modelo->index(f, 1)).toInt()), _modeloCategoriaDescargas);
			qApp->processEvents();
		}
	}

	instruccionSQL.chop(4);
	flujo << ")";
	solicitudSQL.exec(instruccionSQL);

	unsetCursor();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Pausar todas las descargas'
 */
void VentanaPrincipal::eventoPausarTodasDescargas() {
	QSharedPointer<ModeloEntradas> modelo;
	unsigned int id = 0;
	QSqlQuery solicitudSQL;
	QString instruccionSQL = "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Pausada) + " WHERE (";
	QTextStream flujo(&instruccionSQL);

	switch (_categoriaActiva) {
		case 0x01:
			modelo = _modeloCategoriaDescargas;
			break;
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

	setCursor(Qt::WaitCursor);

	for (int f = 0; f < modelo->rowCount(); f++) {
		if (modelo->data(modelo->index(f, 2)).toInt() == _ListadoEstados::EnEsperaIniciar || modelo->data(modelo->index(f, 2)).toInt() == _ListadoEstados::Iniciada) {
			id = modelo->data(modelo->index(f, 0)).toUInt();
			flujo << "id = " << QString::number(id) << " OR ";
			modelo->setData(modelo->index(f, 2), _ListadoEstados::Pausada);
			_gestorDescargas->detenerDescarga(id);
			qApp->processEvents();
		}
	}

	instruccionSQL.chop(4);
	flujo << ")";
	solicitudSQL.exec(instruccionSQL);

	unsetCursor();
}

/**
 * @brief Evento que se dispara cuando se hace clic en el botón 'Configuración'
 */
void VentanaPrincipal::eventoConfiguracion() {
	_configuracionTelefono = _configuracion.value("todus/telefono").toString();
	_configuracionFichaAcceso = _configuracion.value("todus/fichaAcceso").toString();
	_configuracionIpServidorAutentificacion = _configuracion.value("avanzadas/ipServidorAutentificacion").toString();
	_configuracionPuertoServidorAutentificacion = _configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	_configuracionNombreDNSServidorAutentificacion = _configuracion.value("avanzadas/nombreDNSServidorAutentificacion").toString();
	_configuracionIpServidorSesion = _configuracion.value("avanzadas/ipServidorSesion").toString();
	_configuracionPuertoServidorSesion = _configuracion.value("avanzadas/puertoServidorSesion", 443).toInt();
	_configuracionNombreDNSServidorSesion = _configuracion.value("avanzadas/nombreDNSServidorSesion").toString();
	_configuracionIpServidorS3 = _configuracion.value("avanzadas/ipServidorS3").toString();
	_configuracionPuertoServidorS3 = _configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	_configuracionNombreDNSServidorS3 = _configuracion.value("avanzadas/nombreDNSServidorS3").toString();
	_configuracionTipoProxy = _configuracion.value("proxy/tipo").toInt();
	_configuracionServidorProxy = _configuracion.value("proxy/servidor").toString();
	_configuracionPuertoServidorProxy = _configuracion.value("proxy/puerto").toInt();
	_configuracionUsuarioServidorProxy = _configuracion.value("proxy/usuario").toString();
	_configuracionContrasenaServidorProxy = _configuracion.value("proxy/contrasena").toString();

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
	QSharedPointer<ModeloEntradas> modelo;

	_modeloCategoriaDescargas->submitAll();

	_configuracion.setValue("atds3/ultimaCategoria", indice.row());

	switch (indice.row()) {
		case 0:
			modelo = _modeloCategoriaDescargas;
			break;
		case 1:
			modelo = _modeloCategoriaFinalizadas;
			break;
		case 2:
			modelo = _modeloCategoriaProgramas;
			break;
		case 3:
			modelo = _modeloCategoriaMusica;
			break;
		case 4:
			modelo = _modeloCategoriaVideos;
			break;
		case 5:
			modelo = _modeloCategoriaOtros;
			break;
	}

	modelo->select();
	_listadoDescargas->setModel(modelo.get());

	_categoriaActiva = indice.row() + 1;

	if (indice.row() == 1) {
		_listadoDescargas->hideColumn(7);
		_listadoDescargas->hideColumn(8);
		_listadoDescargas->hideColumn(9);
	} else {
		_listadoDescargas->showColumn(7);
		_listadoDescargas->showColumn(8);
		_listadoDescargas->showColumn(9);
	}

	//emit categoriaSeleccionada(modeloActivo);
}

/**
 * @brief Evento que se dispara cuando se finaliza la descarga del avatar
 */
void VentanaPrincipal::eventoDescargaTerminadaAvatarTodus() {
	if (_httpAvatar->codigoHTTP() == 200 && isVisible() == true) {
		QPixmap avatar;

		if (avatar.loadFromData(_buferDescargaAvatar) == true) {
			_buferDescargaAvatar.clear();
			_avatarTodus->setPixmap(avatar);
		}
	}
}

/**
 * @brief Actualiza la etiqueta que refleja el estado de la sesión toDus
 * @param Nuevo estado
 */
void VentanaPrincipal::actualizarEstadoTodus(toDus::Estado estado) {
	if (_estadoSesionTodus.isNull() == true) {
		return;
	}

	switch (estado) {
		case toDus::Estado::Desconectado:
			_estadoSesionTodus->setText("Desconectado.");
			_avatarTodus->unsetCursor();
			_avatarTodus->setToolTip("");
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
		{
			QString nombre = _configuracion.value("todus/nombre", "").toString();
			QString biografia = _configuracion.value("todus/biografia", "").toString();

			if (nombre.size() > 0) {
				_estadoSesionTodus->setText("<b>" + nombre + "</b><br/>Listo para usar.");
			} else {
				_estadoSesionTodus->setText("Listo para usar.");
			}
			_estadoSesionTodus->setToolTip(biografia);
/*
			_avatarTodus->setCursor(Qt::PointingHandCursor);
			_avatarTodus->setToolTip("Haga clic aquí para cambiar el avatar.");
*/
			actualizarAvatar();
			break;
		}
		case toDus::Estado::Desconectando:
			_estadoSesionTodus->setText("Desconectando...");
			break;
	}
}

void VentanaPrincipal::eventoIconoSistemaActivado(QSystemTrayIcon::ActivationReason razon) {
	switch(razon) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		case QSystemTrayIcon::MiddleClick:
			setVisible(!isVisible());
			break;
		default:
			break;
	}
}

/**
 * @brief Inicializa la base de datos en memoria
 */
void VentanaPrincipal::inicializarBaseDatos() {
	QSqlDatabase bdArchivo = QSqlDatabase::addDatabase("QSQLITE", "ATDS3");
	QSqlDatabase bd = QSqlDatabase::addDatabase("QSQLITE");

	bd.setDatabaseName(":memory:");
	bd.open();

	/**
	 * Crea las estructuras de las tablas
	 */
	QSqlQuery solicitudSQL;
	solicitudSQL.exec("CREATE TABLE IF NOT EXISTS entradas (id INTEGER PRIMARY KEY AUTOINCREMENT, categoria INTEGER NOT NULL, estado INTEGER NOT NULL, enlace TEXT NOT NULL, nombre TEXT NOT NULL, ruta TEXT NOT NULL, bytesTotal INTEGER DEFAULT 0, bytesTransferidos INTEGER DEFAULT 0, completado INTEGER DEFAULT 0, velocidad INTEGER DEFAULT 0)");

	bdArchivo.setDatabaseName(_rutaBaseDatos);
	if (bdArchivo.open() == true) {
		QSqlQuery solicitudSQLArchivo(bdArchivo);

		/**
		 * Crea las estructuras de las tablas
		 */
		solicitudSQLArchivo.exec("CREATE TABLE IF NOT EXISTS entradas (id INTEGER PRIMARY KEY AUTOINCREMENT, categoria INTEGER NOT NULL, estado INTEGER NOT NULL, enlace TEXT NOT NULL, nombre TEXT NOT NULL, ruta TEXT NOT NULL, bytesTotal INTEGER DEFAULT 0, bytesTransferidos INTEGER DEFAULT 0, completado INTEGER DEFAULT 0, velocidad INTEGER DEFAULT 0)");

		bd.transaction();
		solicitudSQLArchivo.exec("SELECT * FROM entradas");
		while (solicitudSQLArchivo.next() == true) {
			solicitudSQL.exec("INSERT INTO entradas VALUES (" + solicitudSQLArchivo.value("id").toString() + ", " + solicitudSQLArchivo.value("categoria").toString() + ", " + solicitudSQLArchivo.value("estado").toString() + ", \"" + solicitudSQLArchivo.value("enlace").toString() + "\", \"" + solicitudSQLArchivo.value("nombre").toString() + "\", \"" + solicitudSQLArchivo.value("ruta").toString() + "\", " + solicitudSQLArchivo.value("bytesTotal").toString() + ", " + solicitudSQLArchivo.value("bytesTransferidos").toString() + ", " + solicitudSQLArchivo.value("completado").toString() + ", " + solicitudSQLArchivo.value("velocidad").toString() + ")");
		}
		bd.commit();
		bdArchivo.close();
	}
}

/**
 * @brief Sincroniza la base de datos física
 */
void VentanaPrincipal::sincronizarBaseDatos() {
	QSqlDatabase bdArchivo = QSqlDatabase::database("ATDS3");
	QSqlDatabase bd = QSqlDatabase::database();
	QSqlQuery solicitudSQLArchivo(bdArchivo);
	QSqlQuery solicitudSQL;

	bdArchivo.setDatabaseName(_rutaBaseDatos);
	if (bdArchivo.open() == true) {
		bdArchivo.transaction();
		solicitudSQLArchivo.exec("DELETE FROM entradas");
		solicitudSQL.exec("SELECT * FROM entradas");
		while (solicitudSQL.next() == true) {
			solicitudSQLArchivo.exec("INSERT INTO entradas VALUES (" + solicitudSQL.value("id").toString() + ", " + solicitudSQL.value("categoria").toString() + ", " + solicitudSQL.value("estado").toString() + ", \"" + solicitudSQL.value("enlace").toString() + "\", \"" + solicitudSQL.value("nombre").toString() + "\", \"" + solicitudSQL.value("ruta").toString() + "\", " + solicitudSQL.value("bytesTotal").toString() + ", " + solicitudSQL.value("bytesTransferidos").toString() + ", " + solicitudSQL.value("completado").toString() + ", " + solicitudSQL.value("velocidad").toString() + ")");
		}
		bdArchivo.commit();
		bdArchivo.close();
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
	connect(accionConfiguracion, &QAction::triggered, this, &VentanaPrincipal::eventoConfiguracion);
	barraHerramientas->addAction(accionConfiguracion);

	QAction *accionTerminar = new QAction();
	accionTerminar->setIcon(QIcon(":/iconos/terminar.svg"));
	accionTerminar->setText("Terminar");
	accionTerminar->setToolTip("Termina la ejecución de la aplicación");
	accionTerminar->setStatusTip("Termina la ejecución de la aplicación");
	accionTerminar->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
	connect(accionTerminar, &QAction::triggered, this, &VentanaPrincipal::cerrarAplicacion);
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

	QMenu *menuIconoSistema = new QMenu(this);

	menuIconoSistema->addAction(accionAgregarDescarga);
	menuIconoSistema->addAction(accionAgregarDescargasDesdeArchivo);
	menuIconoSistema->addSeparator();
	menuIconoSistema->addAction(accionConfiguracion);
	menuIconoSistema->addSeparator();
	menuIconoSistema->addAction(accionTerminar);

	_iconoSistema = new QSystemTrayIcon(this);
	_iconoSistema->setContextMenu(menuIconoSistema);
	_iconoSistema->setIcon(QIcon(":/iconos/atds3.png"));
	_iconoSistema->setToolTip(_aplicacionTitulo);
	connect(_iconoSistema, &QSystemTrayIcon::activated, this, &VentanaPrincipal::eventoIconoSistemaActivado);
	_iconoSistema->show();
}

/**
 * @brief Construye las categorías
 * @return Puntero al elemento del listado de categorías
 */
QListView *VentanaPrincipal::construirListadoCategorias() {
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
	_listadoCategorias->setWrapping(true);
	_listadoCategorias->setViewMode(QListView::IconMode);
	connect(_listadoCategorias, &QListView::activated, this, &VentanaPrincipal::eventoCategoriaSeleccionada);
	connect(_listadoCategorias, &QListView::pressed, this, &VentanaPrincipal::eventoCategoriaSeleccionada);

	return _listadoCategorias;
}

/**
 * @brief Construye el elemento del listado de descargas
 * @return Puntero al elemento del listado de descargas
 */
QTreeView *VentanaPrincipal::construirListadoDescargas() {
	/**
	 * Modelo del listado de la categoría 'Descargando'
	 */
	_modeloCategoriaDescargas = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaDescargas->setFilter(QString("categoria != %1").arg(_ListadoCategorias::Subidas));
	_modeloCategoriaDescargas->select();

	/**
	 * Modelo del listado de la categoría 'Finalizadas'
	 */
	_modeloCategoriaFinalizadas = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaFinalizadas->setFilter(QString("estado = %1").arg(_ListadoEstados::Finalizada));
	_modeloCategoriaFinalizadas->select();

	/**
	 * Modelo del listado de la categoría 'Programas'
	 */
	_modeloCategoriaProgramas = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaProgramas->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Programas));
	_modeloCategoriaProgramas->select();

	/**
	 * Modelo del listado de la categoría 'Música'
	 */
	_modeloCategoriaMusica = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaMusica->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Musica));
	_modeloCategoriaMusica->select();

	/**
	 * Modelo del listado de la categoría 'Videos'
	 */
	_modeloCategoriaVideos = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaVideos->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Videos));
	_modeloCategoriaVideos->select();

	/**
	 * Modelo del listado de la categoría 'Otros'
	 */
	_modeloCategoriaOtros = QSharedPointer<ModeloEntradas>(new ModeloEntradas(this));
	_modeloCategoriaOtros->setFilter(QString("categoria = %1").arg(_ListadoCategorias::Otros));
	_modeloCategoriaOtros->select();

	_listadoDescargas = new QTreeView();
	_listadoDescargas->setAllColumnsShowFocus(false);
	_listadoDescargas->setAlternatingRowColors(true);
	_listadoDescargas->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_listadoDescargas->setItemsExpandable(false);
	_listadoDescargas->setModel(_modeloCategoriaDescargas.get());
	_listadoDescargas->setUniformRowHeights(true);
	_listadoDescargas->setRootIsDecorated(false);
	_listadoDescargas->setSelectionBehavior(QAbstractItemView::SelectRows);
	_listadoDescargas->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_listadoDescargas->setSortingEnabled(false);
	_listadoDescargas->setStyleSheet("QTreeView::item { height: 28px; }");
	_listadoDescargas->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_listadoDescargas->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
	_listadoDescargas->header()->setStretchLastSection(true);

	_listadoDescargas->hideColumn(0);
	_listadoDescargas->hideColumn(1);
	_listadoDescargas->hideColumn(3);
	_listadoDescargas->hideColumn(5);

	_elementoIconoEstado = new DelegacionIconoEstado();
	_elementoBarraProgreso = new DelegacionBarraProgreso();
	_elementoTamano = new DelegacionTamano();
	_elementoVelocidad = new DelegacionTamano(true);
	_listadoDescargas->setItemDelegateForColumn(2, _elementoIconoEstado);
	_listadoDescargas->setItemDelegateForColumn(6, _elementoTamano);
	_listadoDescargas->setItemDelegateForColumn(7, _elementoTamano);
	_listadoDescargas->setItemDelegateForColumn(8, _elementoBarraProgreso);
	_listadoDescargas->setItemDelegateForColumn(9, _elementoVelocidad);

	_categoriaActiva = 0x01;

	return _listadoDescargas;
}

/**
 * @brief Construye la interfaz de usuario
 */
void VentanaPrincipal::construirIU() {
	setMinimumSize(QSize(705, 430));
	setWindowIcon(QIcon(":/iconos/atds3.svg"));

	// Restaura la geometría de la ventana previamente guardado
	if (_configuracion.value("atds3/geometria").toByteArray().size() > 0) {
		restoreGeometry(_configuracion.value("atds3/geometria").toByteArray());
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
	 * Construir elemento central: listado de descargas
	 */
	setCentralWidget(construirListadoDescargas());
	//setContentsMargins(QMargins(5, 5, 5, 5));

	// Restaura la última categoría seleccionada
	QModelIndex indiceSeleccionado = _modeloListadoCategorias->index(_configuracion.value("atds3/ultimaCategoria", 0).toInt(), 0);
	_listadoCategorias->setCurrentIndex(indiceSeleccionado);
	eventoCategoriaSeleccionada(indiceSeleccionado);

	statusBar()->showMessage("Listo");
	statusBar()->addPermanentWidget(new QLabel("v" + _aplicacionVersion));
}

/**
 * @brief Descarga el avatar del usuario desde la red toDus y lo muestra en la barra de herramientas
 */
void VentanaPrincipal::actualizarAvatar() {
	QString enlaceAvatar = _configuracion.value("todus/enlaceAvatar").toString();

	if (enlaceAvatar.size() == 0) {
		return;
	}

	QString ipServidorS3 = _configuracion.value("avanzadas/ipServidorS3", "").toString();
	QString nombreDNSServidorS3 = _configuracion.value("avanzadas/nombreDNSServidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = _configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	QString autorizacion = "Bearer " + _configuracion.value("todus/fichaAcceso").toString();

	enlaceAvatar.remove(0, 8 + nombreDNSServidorS3.size());

	_httpAvatar = new HTTP1(this);

	_httpAvatar->establecerMetodo(HTTP1::Metodo::OBTENER);
	if (ipServidorS3.size() > 0) {
		_httpAvatar->establecerAnfitrion(ipServidorS3);
	} else {
		_httpAvatar->establecerAnfitrion(nombreDNSServidorS3);
	}
	_httpAvatar->establecerPuerto(puertoServidorS3);
	_httpAvatar->establecerRuta(enlaceAvatar);

	_buferDescargaAvatar.clear();
	_httpAvatar->establecerDestinoDescarga(&_buferDescargaAvatar);

	_httpAvatar->agregarCabeceraSolicitud("Accept", "image/*");
	_httpAvatar->agregarCabeceraSolicitud("Authorization", autorizacion);
	_httpAvatar->agregarCabeceraSolicitud("Content-Type", "application/octet-stream");
	_httpAvatar->agregarCabeceraSolicitud("Host", nombreDNSServidorS3);
	_httpAvatar->agregarCabeceraSolicitud("User-Agent", _configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Download");

	connect(_httpAvatar, &HTTP1::finalizado, this, &VentanaPrincipal::eventoDescargaTerminadaAvatarTodus);

	_httpAvatar->ejecutar();
}

QSharedPointer<ModeloEntradas> VentanaPrincipal::obtenerModeloDesdeCategoria(int categoria) {
	QSharedPointer<ModeloEntradas> modelo;

	switch (categoria) {
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
			break;
	}

	return modelo;
}
