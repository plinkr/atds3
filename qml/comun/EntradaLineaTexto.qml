import QtQuick 2.12
import QtQuick.Controls 2.12


TextField {
	property int seleccionInicio: 0
	property int seleccionFin: 0
	property bool seleccion: false

	Accessible.role: Accessible.EditableText
	hoverEnabled: true
	selectByMouse: true
	wrapMode: TextField.WrapAnywhere

	function abrirMenuContextual() {
		seleccionInicio = parseInt(selectionStart)
		seleccionFin = parseInt(selectionEnd)
		seleccion = selectedText.length > 0

		textoMenuContextual.popup()
	}

	MouseArea {
		anchors.fill: parent
		acceptedButtons: Qt.RightButton
		cursorShape: Qt.IBeamCursor
		preventStealing: true

		onClicked: abrirMenuContextual()
	}

	onPressAndHold: abrirMenuContextual()

	TextoMenuContextual {
		id: textoMenuContextual
		control: parent

		onOpened: {
			if (seleccion === true) {
				select(seleccionInicio, seleccionFin)
			}
		}
	}
}
