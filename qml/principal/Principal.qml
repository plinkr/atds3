import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2


Page {
	property string nombre: "Principal"
	property string titulo: "ATDS3"
	property alias vistaCategorias: vistaCategorias
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
		visible: orientacionHorizontal === false

		RowLayout {
			anchors.fill: parent

			ToolButton {
				Accessible.role: Accessible.Button
				Accessible.name: "Menú lateral"
				icon.source: "qrc:/svg/bars.svg"
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				onClicked: menuLateral.open()
			}
			Label {
				Layout.fillHeight: true
				Layout.fillWidth: true
			}

			ToolButton {
				focusPolicy: Qt.NoFocus
				flat: true
				hoverEnabled: false
				icon.source: iconoCategoriaActual.length > 0 ? `qrc:/svg/${iconoCategoriaActual}` : ""
			}
			Label {
				Layout.fillHeight: true
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
				Accessible.role: Accessible.Button
				Accessible.name: "Configuración"
				icon.source: "qrc:/svg/cog.svg"
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				onClicked: vistaApilable.push(pantallaConfiguracion)
			}
		}
	}

	ToolBar {
		id: barraBotonesHorizontal
		visible: orientacionHorizontal === true

		RowLayout {
			anchors.fill: parent
			spacing: 4

			BotonesPaquetes {}

			ToolSeparator {}

			BotonesTransferencias {}

			Item {
				Layout.fillWidth: true
			}

			ToolButton {
				Accessible.role: Accessible.Button
				Accessible.name: "Configuración"
				display: AbstractButton.TextUnderIcon
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
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
		title: "Seleccione los archivos de descarga"
		selectExisting: true
		selectMultiple: true
		selectFolder: false
//		nameFilters: [ "Archivos de descarga soportados(*.s3 *.txt)", "S3(*.s3)", "Clásico(*.txt)" ]
		nameFilters: [ "Archivos de descarga soportados(*.txt)" ]
		selectedNameFilterIndex: 0

		onAccepted: {
			let categoria = modeloCategorias.obtener(vistaCategorias.listadoCategorias.currentIndex)

			configuraciones.establecerValor("atds3/ultimaRutaUtilizada", decodeURIComponent(folder))
			vistaPaquetes.listadoPaquetes.currentIndex = -1
			modeloPaquetes.agregarDescargasDesdeArchivos(fileUrls)
		}
	}

	Connections {
		target: utiles

		function onNotificarArchivoCompartido(ruta) {
			if (vistaCategorias.listadoCategorias.currentIndex > 0 && vistaCategorias.listadoCategorias.currentIndex < 3) {
				return
			}

			vistaPaquetes.listadoPaquetes.currentIndex = -1
			modeloPaquetes.agregarDescargasDesdeArchivos([ ruta ])
		}
	}
}
