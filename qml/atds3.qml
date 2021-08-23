import Qt.labs.platform 1.0
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtMultimedia 5.12
import cu.atds3.librerias 1.6
import "qrc:/qml/configuracion"
import "qrc:/qml/principal"


ApplicationWindow {
	property bool orientacionHorizontal: width > 649
	property bool sistemaOperativoAndroid: Qt.platform.os.toLowerCase() === "android"
	property string androidRutaPrimario: ""
	property string androidRutaExterno: ""
	property color colorPrimario: Material.color(colorTemaPrimario(), Material.Shade800)
	property int colorAcento: colorTemaPrimario()
	property string estadoListadoCategorias: configuraciones.valor("atds3/estadoListadoCategorias", "expandido")
	property int categoriaActual: parseInt(configuraciones.valor("atds3/ultimaCategoriaSeleccionada", 0))
	property string nombreCategoriaActual: ""
	property string iconoCategoriaActual: ""
	property string estadoDisponibilidadTodus: "perdido"
	property string rutaSistemaDescargas: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
	property alias modeloIconoCategorias: modeloIconoCategorias
	property alias modeloCategorias: modeloCategorias
	property alias modeloPaquetes: modeloPaquetes
	property alias pantallaConfiguracion: pantallaConfiguracion
	property alias pantallaPrincipal: pantallaPrincipal
	property alias pantallaCodigoVerificacion: pantallaCodigoVerificacion
	property alias pantallaPublicarContenido: pantallaPublicarContenido
	property alias vistaApilable: vistaApilable

	id: ventanaPrincipal
	font.pointSize: tamanoFuente
	height: 620
    width: 1080
	visible: true

	Configuraciones {
		id: configuraciones
	}

	Utiles {
		id: utiles
	}

	function mostrarOcultarVentana() {
		if (ventanaPrincipal.visible === true) {
			hide()
		} else {
			show()
			raise()
			requestActivate()
		}
	}

	function colorTemaPrimario() {
		let color;

		switch (parseInt(configuraciones.valor("atds3/temaColores", 4))) {
			case 0: // Amarillo
				color = Material.Yellow;
				break;
			case 1: // Ámber
				color = Material.Amber;
				break;
			case 2: // Azul
				color = Material.Blue;
				break;
			case 3: // Azul claro
				color = Material.LightBlue;
				break;
			case 4: // Azul oscuro
				color = Material.Indigo;
				break;
			case 5: // Cian
				color = Material.Cyan;
				break;
			case 6: // Gris
				color = Material.Grey;
				break;
			case 7: // Gris azulado
				color = Material.BlueGrey;
				break;
			case 8: // Marrón
				color = Material.Brown;
				break;
			case 9: // Naranja
				color = Material.Orange;
				break;
			case 10: // Naranja profundo
				color = Material.DeepOrange;
				break;
			case 11: // Púrpura
				color = Material.Purple;
				break;
			case 12: // Púrpura oscuro
				color = Material.DeepPurple;
				break;
			case 13: // Rojo
				color = Material.Red;
				break;
			case 14: // Rosado
				color = Material.Pink;
				break;
			case 15: // Verde
				color = Material.Green;
				break;
			case 16: // Verde azulado
				color = Material.Teal;
				break;
			case 17: // Verde claro
				color = Material.LightGreen;
				break;
			case 18: // Verde lima
				color = Material.Lime;
				break;
		}

		return color;
	}

	function actualizarTemaColores() {
		colorPrimario = Material.color(colorTemaPrimario(), Material.Shade800)
		colorAcento = colorTemaPrimario()
	}

	function actualizarEstadoListadoCategorias(valor) {
		configuraciones.establecerValor("atds3/estadoListadoCategorias", valor)
		estadoListadoCategorias = valor
	}

	function visualizarTareaIniciada(idCategoria, idPaquete, filaPaquete, filaTarea) {
		if (modeloPaquetes.rowCount() > 0) {
			if (pantallaPrincipal.vistaPaquetes.listadoPaquetes.currentIndex !== -1) {
				let registroPaquete
				let fila

				for (fila = 0; fila < modeloPaquetes.rowCount(); fila++) {
					registroPaquete = modeloPaquetes.obtener(filaPaquete)

					if (registroPaquete.id === idPaquete) {
						if (pantallaPrincipal.vistaPaquetes.listadoPaquetes.currentItem !== null) {
							if (pantallaPrincipal.vistaPaquetes.listadoPaquetes.currentItem.idPaquete === idPaquete) {
								pantallaPrincipal.vistaPaquetes.listadoPaquetes.currentItem.listadoTareas.currentIndex = filaTarea
							}
						}

						break;
					}
				}
			}
		}
	}

	function mostrarPantallaCodigoVerificacion() {
		vistaApilable.push(pantallaCodigoVerificacion)
	}

	function mostrarPantallaFalloInicioSesion(error) {
		vistaApilable.push(pantallaFalloInicioSesion)
		vistaApilable.currentItem.mostrar(error);
	}

	Audio {
		id: reproductorNotificacion
		audioRole: Audio.NotificationRole
	}

	function reproducirNotificacion(archivo) {
		reproductorNotificacion.source = `qrc:/mp3/${configuraciones.valor("notificaciones/temaSonidos", "encantado")}/${archivo}`
		reproductorNotificacion.play()
	}

	function actualizarEstadoDisponibilidadTodus(estado) {
		estadoDisponibilidadTodus = estado
	}

	Material.primary: colorPrimario
	Material.accent: colorAcento

	Shortcut {
		sequence: "Ctrl+Q"
		onActivated: Qt.quit(0)
	}
	Shortcut {
		sequences: ["Esc", "Back"]
		enabled: (vistaApilable.depth > 1)
		onActivated: vistaApilable.pop()
	}

	SystemPalette { id: paletaSistema; colorGroup: SystemPalette.Active }

	ModeloCategorias {
		id: modeloCategorias
	}

	ModeloIconoCategorias {
		id: modeloIconoCategorias
	}

	ModeloPaquetes {
		id: modeloPaquetes
	}

	Component {
		id: pantallaConfiguracion

		Configuracion {}
	}

	Principal {
		id: pantallaPrincipal
	}

	Component {
		id: pantallaCodigoVerificacion

		CodigoVerificacion {}
	}

	Component {
		id: pantallaFalloInicioSesion

		FalloInicioSesion {}
	}

	Component {
		id: pantallaPublicarContenido

		PublicarContenido {}
	}

	StackView {
		id: vistaApilable
		anchors.fill: parent
		initialItem: pantallaPrincipal
	}

	Component.onCompleted: {
		let registroCategoria = modeloCategorias.obtener(categoriaActual)

		if (sistemaOperativoAndroid === true) {
			androidRutaPrimario = utiles.obtenerRutaSistema(0);
			androidRutaExterno = utiles.obtenerRutaSistema(1);

			if (configuraciones.valor("descargas/ruta", "") === "") {
				configuraciones.establecerValor("descargas/ruta", androidRutaPrimario);
			}
		}

		utiles.establecerObjetoModeloPaquetes(modeloPaquetes)
		utiles.iniciarMonitorizacionConexionTodus()
		utiles.crearBandejaIcono()

		modeloPaquetes.establecerObjetoUtiles(utiles)

		nombreCategoriaActual = registroCategoria.titulo
		iconoCategoriaActual = registroCategoria.icono

		modeloPaquetes.establecerFiltroCategoria(registroCategoria.id)
	}
}
