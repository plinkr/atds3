import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "CodigoVerificacion"
	property string titulo: "Código de verificación"
	property alias codigo: codigo

	function confirmarCodigo() {
		modeloPaquetes.confirmarCodigo(codigo.text)
		vistaApilable.pop()
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla del código de verificación"
	Accessible.description: "Aquí se solicita el código de verificación que ha sido enviado al número de teléfono que configuró en la sección toDus"
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
				sourceSize.height: 80
				sourceSize.width: 80
				source: "qrc:/svg/code.svg"
			}

			Label {
				Layout.fillWidth: true
				Layout.topMargin: 40
				horizontalAlignment: Qt.AlignHCenter
				font.bold: true
				text: "Introduce el código de verificación que ha sido enviado a tu móvil para continuar con el proceso de inicio de sesión en toDus."
				wrapMode: Label.WordWrap
			}

			RowLayout {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.topMargin: 20

				Label {
					horizontalAlignment: Qt.AlignHCenter
					text: "Código:"
				}

				EntradaLineaTexto {
					id: codigo
					Accessible.name: "Código de verificación"
					Accessible.description: "Introduzca el código de verificación que ha sido enviado al número de teléfono que configuró en la sección toDus"
					Layout.fillWidth: true
					Layout.leftMargin: 10
					Layout.maximumWidth: 150
					Keys.onReturnPressed: confirmarCodigo()
					horizontalAlignment: TextField.AlignHCenter
					focus: true
					font.bold: true
					font.pointSize: ventanaPrincipal.font.pointSize + 2
					font.letterSpacing: 5
					maximumLength: 6
					validator: IntValidator { bottom: 0; top: 999999 }
				}
			}

			Button {
				Accessible.name: "Confirmar"
				Accessible.description: "Haga clic aquí para confirmar el código de verificación"
				flat: true
				focusPolicy: Qt.StrongFocus
				enabled: codigo.length === 6
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.maximumWidth: 200
				Layout.topMargin: 20
				icon.source: "qrc:/svg/check.svg"
				text: "Confirmar"

				onClicked: confirmarCodigo()
			}
		}
	}

	Component.onCompleted: {
		codigo.text = ""
		codigo.forceActiveFocus()
	}
}
