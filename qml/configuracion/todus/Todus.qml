import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionTodus"
	property string titulo: "toDus"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias toDusTelefono: toDusTelefono
	property alias toDusFichaAcceso: toDusFichaAcceso
	property alias toDusProgramaPiscinaFichas: toDusProgramaPiscinaFichas
	property alias toDusProgramaPiscinaFichasInternet: toDusProgramaPiscinaFichasInternet
	property alias pantallaPPFInformacion: pantallaPPFInformacion

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración del servicio toDus"
	Accessible.description: "Aquí se definen las opciones que definen el comportamiento del servicio toDus"
	header: BarraBotones { titulo: parent.titulo }

	Component {
		id: pantallaPPFInformacion

		PPFInformacion {}
	}

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

				GridLayout {
					Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
					Layout.fillWidth: true
					columns: 2

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Número de teléfono:"
					}
					EntradaLineaTexto {
						id: toDusTelefono
						Accessible.name: "Número de teléfono"
						Accessible.description: "Este campo es uno de los más importantes de la configuración de ATDS3. Aquí se define el número de teléfono para el inicio de sesión en toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						focus: true
						maximumLength: 8

						onEditingFinished: configuraciones.establecerValor("todus/telefono", text)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Este campo es uno de los más importantes de la configuración de ATDS3. Aquí se define el número de teléfono para el inicio de sesión en toDus."
						wrapMode: Label.WordWrap
					}
					Button {
						Accessible.role: Accessible.Button
						Accessible.name: "Iniciar sesión"
						Accessible.description: "Haga clic aquí para iniciar sesión en la red toDus con el número configurado"
						Layout.alignment: Qt.AlignCenter
						Layout.columnSpan: 2
						Layout.topMargin: 10
						enabled: toDusTelefono.text.length === 8
						flat: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						icon.source: "qrc:/svg/sign-in-alt.svg"
						padding: 20
						spacing: 20
						text: "Iniciar sesión"

						onClicked: {
							configuraciones.eliminar("todus/fichaSolicitud")
							configuraciones.eliminar("todus/fichaAccesoTiempoExpiracion")
							configuraciones.eliminar("todus/fichaAcceso")
							toDusFichaAcceso.text = ""
							modeloPaquetes.iniciarSesionToDus()
						}
					}

					Label {
						Layout.columnSpan: 2
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Ficha de acceso:"
					}
					EntradaLineaTexto {
						id: toDusFichaAcceso
						Accessible.name: "Ficha de acceso"
						Accessible.description: "Este campo es opcional, dicho valor se obtiene del proceso de inicio de sesión. Solo debe ser configurado si el inicio de sesión se rompe debido a un cambio en toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true

						onTextEdited: configuraciones.establecerValor("todus/fichaAcceso", text)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Este campo es opcional, dicho valor se obtiene del proceso de inicio de sesión. Solo debe ser configurado si el inicio de sesión se rompe debido a un cambio en toDus."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.columnSpan: 2
						Layout.topMargin: 30
					}
					Image {
						Layout.alignment: columnasIntegradasVistaVertical === 1 ? Qt.AlignCenter : alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						fillMode: Image.PreserveAspectFit
						sourceSize.height: 48
						sourceSize.width: 48
						source: "qrc:/svg/atom.svg"
					}
					RowLayout {
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true

						Switch {
							id: toDusProgramaPiscinaFichas
							Accessible.role: Accessible.CheckBox
							Accessible.name: text
							Accessible.description: "Esta opción define si se debe activar o no la participación en el Programa de Fichas de ATDS3"
							hoverEnabled: true
							text: "<b>Programa Piscina de Fichas de ATDS3</b>"

							onToggled: configuraciones.establecerValor("todus/programaPiscinaFichas", checked)
						}
						Button {
							Accessible.role: Accessible.Button
							Accessible.name: "Más información sobre el Programa de Fichas de ATDS3"
							Accessible.description: "Haga clic en este botón para obtener más información sobre el Programa de Fichas de ATDS3"
							flat: true
							hoverEnabled: true
							icon.source: "qrc:/svg/question-circle.svg"

							onClicked: vistaApilable.push(pantallaPPFInformacion)
						}
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Switch {
						id: toDusProgramaPiscinaFichasInternet
						Accessible.role: Accessible.CheckBox
						Accessible.name: text
						Accessible.description: "Habilitando esta opción usted autoriza a ATDS3 a conectarse al servidor del Programa Piscina de Fichas de ATDS3 para aportar su ficha de acceso de la red toDus y solicitar las otras fichas de acceso para evadir los límites"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						hoverEnabled: true
						enabled: toDusProgramaPiscinaFichas.checked
						text: "Utilizar el proveedor de fichas de Internet"

						onToggled: configuraciones.establecerValor("todus/programaPiscinaFichasInternet", checked)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: toDusProgramaPiscinaFichas.checked
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Habilitando esta opción usted autoriza a ATDS3 a conectarse al servidor del Programa Piscina de Fichas de ATDS3 para aportar su ficha de acceso de la red toDus y solicitar las otras fichas de acceso para evadir los límites."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.topMargin: columnasIntegradasVistaVertical === 1 ? 20 : 0
						enabled: toDusProgramaPiscinaFichas.checked
						text: "Piscina local:"
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					ScrollView {
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						Layout.maximumHeight: 250
						clip: true

						EntradaAreaTexto {
							id: toDusProgramaPiscinaFichasLocal
							Accessible.name: "Piscina local"
							Accessible.description: "Este campo define el listado de fichas de acceso de la red toDus (una ficha por línea) a utilizar de forma local y en modo rotatorio en el Programa Piscina de Fichas de ATDS3. Este listado se utilizará en prioridad a las fichas de acceso que se soliciten al servidor de Internet"
							Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
							KeyNavigation.priority: KeyNavigation.BeforeItem
							KeyNavigation.tab: toDusTelefono
							enabled: toDusProgramaPiscinaFichas.checked

							onEditingFinished: {
								configuraciones.establecerValor("todus/programaPiscinaFichasLocal", text.trim())
							}
						}
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: toDusProgramaPiscinaFichas.checked
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Este campo define el listado de fichas de acceso de la red toDus (una ficha por línea) a utilizar de forma local y en modo rotatorio en el Programa Piscina de Fichas de ATDS3. Este listado se utilizará en prioridad a las fichas de acceso que se soliciten al servidor de Internet."
						wrapMode: Label.WordWrap
					}
				}
			}
		}
	}

	Component.onCompleted: {
		toDusTelefono.text = configuraciones.valor("todus/telefono", "")
		toDusFichaAcceso.text = configuraciones.valor("todus/fichaAcceso", "")
		toDusProgramaPiscinaFichas.checked = parseInt(configuraciones.valor("todus/programaPiscinaFichas", false))
		toDusProgramaPiscinaFichasInternet.checked = parseInt(configuraciones.valor("todus/programaPiscinaFichasInternet", true))
		toDusProgramaPiscinaFichasLocal.text = configuraciones.valor("todus/programaPiscinaFichasLocal", "")
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		toDusTelefono.forceActiveFocus()
	}
}
