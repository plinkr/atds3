import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.0


Page {
	property string nombre: "Principal"
	property string titulo: "ATDS3"
	property alias vistaCategorias: vistaCategorias
	property string nombreCategoriaActual: ""
	property string iconoCategoriaActual: ""
	property alias vistaPaquetes: vistaPaquetes

    Layout.fillHeight: true
    Layout.fillWidth: true

	Shortcut {
		sequence: "Menu"
		onActivated: menuLateral.open()

	}

    header: orientacionHorizontal === true ? barraBotonesHorizontal : barraBotonesVertical

	MenuLateral {
		id: menuLateral
	}

	ToolBar {
		id: barraBotonesVertical
		padding: 4
        width: pantallaPrincipal.width
		visible: orientacionHorizontal === false

		RowLayout {
			anchors.fill: parent

			ToolButton {
				icon.source: "qrc:/svg/bars.svg"
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				onClicked: menuLateral.open()
			}
			Label {
				Layout.fillHeight: true
				Layout.fillWidth: true
			}
/*
			Image {
				Layout.fillHeight: true
				Layout.preferredWidth: 24
				antialiasing: true
				fillMode: Image.PreserveAspectFit
				sourceSize.height: 24
				sourceSize.width: 24
				source: iconoCategoriaActual.length > 0 ? `qrc:/svg/${iconoCategoriaActual}` : ""

				ColorOverlay {
					anchors.fill: parent
					source: parent
					color: Material.foreground
				}
			}
*/
			Label {
				Layout.fillHeight: true
				Layout.leftMargin: 10
				clip: true
				elide: Label.ElideRight
				font.pointSize: ventanaPrincipal.font.pointSize + 2
				font.bold: true
				verticalAlignment: Qt.AlignVCenter
				text: nombreCategoriaActual
			}
			Label {
				Layout.fillHeight: true
				Layout.fillWidth: true
			}
			ToolButton {
				icon.source: "qrc:/svg/cog.svg"
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				onClicked: vistaApilable.push(pantallaConfiguracion)
			}
		}
	}

	ToolBar {
		id: barraBotonesHorizontal
//		padding: 4
		visible: orientacionHorizontal === true

		RowLayout {
			anchors.fill: parent
			spacing: 4

			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex !== 1 && vistaCategorias.listadoCategorias.currentIndex !== 2
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/file-download.svg"
				ToolTip.text: "Agregar descargas"
				ToolTip.visible: hovered

				onClicked: dialogoSeleccionarArchivoDescarga.open()
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex === 1
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/file-upload.svg"
				ToolTip.text: "Publicar archivos"
				ToolTip.visible: hovered

				onClicked: pantallaPublicarContenido.mostrar()
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaPaquetes.listadoPaquetes.currentIndex !== -1
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/minus-square.svg"
				ToolTip.text: "Eliminar"
				ToolTip.visible: hovered

				onClicked: {
					modeloPaquetes.eliminar(vistaPaquetes.listadoPaquetes.currentIndex)
					if (modeloPaquetes.rowCount() === 0) {
						vistaPaquetes.listadoPaquetes.currentIndex = -1
					}
				}
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaPaquetes.listadoPaquetes.count > 0
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/trash-alt.svg"
				ToolTip.text: "Limpiar"
				ToolTip.visible: hovered

				onClicked: {
					modeloPaquetes.eliminarTodas()
					vistaPaquetes.listadoPaquetes.currentIndex = -1
				}
			}

			ToolSeparator {}

			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex !== 2 && vistaPaquetes.listadoPaquetes.currentIndex !== -1
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/play.svg"
				ToolTip.text: "Iniciar"
				ToolTip.visible: hovered

				onClicked: modeloPaquetes.iniciar(vistaPaquetes.listadoPaquetes.currentIndex)
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex !== 2 && vistaPaquetes.listadoPaquetes.currentIndex !== -1
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/pause.svg"
				ToolTip.text: "Pausar"
				ToolTip.visible: hovered

				onClicked: modeloPaquetes.pausar(vistaPaquetes.listadoPaquetes.currentIndex)
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex !== 2 && vistaPaquetes.listadoPaquetes.count > 0
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/play-circle.svg"
				ToolTip.text: "Iniciar todas"
				ToolTip.visible: hovered

				onClicked: modeloPaquetes.iniciarTodas()
			}
			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: ToolTip.text
				display: AbstractButton.TextUnderIcon
				enabled: vistaCategorias.listadoCategorias.currentIndex !== 2 && vistaPaquetes.listadoPaquetes.count > 0
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/pause-circle.svg"
				ToolTip.text: "Pausar todas"
				ToolTip.visible: hovered

				onClicked: modeloPaquetes.pausarTodas()
			}

			Item {
				Layout.fillWidth: true
			}

			ItemDelegate {
				Accessible.role: Accessible.Button
				Accessible.name: "Configuración"
				display: AbstractButton.TextUnderIcon
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				icon.height: tamanoIconos
				icon.width: tamanoIconos
				icon.source: "qrc:/svg/cog.svg"

				onClicked: vistaApilable.push(pantallaConfiguracion)
			}
		}
	}

	RowLayout {
		anchors.fill: parent
		spacing: 4

		Categorias {
			id: vistaCategorias
			visible: ventanaPrincipal.orientacionHorizontal === true
		}

        Paquetes {
            id: vistaPaquetes
			Accessible.role: Accessible.List
			Accessible.name: "Listado de los paquetes de descargas y publicaciones"
		}
	}

	FileDialog {
		id: dialogoSeleccionarArchivoDescarga
		fileMode: FileDialog.OpenFiles
		folder: configuraciones.valor("atds3/ultimaRutaUtilizada", StandardPaths.writableLocation(StandardPaths.DownloadLocation))
//		nameFilters: [ "Archivos de descarga soportados(*.s3 *.txt)", "S3(*.s3)", "Clásico(*.txt)" ]
		nameFilters: [ "Archivos de descarga soportados(*.txt)", "Clásico(*.txt)" ]
		options: FileDialog.ReadOnly

		onAccepted: {
			let categoria = modeloCategorias.obtener(vistaCategorias.listadoCategorias.currentIndex)

			configuraciones.establecerValor("atds3/ultimaRutaUtilizada", folder.toString())
			vistaPaquetes.listadoPaquetes.currentIndex = -1
			modeloPaquetes.agregarDescargasDesdeArchivos(currentFiles)
		}
	}
}
