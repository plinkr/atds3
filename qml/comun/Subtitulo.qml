import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12


ColumnLayout {
	required property string titulo

	id: elementoRaiz
	width: parent.width

	Label {
		Layout.fillWidth: true
		horizontalAlignment: Qt.AlignLeft
		verticalAlignment: Qt.AlignVCenter
		clip: true
		elide: Label.ElideMiddle
		font.bold: true
		font.pointSize: ventanaPrincipal.font.pointSize + 2
		text: elementoRaiz.titulo
	}

	Rectangle {
		Layout.fillWidth: true
		Layout.minimumHeight: 1
		Layout.maximumHeight: 1
		color: Material.backgroundDimColor
	}
}
