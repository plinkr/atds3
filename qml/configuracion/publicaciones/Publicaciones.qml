import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionPublicaciones"
	property string titulo: "Publicaciones"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias publicacionesParalelas: publicacionesParalelas
	property alias publicacionesReintentarTareasErroneas: publicacionesReintentarTareasErroneas
//	property alias publicacionesDividirPorVolumenes: publicacionesDividirPorVolumenes
//	property alias publicacionesTamanoVolumenes: publicacionesTamanoVolumenes

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración de las publicaciones"
	Accessible.description: "Aquí se definen las opciones que definen el comportamiento de las publicaciones"
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

				GridLayout {
					Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
					Layout.fillWidth: true
					columns: 2

					Subtitulo {
						Layout.columnSpan: 2
						Layout.topMargin: 20
						titulo: "Publicaciones"
					}

					Label {
						Layout.columnSpan: 2
						Layout.topMargin: 5
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Publicaciones paralelas:"
					}
					SpinBox {
						id: publicacionesParalelas
						Accessible.role: Accessible.SpinBox
						Accessible.name: "Publicaciones paralelas. Valor: " + value
						Accessible.description: "Define el total de tareas de publicación que pueden estar activas de forma simultánea (máximo 10)"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						Layout.maximumWidth: 200
						focus: true
						focusPolicy: Qt.WheelFocus
						from: 1
						hoverEnabled: true
						to: 10
						wheelEnabled: true

						onValueModified: configuraciones.establecerValor("publicaciones/paralelas", value)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define el total de tareas de publicación que pueden estar activas de forma simultánea (máximo 10)."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.columnSpan: 2
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Switch {
						id: publicacionesReintentarTareasErroneas
						Accessible.role: Accessible.CheckBox
						Accessible.name: text
						Accessible.description: "Habilite esta opción si desea que se reintenten automáticamente las tareas de publicación que se detengan con error."
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						text: "Reintentar las tareas erróneas"

						onToggled: configuraciones.establecerValor("publicaciones/reintentarTareasErroneas", checked)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Habilite esta opción si desea que se reintenten automáticamente las tareas de publicación que se detengan con error."
						wrapMode: Label.WordWrap
					}

/*
					Label {
						Layout.columnSpan: 2
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Switch {
						id: publicacionesDividirPorVolumenes
						hoverEnabled: true
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						text: "Dividir por volúmenes"

						onToggled: configuraciones.publicacionesDividirPorVolumenes = checked
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define si se deben dividir por volúmenes los archivos a subir. Disponible sólo para el formato S3."
						wrapMode: Label.WordWrap
					}

					Label {
						Layout.columnSpan: 2
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Tamaño de los volúmenes:"
					}
					TextField {
						id: publicacionesTamanoVolumenes
						hoverEnabled: true
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						validator: IntValidator { bottom: 5; top: 1024 }

						onEditingFinished: configuraciones.publicacionesTamanoVolumenes = parseInt(text.trim())
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define si tamaño de los volúmenes en MiB (MebiBytes, equivalente a 1024 KiB). Mínimo: 5 MiB. Máximo: 1024 MiB (1 GiB). Disponible sólo para el formato S3."
						wrapMode: Label.WordWrap
					}
*/
				}
			}
		}
	}

	Component.onCompleted: {
		publicacionesParalelas.value = configuraciones.valor("publicaciones/paralelas", 1)
		publicacionesReintentarTareasErroneas.checked = configuraciones.valor("publicaciones/reintentarTareasErroneas", false)
//		publicacionesDividirPorVolumenes.checked = configuraciones.publicacionesDividirPorVolumenes
//		publicacionesTamanoVolumenes.text = configuraciones.publicacionesTamanoVolumenes
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		publicacionesParalelas.forceActiveFocus()
	}
}
