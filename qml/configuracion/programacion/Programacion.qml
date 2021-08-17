import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionProgramacion"
	property string titulo: "Programación"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias colaTransferenciasInicio: colaTransferenciasInicio
	property alias colaTransferenciasInicioHora: colaTransferenciasInicioHora
	property alias colaTransferenciasFinalizar: colaTransferenciasFinalizar
	property alias colaTransferenciasFinalizarHora: colaTransferenciasFinalizarHora
	property alias colaTransferenciasAccionInicio: colaTransferenciasAccionInicio
	property alias colaTransferenciasAccionFinalizar: colaTransferenciasAccionFinalizar

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración de las acciones programadas"
	Accessible.description: "Aquí se definen las opciones de la programación de acciones"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
		id:deslizante
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

					Subtitulo {
						Layout.columnSpan: 2
						Layout.topMargin: 20
						titulo: "Cola de transferencias"
					}

					Label {
						Layout.columnSpan: 2
					}

					Switch {
						id: colaTransferenciasInicio
						Accessible.role: Accessible.CheckBox
						Accessible.name: text
						Accessible.description: "Define si se debe iniciar de forma automática la cola de las transferencias"
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						focus: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						text: "Inicio automático?"

						onToggled: {
							configuraciones.establecerValor("programacion/colaTransferenciasInicio", checked)
							utiles.activarProgramacionInicioColaTransferencias(checked)
						}
					}
					EntradaLineaTexto {
						id: colaTransferenciasInicioHora
						Accessible.name: "Hora de inicio de la cola de las transferencias"
						Accessible.description: "Define la hora cuando se debe iniciar de forma automática la cola de las transferencias (formato de 24 horas)"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						Layout.maximumWidth: 75
						enabled: colaTransferenciasInicio.checked
						horizontalAlignment: Qt.AlignHCenter
						inputMask: "99:99"

						onEditingFinished: configuraciones.establecerValor("programacion/colaTransferenciasInicioHora", text)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasInicio.checked
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define la hora cuando se debe iniciar de forma automática la cola de las transferencias (formato de 24 horas)."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						enabled: colaTransferenciasInicio.checked
						text: "Acción al iniciar:"
					}
					ComboBox {
						id: colaTransferenciasAccionInicio
						Accessible.role: Accessible.ComboBox
						Accessible.name: "Acción al iniciar"
						Accessible.description: "Define la acción a ejecutar al iniciar la cola de las transferencias"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasInicio.checked
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						model: ListModel {
							ListElement { titulo: "Ninguna"; valor: 0 }
							ListElement { titulo: "Apagar la pantalla"; valor: 1 }
						}
						textRole: "titulo"
						valueRole: "valor"
						onActivated: configuraciones.establecerValor("programacion/colaTransferenciasAccionInicio", currentValue)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasInicio.checked
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define la acción a ejecutar al iniciar la cola de las transferencias."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.columnSpan: 2
					}

					Switch {
						id: colaTransferenciasFinalizar
						Accessible.role: Accessible.CheckBox
						Accessible.name: text
						Accessible.description: "Define si se debe finalizar de forma automática la cola de las transferencias independientemente de si se completó o no"
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						focus: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						text: "Finalización automática?"

						onToggled: {
							configuraciones.establecerValor("programacion/colaTransferenciasFinalizar", checked)
							utiles.activarProgramacionFinalizarColaTransferencias(checked)
						}
					}
					EntradaLineaTexto {
						id: colaTransferenciasFinalizarHora
						Accessible.name: "Hora de finalización de la cola de las transferencias"
						Accessible.description: "Define la hora cuando se debe finalizar de forma automática la cola de las transferencias independientemente de si se completó o no"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						Layout.maximumWidth: 75
						enabled: colaTransferenciasFinalizar.checked === true
						horizontalAlignment: Qt.AlignHCenter
						inputMask: "99:99"

						onEditingFinished: configuraciones.establecerValor("programacion/colaTransferenciasFinalizarHora", text)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasFinalizar.checked === true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define la hora cuando se debe finalizar de forma automática la cola de las transferencias independientemente de si se completó o no."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						enabled: colaTransferenciasFinalizar.checked
						text: "Acción al finalizar:"
					}
					ComboBox {
						id: colaTransferenciasAccionFinalizar
						Accessible.role: Accessible.ComboBox
						Accessible.name: "Acción al finalizar"
						Accessible.description: "Define la acción a ejecutar al finalizar la cola de las transferencias"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasFinalizar.checked
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						model: ListModel {
							ListElement { titulo: "Ninguna"; valor: 0 }
							ListElement { titulo: "Suspender"; valor: 2 }
							ListElement { titulo: "Hibernar"; valor: 3 }
							ListElement { titulo: "Apagar"; valor: 4 }
						}
						textRole: "titulo"
						valueRole: "valor"
						onActivated: configuraciones.establecerValor("programacion/colaTransferenciasAccionFinalizar", currentValue)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						enabled: colaTransferenciasFinalizar.checked
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define la acción a ejecutar al finalizar la cola de las transferencias."
						wrapMode: Label.WordWrap
					}
				}
			}
		}
	}

	Component.onCompleted: {
		colaTransferenciasInicio.checked = parseInt(configuraciones.valor("programacion/colaTransferenciasInicio", false))
		colaTransferenciasInicioHora.text = configuraciones.valor("programacion/colaTransferenciasInicioHora", "00:00")
		colaTransferenciasFinalizar.checked = parseInt(configuraciones.valor("programacion/colaTransferenciasFinalizar", false))
		colaTransferenciasFinalizarHora.text = configuraciones.valor("programacion/colaTransferenciasFinalizarHora", "06:00")
		colaTransferenciasAccionInicio.currentIndex = colaTransferenciasAccionInicio.indexOfValue(parseInt(configuraciones.valor("programacion/colaTransferenciasAccionInicio", 0)))
		colaTransferenciasAccionFinalizar.currentIndex = colaTransferenciasAccionFinalizar.indexOfValue(parseInt(configuraciones.valor("programacion/colaTransferenciasAccionFinalizar", 0)))
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		colaTransferenciasInicio.forceActiveFocus()
	}
}
