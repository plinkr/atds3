import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12


Dialog {
	anchors.centerIn: parent
	modal: true
	title: "Apariencia"

	standardButtons: Dialog.Ok

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
				model: [ "Amarillo", "Ámber", "Azul", "Azul claro", "Azul oscuro", "Cian", "Gris", "Gris azulado", "Marrón", "Naranja", "Naranja profundo", "Púrpura", "Púrpura oscuro", "Rojo", "Rosado", "Verde", "Verde azulado", "Verde claro", "Verde lima" ]

				onActivated: {
					configuraciones.establecerValor("atds3/temaColores", currentIndex)
					ventanaPrincipal.actualizarTemaColores()
				}

				Component.onCompleted: {
					currentIndex = configuraciones.valor("atds3/temaColores", 4)
				}
			}
		}
	}

	onAccepted: {
		close()
	}
}
