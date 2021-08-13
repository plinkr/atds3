import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionProxy"
	property string titulo: "Proxy"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias proxyActivado: proxyActivado
	property alias proxyTipo: proxyTipo
	property alias proxyAnfitrion: proxyAnfitrion
	property alias proxyPuerto: proxyPuerto
	property alias proxyUsuario: proxyUsuario
	property alias proxyContrasena: proxyContrasena

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de información del proxy"
	Accessible.description: "Aquí se definen las opciones del proxy a utilizar en las conexiones"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
		id: deslizante
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		contentHeight: contenido.height
		ScrollIndicator.vertical: ScrollIndicator { }

		ColumnLayout {
			id: contenido
			width: parent.width

			ColumnLayout {
				Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
				Layout.fillWidth: true
				Layout.maximumWidth: 720
				Layout.margins: 20

				Switch {
					id: proxyActivado
					Accessible.role: Accessible.CheckBox
					Accessible.name: text
					Accessible.description: "Define si se utilizará un servidor proxy para establecer las conexiones de red. Si no está segura(o) de los valores que debe introducir entonces consulte con su administrador de red"
					focus: true
					focusPolicy: Qt.StrongFocus
					hoverEnabled: true
					Layout.columnSpan: 2
					Layout.topMargin: 20
					text: "Utilizar un servidor proxy?"

					onToggled: configuraciones.establecerValor("proxy/activado", checked)
				}
				Label {
					font.pointSize: ventanaPrincipal.font.pointSize - 1
					Layout.bottomMargin: 24
					Layout.columnSpan: 2
					Layout.fillWidth: true
					Layout.leftMargin: 55
					text: "Define si se utilizará un servidor proxy para establecer las conexiones de red. Si no está segura(o) de los valores que debe introducir entonces consulte con su administrador de red."
					wrapMode: Label.WordWrap
				}

				GridLayout {
					columns: 2
					enabled: proxyActivado.checked
					Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
					Layout.fillWidth: true

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Tipo de proxy:"
					}
					ComboBox {
						id: proxyTipo
						Accessible.role: Accessible.ComboBox
						Accessible.name: "Tipo de proxy"
						Accessible.description: "Define el tipo de proxy a utilizar"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						model: ListModel {
							ListElement { titulo: "HTTP(S)"; valor: 3 }
							ListElement { titulo: "SOCKS"; valor: 1 }
						}
						textRole: "titulo"
						valueRole: "valor"
						onActivated: configuraciones.establecerValor("proxy/tipo", currentValue)
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Anfitrión:"
					}
					EntradaLineaTexto {
						id: proxyAnfitrion
						Accessible.name: "Anfitrión"
						Accessible.description: "Define el nombre DNS o dirección IP del anfitrión"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true

						onEditingFinished: configuraciones.establecerValor("proxy/anfitrion", text.trim())
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Puerto:"
					}
					EntradaLineaTexto {
						id: proxyPuerto
						Accessible.name: "Puerto del anfitrión"
						Accessible.description: "Define el número del puerto del servidor anfitrión"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						maximumLength: 5
						validator: IntValidator { bottom: 1; top: 65535 }

						onEditingFinished: configuraciones.establecerValor("proxy/puerto", parseInt(text.trim()))
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Usuario:"
					}
					EntradaLineaTexto {
						id: proxyUsuario
						Accessible.name: "Usuario"
						Accessible.description: "Define el nombre de usuario a utilizar en el servidor proxy"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true

						onEditingFinished: configuraciones.establecerValor("proxy/usuario", text.trim())
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Contraseña:"
					}
					EntradaLineaTexto {
						id: proxyContrasena
						Accessible.name: "Contraseña"
						Accessible.description: "Define la contraseña del usuario a utilizar en el servidor proxy"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						echoMode: TextInput.Password

						onEditingFinished: configuraciones.establecerValor("proxy/contrasena", text)
					}
				}
			}
		}
	}

	Component.onCompleted: {
		proxyActivado.checked = parseInt(configuraciones.valor("proxy/activado", false))
		proxyTipo.currentIndex = proxyTipo.indexOfValue(configuraciones.valor("proxy/tipo", 3))
		proxyAnfitrion.text = configuraciones.valor("proxy/anfitrion", "proxy.local")
		proxyPuerto.text = configuraciones.valor("proxy/puerto", 3128)
		proxyUsuario.text = configuraciones.valor("proxy/usuario", "")
		proxyContrasena.text = ""
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		proxyActivado.forceActiveFocus()
	}
}
