import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.0
import "qrc:/qml/comun"


Page {
	property string nombre: "PublicarContenido"
	property string titulo: "Publicar contenido"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias tituloPaquete: tituloPaquete
	property alias listadoArchivosPublicar: listadoArchivosPublicar
	property alias modeloArchivosPublicar: modeloArchivosPublicar
	property alias formatoArchivoDescarga: formatoArchivoDescarga

	function mostrar() {
		tituloPaquete.text = ""
		modeloArchivosPublicar.clear()
		deslizable.contentY = 0
		vistaApilable.push(this)
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de la publicación de contenidos"
	Accessible.description: "Aquí se muestran los elementos necesarios para crear una publicación"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
		id: deslizable
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

				Image {
					Layout.fillWidth: true
					fillMode: Image.PreserveAspectFit
					sourceSize.height: 128
					sourceSize.width: 128
					source: "qrc:/svg/file-upload.svg"
/*
					ColorOverlay {
						anchors.fill: parent
						source: parent
						color: Material.accent
					}
*/
				}

				Label {
					Layout.fillWidth: true
					Layout.topMargin: 20
					horizontalAlignment: Qt.AlignHCenter
					font.bold: true
					text: "Defina el título del paquete, agregue los archivos que desea publicar en el siguiente listado, luego, seleccione el formato para crear el archivo de descarga del paquete que compartirá con los demás y, por último, presione el botón <u>Publicar</u>."
					wrapMode: Label.WordWrap
				}

				GridLayout {
					Layout.fillWidth: true
					Layout.topMargin: 40
					columns: 2

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Título del paquete:"
					}
					EntradaLineaTexto {
						id: tituloPaquete
						Accessible.name: "Título del paquete"
						Accessible.description: "Define el título del paquete. Será utilizado también como nombre del archivo de la descarga que podrá utilizar para compartir con los demás"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						focus: true
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define el título del paquete. Será utilizado también como nombre del archivo de la descarga que podrá utilizar para compartir con los demás."
						wrapMode: Label.WordWrap
					}
				}

				DropArea {
					Layout.preferredHeight: cuadroGrupoArchivosAPublicar.height
					Layout.fillWidth: true
					Layout.topMargin: 30

					onDropped: {
						if (drop.hasUrls === true) {
							for (let url of drop.urls) {
								modeloArchivosPublicar.append({ nombre: decodeURIComponent(url.toString().substring(url.toString().lastIndexOf("/") + 1)), ruta: url.toString() });
							}
						}
					}

					GroupBox {
						id: cuadroGrupoArchivosAPublicar
						Accessible.role: Accessible.Grouping
						Accessible.name: "Título del paquete"
						Accessible.description: "Define el título del paquete. Será utilizado también como nombre del archivo de la descarga que podrá utilizar para compartir con los demás"
						width: parent.width
						title: "Archivos a publicar"

						ColumnLayout {
							width: parent.width

							RowLayout {
								Layout.fillWidth: true

								Button {
									Accessible.role: Accessible.Button
									Accessible.name: "Agregar archivos"
									Accessible.description: "Haga clic aquí para agregar archivos al listado"
									flat: true
									focusPolicy: Qt.StrongFocus
									hoverEnabled: true
									icon.source: "qrc:/svg/file-medical.svg"
									text: ventanaPrincipal.orientacionHorizontal === true ? "Archivos" : ""

									onClicked: dialogoArchivos.open()
								}
/*
								Button {
									flat: true
									hoverEnabled: true
									icon.source: "qrc:/svg/folder-plus.svg"
									text: ventanaPrincipal.orientacionHorizontal === true ? "Directorio" : ""

									onClicked: dialogoDirectorios.open()
								}
*/
								Button {
									Accessible.role: Accessible.Button
									Accessible.name: "Eliminar"
									Accessible.description: "Haga clic aquí para eliminar el elemento seleccionado en el listado"
									flat: true
									focusPolicy: Qt.StrongFocus
									enabled: listadoArchivosPublicar.currentIndex !== -1
									hoverEnabled: true
									icon.source: "qrc:/svg/minus-square.svg"
									text: ventanaPrincipal.orientacionHorizontal === true ? "Eliminar" : ""

									onClicked: modeloArchivosPublicar.remove(listadoArchivosPublicar.currentIndex)
								}

								Button {
									Accessible.role: Accessible.Button
									Accessible.name: "Limpiar"
									Accessible.description: "Haga clic aquí para eliminar todos los elementos del listado"
									flat: true
									focusPolicy: Qt.StrongFocus
									enabled: modeloArchivosPublicar.count > 0
									hoverEnabled: true
									icon.source: "qrc:/svg/trash-alt.svg"
									text: ventanaPrincipal.orientacionHorizontal === true ? "Limpiar" : ""

									onClicked: modeloArchivosPublicar.clear()
								}
							}
							Rectangle {
								Layout.fillWidth: true
								height: 1
								color: Material.backgroundDimColor
							}

							ListView {
								id: listadoArchivosPublicar
								Layout.fillWidth: true
								Layout.minimumHeight: 300
								Layout.topMargin: 10
								boundsBehavior: Flickable.StopAtBounds
								clip: true
								currentIndex: -1
								highlightFollowsCurrentItem: true

								ScrollIndicator.vertical: ScrollIndicator {}

								model: ListModel {
									id: modeloArchivosPublicar
								}

								header: RowLayout {
									width: listadoArchivosPublicar.width
									spacing: 8

									Label {
										Layout.fillWidth: true
										font.bold: true
										text: "Nombre"
									}
								}

								delegate: ItemDelegate {
									highlighted: ListView.isCurrentItem
									hoverEnabled: true
									padding: 4
									height: 40
									width: listadoArchivosPublicar.width

									RowLayout {
										height: parent.height
										width: parent.width

										Label {
											Layout.fillWidth: true
											elide: Label.ElideRight
											text: model.nombre
											verticalAlignment: Qt.AlignVCenter
										}
									}

									onPressed: {
										listadoArchivosPublicar.currentIndex = index
									}
								}
							}
						}
					}
				}

				GridLayout {
					Layout.columnSpan: 2
					Layout.topMargin: 20
					columns: 2

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Formato del archivo de descarga:"
					}
					ComboBox {
						id: formatoArchivoDescarga
						Accessible.role: Accessible.ComboBox
						Accessible.name: "Formato del archivo de descarga"
						Accessible.description: "Define el formato del archivo que compartirá con los demás para que ellos puedan descargar los archivos que especificó previamente. Se recomienda que utilice el formato más reciente para mayor prestaciones y rendimiento en las descargas"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						hoverEnabled: true
						focusPolicy: Qt.WheelFocus
						currentIndex: 0
						textRole: "titulo"
						valueRole: "valor"
						wheelEnabled: true
						model: ListModel {
//							ListElement { titulo: "S3 (.s3)"; valor: Paquetes.Formatos.S3 }
							ListElement { titulo: "Clásico (.txt)"; valor: Paquetes.Formatos.Clasico }
						}
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Define el formato del archivo que compartirá con los demás para que ellos puedan descargar los archivos que especificó previamente. Se recomienda que utilice el formato más reciente para mayor prestaciones y rendimiento en las descargas."
						wrapMode: Label.WordWrap
					}
				}

				Button {
					Accessible.role: Accessible.Button
					Accessible.name: text
					Accessible.description: "Haga clic aquí para crear un paquete de publicación con los elementos anteriormente especificados"
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.maximumWidth: 200
					Layout.topMargin: 30
					enabled: tituloPaquete.text.length > 0 && modeloArchivosPublicar.count > 0
					focusPolicy: Qt.StrongFocus
					hoverEnabled: true
					icon.source: "qrc:/svg/file-upload.svg"
					text: "Publicar"
					onClicked: {
						modeloPaquetes.agregarPublicacion(tituloPaquete.text.trim(), modeloArchivosPublicar, formatoArchivoDescarga.currentValue)
						vistaApilable.pop()
					}
				}
			}
		}
	}
