import Qt.labs.platform 1.0
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Pane {
	property alias listadoCategorias: listadoCategorias
	property alias indicadorDisponibilidadTodus: indicadorDisponibilidadTodus

	id: panelCategorias
	Accessible.role: Accessible.List
	Accessible.name: "Listado de las categorías"
	Accessible.description: "Aquí se muestra el listado editable de las categorías en donde se almacenarán los paquetes de descargas y publicaciones"
	Layout.fillHeight: true
	Layout.preferredWidth: 300

	background: Rectangle {
		anchors.fill: parent
		color: Material.color(colorAcento, Material.Shade700)
	}

	ColumnLayout {
		anchors.fill: parent

		RowLayout {
			clip: true
			width: parent.width

			Label {
				Layout.leftMargin: 20
				enabled: visible
				font.bold: true
				text: "Categorías"
				visible: ventanaPrincipal.orientacionHorizontal === true ? panelCategorias.state === "expandido" : true
			}
			Label {
				Layout.fillWidth: true
			}
			Button {
				Accessible.role: Accessible.Button
				Accessible.name: "Agregar categoría"
				Accessible.description: "Haga clic aquí para mostrar la pantalla para agregar una categoría al listado"
				enabled: visible
				focusPolicy: Qt.StrongFocus
				flat: true
				hoverEnabled: true
				icon.height: tamanoIconos === 48 ? 24 : 16
				icon.width: tamanoIconos === 48 ? 24 : 16
				icon.source: "qrc:/svg/plus-square.svg"
				visible: ventanaPrincipal.orientacionHorizontal === true ? panelCategorias.state === "expandido" : true

				onClicked: {
					menuLateral.close()
					pantallaEditarCategoria.crear()
				}
			}
			RoundButton {
				id: botonExpandirContraer
				Accessible.role: Accessible.Button
				Accessible.name: "Expandir o contraer"
				Accessible.description: "Haga clic aquí para expandir o contraer el listado de las categorías"
				focusPolicy: Qt.StrongFocus
				flat: true
				hoverEnabled: true
				icon.height: tamanoIconos === 48 ? 24 : 16
				icon.width: tamanoIconos === 48 ? 24 : 16
				icon.source: "qrc:/svg/chevron-circle-left.svg"
				visible: orientacionHorizontal === true
				state: estadoListadoCategorias

				states: [
					State {
						name: "expandido"
						PropertyChanges {
							target: botonExpandirContraer
							rotation: 0
						}
					},
					State {
						name: "contraido"
						PropertyChanges {
							target: botonExpandirContraer
							rotation: 180
						}
					}
				]

				transitions: Transition {
					RotationAnimation {
						duration: 250
						direction: RotationAnimation.Clockwise
					}
				}

				onClicked: {
					actualizarEstadoListadoCategorias(estadoListadoCategorias === "expandido" ? "contraido" : "expandido")
				}
			}
		}

		ListView {
			id: listadoCategorias
			Accessible.role: Accessible.List
			Accessible.name: "Listado de categorías"
			Layout.fillHeight: true
			Layout.fillWidth: true
			boundsBehavior: Flickable.StopAtBounds
			currentIndex: parseInt(configuraciones.valor("atds3/ultimaCategoriaSeleccionada", 0))
			clip: true
			highlightFollowsCurrentItem: true
			model: modeloCategorias
			delegate: ItemDelegate {
				Accessible.role: Accessible.ListItem
				Accessible.name: `Categoría ${model.titulo}`
				ToolTip.text: estadoListadoCategorias === "expandido" ? "" : model.titulo
				ToolTip.visible: estadoListadoCategorias === "expandido" ? false : hovered
				clip: true
				display: AbstractButton.TextBesideIcon
				focusPolicy: Qt.StrongFocus
				font.bold: ListView.isCurrentItem
				highlighted: ListView.isCurrentItem
				hoverEnabled: true
				icon.color: {
					if ( ListView.isCurrentItem === true) {
						return Material.foreground
					} else {
						return Material.color(colorTemaPrimario(), Material.Shade100)
					}
				}
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: `qrc:/svg/${model.icono}`
				spacing: 20
				text: estadoListadoCategorias === "expandido" ? model.titulo : ""
				width: ListView.view.width

				Behavior on icon.color {
					ColorAnimation {
						duration: 100
					}
				}

				onPressed: {
					listadoCategorias.currentIndex = index

					if (menuLateral.opened === true) {
						menuLateral.close()
					}
				}

				onPressAndHold: {
					if (index !== 2) {
						menuCategoria.x = (ventanaPrincipal.width / 2) - (menuCategoria.width / 2)
						menuCategoria.y = (ventanaPrincipal.height / 2) - (menuCategoria.height / 2)
						menuCategoria.open()
					}
				}

				MouseArea {
					anchors.fill: parent
					acceptedButtons: Qt.RightButton

					onPressed: {
					   listadoCategorias.currentIndex = index

						if (mouse.button === Qt.RightButton) {
						   if (index !== 2) {
							   menuCategoria.popup()
						   }
					   }
				   }
				}
			}

			ScrollIndicator.vertical: ScrollIndicator {}

			onCurrentIndexChanged: {
				let registroCategoria = modeloCategorias.obtener(listadoCategorias.currentIndex)

				nombreCategoriaActual = registroCategoria.titulo
				iconoCategoriaActual = registroCategoria.icono
				vistaPaquetes.listadoPaquetes.currentIndex = -1
				modeloPaquetes.establecerFiltroCategoria(registroCategoria.id)
				configuraciones.establecerValor("atds3/ultimaCategoriaSeleccionada", listadoCategorias.currentIndex)
			}
		}
		RowLayout {
			Layout.alignment: Qt.AlignRight

			Rectangle {
				id: indicadorDisponibilidadTodus
				Layout.alignment: Qt.AlignCenter
				height: tamanoIconos === 48 ? 12 : 10
				width: tamanoIconos === 48 ? 12 : 10
				radius: 50
				state: "perdido"
				ToolTip.text: state === "perdido" ? "Conexión perdida con los servidores de toDus" : "Conexión disponible con los servidores de toDus"
				states: [
					State {
						name: "disponible"
						PropertyChanges {
							target: indicadorDisponibilidadTodus
							color: {
								let colorPrimario = colorTemaPrimario()

								if (colorPrimario === Material.Green) {
									return Material.color(Material.Yellow)
								} else {
									return Material.color(Material.Green)
								}
							}
						}
					},
					State {
						name: "perdido"
						PropertyChanges {
							target: indicadorDisponibilidadTodus
							color: Material.iconDisabledColor
						}
					}
				]

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true

					onHoveredChanged: indicadorDisponibilidadTodus.ToolTip.visible = containsMouse
				}
			}

			Label {
				Layout.alignment: Qt.AlignCenter
				font.pointSize: ventanaPrincipal.font.pointSize - 1
				text: `v${_aplicacionVersion}`
			}
		}
	}
	state: estadoListadoCategorias

	states: [
		State {
			name: "expandido"
			PropertyChanges {
				target: panelCategorias
				Layout.preferredWidth: 300
			}
		},
		State {
			name: "contraido"
			PropertyChanges {
				target: panelCategorias
				Layout.preferredWidth: ventanaPrincipal.orientacionHorizontal === true ? 60 + tamanoIconos : 300
			}
		}
	]

	Behavior on Layout.preferredWidth {
		NumberAnimation {
			duration: 250
		}
	}

	Menu {
		id: menuCategoria
		modal: true

		MenuItem {
			enabled: false
			font.bold: true
			height: tamanoIconos === 48 ? 48 : 38
			icon.source: `qrc:/svg/${modeloCategorias.obtener(listadoCategorias.currentIndex).icono}`
			text: modeloCategorias.obtener(listadoCategorias.currentIndex).titulo
		}
		MenuSeparator {}
		MenuItem {
			enabled: listadoCategorias.currentIndex > 2
			hoverEnabled: true
			height: tamanoIconos === 48 ? 48 : 38
			icon.source: "qrc:/svg/edit.svg"
			text: "Editar"
			onTriggered:{
				if (menuLateral.opened === true) {
					menuLateral.close()
				}

				pantallaEditarCategoria.editar(listadoCategorias.currentIndex)
			}
		}
		MenuItem {
			enabled: listadoCategorias.currentIndex > 2
			hoverEnabled: true
			height: tamanoIconos === 48 ? 48 : 38
			icon.source: "qrc:/svg/minus-square.svg"
			text: "Eliminar"
			onTriggered: {
				if (menuLateral.opened === true) {
					menuLateral.close()
				}

				modeloCategorias.eliminar(listadoCategorias.currentIndex)
			}
		}
		MenuSeparator {}
		MenuItem {
			hoverEnabled: true
			height: tamanoIconos === 48 ? 48 : 38
			icon.source: "qrc:/svg/folder-open.svg"
			text: "Abrir ubicación"
			onTriggered: {
				let registroCategoria = modeloCategorias.obtener(listadoCategorias.currentIndex)
				let ubicacion = configuraciones.valor("descargas/ruta", StandardPaths.writableLocation(StandardPaths.DownloadLocation) + "/atds3") + "/" + registroCategoria.titulo

				modeloPaquetes.crearDirectorio(ubicacion)
				Qt.openUrlExternally(ubicacion)
			}
		}
	}

	EditarCategoria {
		id: pantallaEditarCategoria
		visible: false
	}
}
