import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12


RowLayout {
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== 1 && categoriaActual !== 2
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/file-download.svg"
		ToolTip.text: "Agregar descargas"
		ToolTip.visible: hovered

		onClicked: {
			if (menuLateral.opened === true) {
				menuLateral.close()
			}

			dialogoSeleccionarArchivoDescarga.folder = configuraciones.valor("atds3/ultimaRutaUtilizada", rutaSistemaDescargas)
			dialogoSeleccionarArchivoDescarga.open()
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual === 1
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/file-upload.svg"
		ToolTip.text: "Publicar archivos"
		ToolTip.visible: hovered

		onClicked: {
			if (menuLateral.opened === true) {
				menuLateral.close()
			}

			vistaApilable.push(pantallaPublicarContenido)
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== -1 && vistaPaquetes.listadoPaquetes.currentIndex !== -1
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/minus-square.svg"
		ToolTip.text: "Eliminar"
		ToolTip.visible: hovered

		onClicked: {
			if (menuLateral.opened === true) {
				menuLateral.close()
			}

			modeloPaquetes.eliminar(vistaPaquetes.listadoPaquetes.currentIndex)
			if (modeloPaquetes.rowCount() === 0) {
				vistaPaquetes.listadoPaquetes.currentIndex = -1
			}
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: vistaPaquetes.listadoPaquetes.count > 0
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/trash-alt.svg"
		ToolTip.text: "Limpiar"
		ToolTip.visible: hovered

		onClicked: {
			if (menuLateral.opened === true) {
				menuLateral.close()
			}

			modeloPaquetes.eliminarTodas()
			vistaPaquetes.listadoPaquetes.currentIndex = -1
		}
	}
}
