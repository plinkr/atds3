import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12


Dialog {
	id: dialogoApariencia
	anchors.centerIn: parent
	modal: true
	title: "Apariencia"

	standardButtons: Dialog.Ok
	onAccepted: {
		dialogoApariencia.close()
	}

	ColumnLayout {
		spacing: 20

		GridLayout {
			columns: 2
			columnSpacing: 10

			Label {
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				text: "Colores:"
			}

			ComboBox {
				id: listadoEstilos
				Accessible.role: Accessible.ComboBox
				Accessible.name: "Colores"
				Accessible.description: "Define el tema de color a utilizar en ATDS3"
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.fillWidth: true
				focus: true
				focusPolicy: Qt.WheelFocus
				hoverEnabled: true
				wheelEnabled: true
				model: [ "Azul", "Gris", "Marrón","Morado", "Naranja", "Rojo", "Rosado", "Verde" ]

				onActivated: {
					configuraciones.establecerValor("atds3/temaColores", currentIndex)
					ventanaPrincipal.actualizarTemaColores()
				}

				Component.onCompleted: {
					currentIndex = configuraciones.valor("atds3/temaColores")
				}
			}

			Label {
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				text: "Tamaño de los íconos:"
			}

			ComboBox {
				id: listadoTamanoIconos
				Accessible.role: Accessible.ComboBox
				Accessible.name: "Tamaño de los íconos"
				Accessible.description: "Define el tamaño de los íconos a utilizar en ATDS3"
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.fillWidth: true
				focusPolicy: Qt.WheelFocus
				hoverEnabled: true
				wheelEnabled: true
				model: [ "Pequeño", "Normal" ]

				onActivated: {
					actualizarTamanoIconos(currentIndex)
				}

				Component.onCompleted: {
					currentIndex = configuraciones.valor("atds3/tamanoIconos", 1)
				}
			}
		}
	}
}