/*
	FolderListModel {
		id: modeloDirectorio
		nameFilters: [ "*.*" ]
		showDirs: false
		showDotAndDotDot: false
		showFiles: true
		showHidden: false
	}
*/
	FileDialog {
		id: dialogoArchivos
		fileMode: FileDialog.OpenFiles
		folder: configuraciones.valor("atds3/ultimaRutaUtilizada", StandardPaths.writableLocation(StandardPaths.DownloadLocation))
		nameFilters: [ "Todos los archivos(*.*)" ]
		options: FileDialog.ReadOnly

		onAccepted: {
			for (let ruta of files) {
				modeloArchivosPublicar.append({ nombre: decodeURIComponent(ruta.toString().substring(ruta.toString().lastIndexOf("/") + 1)), ruta: ruta.toString() });
			}

			configuraciones.establecerValor("atds3/ultimaRutaUtilizada", folder.toString())
		}
	}
/*
	FolderDialog {
		id: dialogoDirectorios
		currentFolder: configuraciones.ultimaRutaUtilizada
		options: FolderDialog.ShowDirsOnly

		onAccepted: {
			modeloDirectorio.folder = dialogoDirectorios.folder
			while (modeloDirectorio.status !== FolderListModel.Ready) {
				sleep(1)
			}
			for (let i = 0; i < modeloDirectorio.count; i++) {
				modeloArchivosPublicar.append({ nombre: modeloDirectorio.get(i, "fileName"), ruta: modeloDirectorio.get(i, "fileName") });
			}

			configuraciones.ultimaRutaUtilizada = folder
		}
	}
*/
}
