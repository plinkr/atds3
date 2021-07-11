import Qt.labs.platform 1.0
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import cu.atds3 1.0


Pane {
	property alias listadoPaquetes: listadoPaquetes
	property int anchoCabeceraEstado: 60
	property int anchoCabeceraTamano: 80
	property int anchoCabeceraTamanoTransferido: 100
	property int anchoCabeceraCompletado: 120
	property int anchoCabeceraVelocidad: 120

	function representarTamano(tamano) {
		const listadoUnidadesTamano = [ "B", "KiB", "MiB", "GiB", "TiB" ]
		let unidadTamano = 0;
		let tamanoAProcesar = tamano
		let representacion;

		while (tamanoAProcesar > 1024) {
			tamanoAProcesar /= 1024;
			unidadTamano++;
		}

		if (unidadTamano < 2) {
			representacion = `${parseInt(tamanoAProcesar)} ${listadoUnidadesTamano[unidadTamano]}`
		} else {
			if (parseFloat(tamanoAProcesar) - parseInt(tamanoAProcesar) > 0) {
				representacion = `${parseFloat(tamanoAProcesar).toPrecision(String(parseInt(tamanoAProcesar)).length + 2)} ${listadoUnidadesTamano[unidadTamano]}`
			} else {
				representacion = `${parseInt(tamanoAProcesar)} ${listadoUnidadesTamano[unidadTamano]}`
			}
		}

		return representacion
	}

	Layout.fillHeight: true
	Layout.fillWidth: true

	enum Tipos {
		Publicacion,
		Descarga
	}
	enum Formatos {
		S3,
		Clasico
	}
	enum Estados {
		Pausado,
		EnEsperaIniciar,
		Iniciado,
		Finalizado,
		Error
	}

	DropArea {
		anchors.fill: parent

		onDropped: {
			if (drop.hasUrls === true) {
				if (vistaCategorias.listadoCategorias.currentIndex !== 2) {
					if (vistaCategorias.listadoCategorias.currentIndex === 1) {
						pantallaPublicarContenido.mostrar()
						pantallaPublicarContenido.agregarArchivosAlListado(drop.urls)
					} else {
						modeloPaquetes.agregarDescargasDesdeArchivos(drop.urls)
					}
				}
			}
		}

		ListView {
			id: listadoPaquetes
			Accessible.role: Accessible.List
			Accessible.name: "Listado de paquetes y tareas"
			anchors.fill: parent
			boundsBehavior: Flickable.StopAtBounds
			clip: true
			currentIndex: -1
			highlightFollowsCurrentItem: true
			spacing: 5
			model: modeloPaquetes
			header: Item {
				height: 30
				width: parent.width

				RowLayout {
					anchors.fill: parent
					spacing: 8

					Label {
						Layout.preferredWidth: anchoCabeceraEstado
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: "Estado"
						verticalAlignment: Qt.AlignTop
					}

					Label {
						Layout.fillWidth: true
						font.bold: true
						text: "Nombre"
						verticalAlignment: Qt.AlignTop
					}

					Label {
						Layout.preferredWidth: anchoCabeceraTamano
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: "Tamaño"
						verticalAlignment: Qt.AlignTop
					}

					Label {
						Layout.preferredWidth: anchoCabeceraTamanoTransferido
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: "Transferido"
						verticalAlignment: Qt.AlignTop
					}

					Label {
						Layout.preferredWidth: anchoCabeceraCompletado
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: "Completado"
						verticalAlignment: Qt.AlignTop
					}

					Label {
						Layout.preferredWidth: anchoCabeceraVelocidad
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: "Velocidad"
						verticalAlignment: Qt.AlignTop
					}
				}
			}

			delegate: ItemDelegate {
				property int idPaquete: model.id
				property alias listadoTareas: listadoTareas

				Accessible.role: Accessible.ListItem
				Accessible.name: "Paquete " + (model.nombre !== undefined ? decodeURIComponent(model.nombre) : "") + ". Completado: " + (model.completado !== undefined ? model.completado : "0") + "%"
				clip: true
				focusPolicy: Qt.StrongFocus
				highlighted: ListView.isCurrentItem
				hoverEnabled: true
				height: ListView.isCurrentItem === false ? (tamanoIconos === 48 ? 40 : 32) : (tamanoIconos === 48 ? 40 : 32) + 4 + etiquetaTituloListadoTareas.height + 4 + listadoTareas.height
				width: ListView.view.width

				Behavior on height {
					NumberAnimation {
						duration: 200
					}
				}

				ColumnLayout {
					height: parent.height
					width: parent.width
					spacing: 4

					VistaTarea {}

					Label {
						id: etiquetaTituloListadoTareas
						Layout.leftMargin: 16
						font.bold: true
					}

					ListView {
						id: listadoTareas
						Layout.fillWidth: true
						Layout.minimumHeight: 40
						Layout.maximumHeight: tamanoIconos === 48 ? 200 : 160
						boundsBehavior: Flickable.StopAtBounds
						clip: true
						highlightFollowsCurrentItem: true
						model: ModeloTareas {
							id: modeloTareas
						}
						delegate: Item {
							clip: true
							height: tamanoIconos === 48 ? 40 : 32
							width: ListView.view.width

							ColumnLayout {
								height: parent.height
								width: parent.width
								spacing: 4

								VistaTarea {}
							}
						}

						ScrollIndicator.vertical: ScrollIndicator {}

						Component.onCompleted: {
							modeloPaquetes.establecerModeloTareas(idPaquete, modeloTareas);
							modeloTareas.establecerFiltroPaquete(idPaquete)
							listadoTareas.Layout.preferredHeight = modeloTareas.rowCount() * (tamanoIconos === 48 ? 40 : 32)
							etiquetaTituloListadoTareas.text = `Listado de tareas: ${modeloTareas.rowCount()} en total`
						}

						Component.onDestruction: {
							modeloPaquetes.eliminarModeloTareas(idPaquete)
						}
					}
				}

				onClicked: {
					if (index === listadoPaquetes.currentIndex) {
						listadoPaquetes.currentIndex = -1
					} else {
						listadoPaquetes.currentIndex = index
					}
				}

				onPressAndHold: {
					menuPaquete.x = (ventanaPrincipal.width / 2) - (menuPaquete.width / 2)
					menuPaquete.y = (ventanaPrincipal.height / 2) - (menuPaquete.height / 2)
					menuPaquete.open()
				}

				MouseArea {
					anchors.fill: parent
					acceptedButtons: Qt.RightButton

					onPressed: {
					   listadoPaquetes.currentIndex = index

						if (mouse.button === Qt.RightButton) {
						   menuPaquete.popup()
					   }
				   }
				}
			}

			ScrollIndicator.vertical: ScrollIndicator {}
		}
	}

	Menu {
		id: menuPaquete
		modal: true

		MenuItem {
			enabled: false
			font.bold: true
			height: tamanoIconos === 48 ? 48 : 38
			text: {
				if (listadoPaquetes.currentIndex === -1) {
					return ""
				}

				return decodeURIComponent(modeloPaquetes.obtener(listadoPaquetes.currentIndex).nombre)
			}
		}
		MenuSeparator {}
		MenuItem {
			height: tamanoIconos === 48 ? 48 : 38
			hoverEnabled: true
			icon.source: "qrc:/svg/play.svg"
			text: "Iniciar"
			onTriggered: modeloPaquetes.iniciar(listadoPaquetes.currentIndex)
		}
		MenuItem {
			height: tamanoIconos === 48 ? 48 : 38
			hoverEnabled: true
			icon.source: "qrc:/svg/pause.svg"
			text: "Pausar"
			onTriggered: modeloPaquetes.pausar(listadoPaquetes.currentIndex)
		}
		MenuSeparator {}
		MenuItem {
			height: tamanoIconos === 48 ? 48 : 38
			hoverEnabled: true
			icon.source: "qrc:/svg/minus-square.svg"
			text: "Eliminar"
			onTriggered: modeloPaquetes.eliminar(listadoPaquetes.currentIndex)
		}
		MenuSeparator {}
		MenuItem {
			height: tamanoIconos === 48 ? 48 : 38
			hoverEnabled: true
			icon.source: "qrc:/svg/folder-open.svg"
			text: "Abrir ubicación"
			onTriggered: {
				let registroPaquete = modeloPaquetes.obtener(listadoPaquetes.currentIndex)
				let registroCategoria = modeloCategorias.obtenerDesdeId(registroPaquete.categoria)

				if (registroPaquete.tipo === Paquetes.Tipos.Publicacion) {
					Qt.openUrlExternally(configuraciones.valor("descargas/ruta", StandardPaths.writableLocation(StandardPaths.DownloadLocation) + "/atds3") + "/Publicaciones")
				} else {
					Qt.openUrlExternally(configuraciones.valor("descargas/ruta", StandardPaths.writableLocation(StandardPaths.DownloadLocation) + "/atds3") + "/" + registroCategoria.titulo + "/" + decodeURIComponent(registroPaquete.nombre))
				}
			}
		}
	}
}
