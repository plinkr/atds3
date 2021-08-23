import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionDescargas"
	property string titulo: "Descargas"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 3
	property alias androidAlmacenamientoPrimario: androidAlmacenamientoPrimario
	property alias androidAlmacenamientoExterno: androidAlmacenamientoExterno
	property alias descargasRutas: descargasRutas
	property alias descargasParalelas: descargasParalelas
	property alias descargasEliminarDelListadoAlFinalizar: descargasEliminarDelListadoAlFinalizar

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración de las descargas"
	Accessible.description: "Aquí se definen las opciones que definen el comportamiento de las descargas"
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
					columns: 3

					Subtitulo {
						Layout.columnSpan: 3
						Layout.topMargin: 20
						titulo: "Descargas"
					}

					Label {
						Layout.columnSpan: 3
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Ruta de guardado:"
					}
					RowLayout {
						Layout.columnSpan: 2
						Layout.fillWidth: true
						visible: sistemaOperativoAndroid === true

						RadioButton {
							id: androidAlmacenamientoPrimario
							text: "Primario"

							onClicked: {
								configuraciones.establecerValor("descargas/androidAlmacenamiento", 0)
								configuraciones.establecerValor("descargas/ruta", androidRutaPrimario)
								descargasRutas.text = utiles.rutaDesdeURI(androidRutaPrimario)
							}
						}
						RadioButton {
							id: androidAlmacenamientoExterno
							text: "Externo"

							onClicked: {
								configuraciones.establecerValor("descargas/androidAlmacenamiento", 1)
								configuraciones.establecerValor("descargas/ruta", androidRutaExterno)
								descargasRutas.text = utiles.rutaDesdeURI(androidRutaExterno)
//								utiles.seleccionarRutaExterno()
							}
						}
					}
					Label {
						visible: sistemaOperativoAndroid === true && columnasIntegradasVistaVertical === 1
					}
					EntradaLineaTexto {
						id: descargasRutas
						Accessible.name: "Ruta de guardado"
						Accessible.description: "Define la ruta en donde se guardarán las nuevas descargas. Dentro de esta ruta se crearán las carpetas de las categorías"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: sistemaOperativoAndroid === true ? (columnasIntegradasVistaVertical === 1 ? 2 : 3) : (columnasIntegradasVistaVertical > 1 ? 2 : 1)
						Layout.fillWidth: true
						readOnly: true
					}
					Button {
						id: botonExaminar
						Accessible.role: Accessible.Button
						Accessible.name: "Examinar"
						Accessible.description: "Muestra el cuadro de diálogo para seleccionar la ruta de guardado de las descargas"
						enabled: sistemaOperativoAndroid === false
						flat: true
						focus: true
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						icon.source: "qrc:/svg/folder.svg"
						text: "Examinar"
						visible: sistemaOperativoAndroid === false

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
						Accessible.name: "Descargas paralelas. Valor: " + value
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

					Subtitulo {
						Layout.columnSpan: 3
						Layout.topMargin: 40
						titulo: "Acción al finalizar un paquete"
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
						Accessible.name: "Eliminar el paquete del listado al finalizar la descarga?"
						Accessible.description: "Define si se debe eliminar el paquete del listado tras finalizar exitosamente la descarga de todas sus tareas."
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
						text: "Define si se debe eliminar el paquete del listado tras finalizar exitosamente la descarga de todas sus tareas."
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

	FileDialog {
		id: dialogoCarpetas
		title: "Seleccione la carpeta de las descargas"
		selectExisting: true
		selectMultiple: false
		selectFolder: true

		onAccepted: {
			let uri = decodeURIComponent(fileUrl)

			configuraciones.establecerValor("descargas/ruta", uri)
			descargasRutas.text = utiles.rutaDesdeURI(uri)
		}
	}

	Component.onCompleted: {
		let ruta = configuraciones.valor("descargas/ruta", rutaSistemaDescargas + "/atds3")

		if (sistemaOperativoAndroid === true) {
			let androidAlmacenamiento = parseInt(configuraciones.valor("descargas/androidAlmacenamiento", 0));

			androidAlmacenamientoPrimario.checked = androidAlmacenamiento === 0
			androidAlmacenamientoExterno.checked = androidAlmacenamiento === 1

			switch (androidAlmacenamiento) {
				case 0:
					descargasRutas.text = utiles.rutaDesdeURI(androidRutaPrimario)
					break;
				case 1:
					descargasRutas.text = utiles.rutaDesdeURI(androidRutaExterno)
					break;
			}
		} else {
			dialogoCarpetas.folder = ruta
			descargasRutas.text = utiles.rutaDesdeURI(ruta)
		}
		descargasParalelas.value = parseInt(configuraciones.valor("descargas/paralelas", 1))
		descargasEliminarDelListadoAlFinalizar.checked = configuraciones.valor("descargas/eliminarDelListadoAlFinalizar", false)
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		descargasParalelas.forceActiveFocus()
	}
/*
	Connections {
		target: utiles

		function onNotificarSeleccionRuta(ruta) {
			let uri = decodeURIComponent(ruta)

			configuraciones.establecerValor("descargas/ruta", uri)
			descargasRutas.text = utiles.rutaDesdeURI(uri)
		}
	}
*/
}
