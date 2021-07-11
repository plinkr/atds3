import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "FalloInicioSesion"
	property string titulo: "Fallo en el inicio de sesión"
	property alias mensajeError: mensajeError

	function mostrar(error) {
		Accessible.description = `Ha ocurrido un error al intentar iniciar sesión en la red toDus con el número de teléfono declarado previamente. Código del error: ${error}.`
		mensajeError.text = `Ha ocurrido un error al intentar iniciar sesión en la red toDus con el número de teléfono declarado previamente.<br/><br/>Código del error: ${error}.`

		switch (parseInt(error)) {
			case 418:
				Accessible.description += " Explicación: Usted tiene restricción temporal en la red toDus."
				mensajeError.text += "<br/>Explicación: Usted tiene restricción temporal en la red toDus."
				break;
			default:
				Accessible.description += " Este error es desconocido y, por lo tanto, no existe una explicación."
				mensajeError.text += "<br/>Este error es desconocido. Se le sugiere que acceda al <a href=\"http://t.me/atds3_debate\"><i>grupo de debate de ATDS3</i></a> para que intente obtener una explicación."
				break;
		}

		vistaApilable.push(this)
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de visualización del fallo del inicio de sesión"
	header: BarraBotones { titulo: parent.titulo }

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 20

		ColumnLayout {
			Layout.alignment: Qt.AlignCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.maximumWidth: 720

			Image {
				Layout.fillWidth: true
				fillMode: Image.PreserveAspectFit
				sourceSize.height: tamanoIconos === 48 ? 128 : 64
				sourceSize.width: tamanoIconos === 48 ? 128 : 64
				source: "qrc:/svg/sad-cry.svg"
			}

			Label {
				id: mensajeError
				Layout.fillWidth: true
				Layout.topMargin: 40
				horizontalAlignment: Qt.AlignHCenter
				font.bold: true
				wrapMode: Label.WordWrap

				MouseArea {
					anchors.fill: parent
					acceptedButtons: Qt.NoButton
					cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
				}
			}
		}
	}
}
