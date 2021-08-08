import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12


ToolBar {
	required property string titulo

	id: elementoRaiz

	RowLayout {
		anchors.fill: parent

		ToolButton {
			Accessible.role: Accessible.Button
			Accessible.name: "Atrás"
			Accessible.description: "Cambia a la pantalla anterior"
			focusPolicy: Qt.StrongFocus
			icon.source: "qrc:/svg/arrow-left.svg"
			hoverEnabled: true
			onClicked: vistaApilable.pop()
		}
		Label {
			Layout.fillWidth: true
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			clip: true
			elide: Label.ElideMiddle
			font.bold: true
			font.pointSize: ventanaPrincipal.font.pointSize + 2
			text: elementoRaiz.titulo
		}
	}
}
