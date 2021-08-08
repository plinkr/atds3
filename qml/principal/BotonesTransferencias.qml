import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12


RowLayout {
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== 2 && vistaPaquetes.listadoPaquetes.currentIndex !== -1
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/play.svg"
		ToolTip.text: "Iniciar"
		ToolTip.visible: hovered

		onClicked: {
			modeloPaquetes.iniciar(vistaPaquetes.listadoPaquetes.currentIndex)

			if (menuLateral.opened === true) {
				menuLateral.close()
			}
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== 2 && vistaPaquetes.listadoPaquetes.currentIndex !== -1
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/pause.svg"
		ToolTip.text: "Pausar"
		ToolTip.visible: hovered

		onClicked: {
			modeloPaquetes.pausar(vistaPaquetes.listadoPaquetes.currentIndex)

			if (menuLateral.opened === true) {
				menuLateral.close()
			}
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== 2 && vistaPaquetes.listadoPaquetes.count > 0
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/play-circle.svg"
		ToolTip.text: "Iniciar todas"
		ToolTip.visible: hovered

		onClicked: {
			modeloPaquetes.iniciarTodas()

			if (menuLateral.opened === true) {
				menuLateral.close()
			}
		}
	}
	ToolButton {
		Accessible.role: Accessible.Button
		Accessible.name: ToolTip.text
		display: AbstractButton.TextUnderIcon
		enabled: categoriaActual !== 2 && vistaPaquetes.listadoPaquetes.count > 0
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.source: "qrc:/svg/pause-circle.svg"
		ToolTip.text: "Pausar todas"
		ToolTip.visible: hovered

		onClicked: {
			modeloPaquetes.pausarTodas()

			if (menuLateral.opened === true) {
				menuLateral.close()
			}
		}
	}
}
