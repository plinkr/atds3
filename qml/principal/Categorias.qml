import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12


Pane {
	property alias listadoCategorias: listadoCategorias
	property alias indicadorDisponibilidadTodus: indicadorDisponibilidadTodus

	Accessible.role: Accessible.List
	Accessible.name: "Listado de las categorías"
	Accessible.description: "Aquí se muestra el listado editable de las categorías en donde se almacenarán los paquetes de descargas y publicaciones"
	Layout.fillHeight: true
	Layout.preferredWidth: 300
	state: orientacionHorizontal ? estadoListadoCategorias : "expandido"

	function mostrarCategoria(indice) {
		if (listadoCategorias.currentIndex !== indice) {
			let registroCategoria = modeloCategorias.obtener(indice)

			vistaPaquetes.listadoPaquetes.currentIndex = -1
			categoriaActual = indice
			nombreCategoriaActual = registroCategoria.titulo
			iconoCategoriaActual = registroCategoria.icono
			modeloPaquetes.establecerFiltroCategoria(registroCategoria.id)
			configuraciones.establecerValor("atds3/ultimaCategoriaSeleccionada", indice)
		}
	}

	background: Rectangle {
		anchors.fill: parent
		color: Material.color(colorAcento, Material.Shade700)
	}

	ColumnLayout {
		anchors.fill: parent

		RowLayout {
			clip: true
			width: parent.width
			visible: ventanaPrincipal.orientacionHorizontal === false

			Label {
				font.bold: true
				text: "Acciones"
			}
		}

		BotonesPaquetes {
			visible: ventanaPrincipal.orientacionHorizontal === false
		}
		BotonesTransferencias {
			Layout.bottomMargin: 20
			visible: ventanaPrincipal.orientacionHorizontal === false
		}

		RowLayout {
			clip: true
			width: parent.width

			Label {
				font.bold: true
				text: "Categorías"
				visible: ventanaPrincipal.orientacionHorizontal === true ? vistaCategorias.state === "expandido" : true
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
				icon.height: 16
				icon.width: 16
				icon.source: "qrc:/svg/plus-square.svg"
				visible: ventanaPrincipal.orientacionHorizontal === true ? vistaCategorias.state === "expandido" : true

				onClicked: {
					menuLateral.close()
					vistaApilable.push(pantallaEditarCategoria)
					vistaApilable.currentItem.crear()
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
				icon.height: 16
				icon.width: 16
				icon.source: "qrc:/svg/chevron-circle-left.svg"
				visible: orientacionHorizontal === true
				state: vistaCategorias.state

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
					actualizarEstadoListadoCategorias(vistaCategorias.state === "expandido" ? "contraido" : "expandido")
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
			currentIndex: categoriaActual
			clip: true
			highlightFollowsCurrentItem: true
			model: modeloCategorias
			delegate: ItemDelegate {
				Accessible.role: Accessible.ListItem
				Accessible.name: `Categoría ${model.titulo}`
				ToolTip.text: vistaCategorias.state === "expandido" ? "" : model.titulo
				ToolTip.visible: vistaCategorias.state === "expandido" ? false : hovered
				clip: true
				display: AbstractButton.TextBesideIcon
				focusPolicy: Qt.StrongFocus
				font.bold: ListView.isCurrentItem
				highlighted: ListView.isCurrentItem
				hoverEnabled: true
				icon.color: ListView.isCurrentItem === true ? Material.foreground : Material.color(colorAcento, Material.Shade100)
				icon.source: `qrc:/svg/${model.icono}`
				spacing: 20
				text: vistaCategorias.state === "expandido" ? model.titulo : ""
				width: ListView.view.width

				Behavior on icon.color {
					ColorAnimation {
						duration: 100
					}
				}

				onClicked: {
					mostrarCategoria(index)

					if (menuLateral.opened === true) {
						menuLateral.close()
					}
				}

				onPressAndHold: {
					mostrarCategoria(index)

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
						mostrarCategoria(index)

						if (mouse.button === Qt.RightButton && index !== 2) {
						   menuCategoria.popup()
					   }
				   }
				}
			}

			ScrollIndicator.vertical: ScrollIndicator {}
		}
		RowLayout {
			Layout.alignment: Qt.AlignRight

			Rectangle {
				id: indicadorDisponibilidadTodus
				Layout.alignment: Qt.AlignCenter
				height: 10
				width: 10
				radius: 50
				state: estadoDisponibilidadTodus
				ToolTip.text: state === "perdido" ? "Conexión perdida con los servidores de toDus" : "Conexión disponible con los servidores de toDus"
				states: [
					State {
						name: "disponible"
						PropertyChanges {
							target: indicadorDisponibilidadTodus
							color: (colorAcento === Material.Cyan || colorAcento === Material.Green || colorAcento === Material.Teal || colorAcento === Material.LightGreen ? Material.color(Material.Yellow) : Material.color(Material.Green))
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

	states: [
		State {
			name: "expandido"
			PropertyChanges {
				target: vistaCategorias
				Layout.preferredWidth: 300
			}
		},
		State {
			name: "contraido"
			PropertyChanges {
				target: vistaCategorias
				Layout.preferredWidth: ventanaPrincipal.orientacionHorizontal === true ? 80 : 300
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
			icon.source: `qrc:/svg/${modeloCategorias.obtener(listadoCategorias.currentIndex).icono}`
			text: modeloCategorias.obtener(listadoCategorias.currentIndex).titulo
		}
		MenuSeparator {}
		MenuItem {
			enabled: listadoCategorias.currentIndex > 2
			hoverEnabled: true
			icon.source: "qrc:/svg/edit.svg"
			text: "Editar"
			onTriggered:{
				if (menuLateral.opened === true) {
					menuLateral.close()
				}

				vistaApilable.push(pantallaEditarCategoria)
				vistaApilable.currentItem.editar(listadoCategorias.currentIndex)
			}
		}
		MenuItem {
			enabled: listadoCategorias.currentIndex > 2
			hoverEnabled: true
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
			icon.source: "qrc:/svg/folder-open.svg"
			text: "Abrir ubicación"
			onTriggered: {
				let registroCategoria = modeloCategorias.obtener(listadoCategorias.currentIndex)
				let ubicacion = configuraciones.valor("descargas/ruta", rutaSistemaDescargas + "/atds3") + "/" + registroCategoria.titulo

				utiles.crearDirectorio(ubicacion)
				Qt.openUrlExternally(ubicacion)
			}
		}
	}

	Component {
		id: pantallaEditarCategoria

		EditarCategoria {}
	}
}
