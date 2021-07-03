import Qt.labs.platform 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionDescargas"
	property string titulo: "Descargas"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 3
	property alias descargasRutas: descargasRutas
	property alias descargasParalelas: descargasParalelas
	property alias descargasEliminarDelListadoAlFinalizar: descargasEliminarDelListadoAlFinalizar

	function mostrar() {
		descargasRutas.text = configuraciones.valor("descargas/ruta", StandardPaths.writableLocation(StandardPaths.DownloadLocation) + "/atds3")
		dialogoCarpetas.currentFolder = descargasRutas.text
		descargasParalelas.value = configuraciones.valor("descargas/paralelas", 3)
		descargasEliminarDelListadoAlFinalizar.checked = configuraciones.valor("descargas/eliminarDelListadoAlFinalizar", false)
		vistaApilable.push(this)
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración de las descargas"
	Accessible.description: "Aquí se definen las opciones que definen el comportamiento de las descargas"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
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
					columns: 3
//					columnSpacing: 10

					Label {
						Layout.columnSpan: 3
						Layout.topMargin: 20
						font.bold: true
						font.pointSize: ventanaPrincipal.font.pointSize + 2
						text: "Descargas"
					}

					Label {
						Layout.columnSpan: 3
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Ruta de guardado:"
					}
					EntradaLineaTexto {
						id: descargasRutas
						Accessible.name: "Ruta de guardado"
						Accessible.description: "Define la ruta en donde se guardarán las nuevas descargas. Dentro de esta ruta se crearán las carpetas de las categorías"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						readOnly: true
					}
					Button {
						Accessible.role: Accessible.Button
						Accessible.name: "Examinar"
						Accessible.description: "Muestra el cuadro de diálogo para seleccionar la ruta de guardado de las descargas"
						flat: true
						focus: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						icon.source: "qrc:/svg/folder.svg"
						text: "Examinar"

						onClicked: dialogoCarpetas.open()
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define la ruta en donde se guardarán las nuevas descargas. Dentro de esta ruta se crearán las carpetas de las categorías."
						wrapMode: Label.WordWrap
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}

					Label {
						Layout.columnSpan: 3
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Descargas paralelas:"
					}
					SpinBox {
						id: descargasParalelas
						Accessible.role: Accessible.SpinBox
						Accessible.name: "Descargas paralelas"
						Accessible.description: "Define el total de tareas de descargas que pueden estar activas de forma simultánea (máximo 10)"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						Layout.maximumWidth: 200
						focusPolicy: Qt.WheelFocus
						from: 1
						hoverEnabled: true
						to: 10
						wheelEnabled: true

						onValueModified: configuraciones.establecerValor("descargas/paralelas", value)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define el total de tareas de descargas que pueden estar activas de forma simultánea (máximo 10)."
						wrapMode: Label.WordWrap
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}

					Label {
						Layout.columnSpan: 3
						Layout.topMargin: 40
						font.bold: true
						font.pointSize: ventanaPrincipal.font.pointSize + 2
						text: "Acción al finalizar una descarga"
					}

					Label {
						Layout.columnSpan: 3
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Switch {
						id: descargasEliminarDelListadoAlFinalizar
						Accessible.role: Accessible.CheckBox
						Accessible.name: "Eliminar del listado al finalizar la descarga?"
						Accessible.description: "Define si se debe eliminar del listado la descarga tras finalizar"
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						text: "Eliminar del listado"

						onToggled: configuraciones.establecerValor("descargas/eliminarDelListadoAlFinalizar", checked)
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define si se debe eliminar del listado la descarga tras finalizar."
						wrapMode: Label.WordWrap
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
				}
			}
		}
	}

	FolderDialog {
		id: dialogoCarpetas
		options: FolderDialog.ShowDirsOnly

		onAccepted: {
			configuraciones.establecerValor("descargas/ruta", folder.toString())
			descargasRutas.text = folder.toString()
		}
	}
}
