import QtQuick 2.12
import QtQuick.Controls 2.12


Menu {
	required property Item control

	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Cortar"
		Accessible.description: "Copia el texto seleccionado hacia el portapapeles y lo elimina del cuadro de texto"
		enabled: control.readOnly === false && control.selectedText.length > 0
		hoverEnabled: true
		icon.source: "qrc:/svg/cut.svg"
		text: "Cortar"

		onTriggered: control.cut()
	}
	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Copiar"
		Accessible.description: "Copia el texto seleccionado hacia el portapapeles"
		enabled: control.selectedText.length > 0
		hoverEnabled: true
		icon.source: "qrc:/svg/copy.svg"
		text: "Copiar"

		onTriggered: control.copy()
	}
	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Pegar"
		Accessible.description: "Inserta el texto del portapapeles en el cuadro de texto reemplazando el texto seleccionado si había"
		enabled: control.readOnly === false
		hoverEnabled: true
		icon.source: "qrc:/svg/paste.svg"
		text: "Pegar"

		onTriggered: control.paste()
	}
	MenuSeparator {}
	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Borrar"
		Accessible.description: "Elimina el texto seleccionado"
		enabled: control.readOnly === false && control.selectedText.length > 0
		hoverEnabled: true
		icon.source: "qrc:/svg/backspace.svg"
		text: "Borrar"

		onTriggered: control.remove(control.selectionStart, control.selectionEnd)
	}
	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Borrar todo"
		Accessible.description: "Elimina todo el contenido del cuadro de texto"
		enabled: control.readOnly === false && control.text.length > 0
		hoverEnabled: true
		icon.source: "qrc:/svg/eraser.svg"
		text: "Borrar todo"

		onTriggered: control.text = ""
	}
	MenuSeparator {}
	MenuItem {
		Accessible.role: Accessible.MenuItem
		Accessible.name: "Seleccionar todo"
		Accessible.description: "Selecciona todo el contenido en el cuadro de texto"
		enabled: control.text.length > 0
		hoverEnabled: true
		icon.source: "qrc:/svg/crop-alt.svg"
		text: "Seleccionar todo"

		onTriggered: control.selectAll()
	}
}
