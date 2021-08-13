import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "qrc:/qml/comun"
import "qrc:/qml/configuracion/apariencia"
import "qrc:/qml/configuracion/todus"
import "qrc:/qml/configuracion/descargas"
import "qrc:/qml/configuracion/publicaciones"
import "qrc:/qml/configuracion/programacion"
import "qrc:/qml/configuracion/notificaciones"
import "qrc:/qml/configuracion/proxy"
import "qrc:/qml/configuracion/avanzadas"
import "qrc:/qml/configuracion/informacion"


Page {
	property string nombre: "Configuracion"
	property string titulo: "Configuración"
	property alias deslizante: deslizante
	property alias pantallaConfiguracionTodus: pantallaConfiguracionTodus
	property alias pantallaConfiguracionDescargas: pantallaConfiguracionDescargas
	property alias pantallaConfiguracionPublicaciones: pantallaConfiguracionPublicaciones
	property alias pantallaConfiguracionProgramacion: pantallaConfiguracionProgramacion
	property alias pantallaConfiguracionNotificaciones: pantallaConfiguracionNotificaciones
	property alias pantallaConfiguracionProxy: pantallaConfiguracionProxy
	property alias pantallaConfiguracionAvanzadas: pantallaConfiguracionAvanzadas
	property alias pantallaConfiguracionInformacion: pantallaConfiguracionInformacion

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de las configuraciones de ATDS3"
	Accessible.description: "Aquí se muestran los accesos a las diferentes secciones de configuración de ATDS3"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
		id: deslizante
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		clip: true
		contentHeight: contenido.height
		ScrollIndicator.vertical: ScrollIndicator { }

		ColumnLayout {
			id: contenido
			width: parent.width

			ColumnLayout {
				id: listadoConfiguraciones
				Accessible.role: Accessible.List
				Accessible.name: "Listado de las configuraciones de ATDS3"
				Accessible.description: "Aquí se muestran los accesos a las diferentes secciones de configuración de ATDS3"
				Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
				Layout.fillWidth: true
				Layout.maximumWidth: 720
				Layout.margins: 20
				spacing: 10

				Repeater {
					model: ListModel {
						ListElement {
							icono: "qrc:/svg/theater-masks.svg"
							titulo: "Estilo"
							descripcion: "Define el estilo de la aplicación a través de temas."
						}
						ListElement {
							icono: "qrc:/png/todus.png"
							titulo: "toDus"
							descripcion: "Define las opciones del inicio de sesión en la red toDus."
						}
						ListElement {
							icono: "qrc:/svg/cloud-download-alt.svg"
							titulo: "Descargas"
							descripcion: "Define las opciones del comportamiento de las descargas."
						}
						ListElement {
							icono: "qrc:/svg/cloud-upload-alt.svg"
							titulo: "Publicaciones"
							descripcion: "Define las opciones del comportamiento de las publicaciones."
						}
						ListElement {
							icono: "qrc:/svg/business-time.svg"
							titulo: "Programación"
							descripcion: "Define las opciones de la programación de acciones."
						}
						ListElement {
							icono: "qrc:/svg/bell.svg"
							titulo: "Notificaciones"
							descripcion: "Define las opciones del comportamiento de las notificaciones."
						}
						ListElement {
							icono: "qrc:/svg/globe.svg"
							titulo: "Proxy"
							descripcion: "Define las opciones del proxy a utilizar para establecer las conexiones de red."
						}
						ListElement {
							icono: "qrc:/svg/sliders-h.svg"
							titulo: "Avanzadas"
							descripcion: "Define opciones variadas de nivel avanzado."
						}
						ListElement {
							icono: "qrc:/svg/info-circle.svg"
							titulo: "Información"
							descripcion: "Muestra información variada de la aplicación."
						}
					}
					delegate: ItemDelegate {
						Accessible.role: Accessible.ListItem
						Accessible.name: `${model.titulo}. ${model.descripcion}`
						Layout.fillWidth: true
						Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
						clip: true
						display: AbstractButton.TextBesideIcon
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						icon.height: 32
						icon.width: 32
						icon.source: model.icono
						leftPadding: 30
						rightPadding: 30
						topPadding: 20
						bottomPadding: 20
						spacing: 30
						text: `<h3>${model.titulo}</h3><span>${model.descripcion}</span>`
						width: listadoConfiguraciones.width

						onClicked: {
							switch (index) {
								case 0: // Estilo
									dialogoConfiguracionApariencia.open()
									break;
								case 1: // toDus
									vistaApilable.push(pantallaConfiguracionTodus)
									break;
								case 2: // Descargas
									vistaApilable.push(pantallaConfiguracionDescargas)
									break;
								case 3: // Publicaciones
									vistaApilable.push(pantallaConfiguracionPublicaciones)
									break;
								case 4: // Programación
									vistaApilable.push(pantallaConfiguracionProgramacion)
									break;
								case 5: // Notificaciones
									vistaApilable.push(pantallaConfiguracionNotificaciones)
									break;
								case 6: // Proxy
									vistaApilable.push(pantallaConfiguracionProxy)
									break;
								case 7: // Avanzadas
									vistaApilable.push(pantallaConfiguracionAvanzadas)
									break;
								case 8: // Informacion
									vistaApilable.push(pantallaConfiguracionInformacion)
									break;
								default:
									break;
							}
						}
					}
				}
			}
		}
	}

	Apariencia {
		id: dialogoConfiguracionApariencia
	}

	Component {
		id: pantallaConfiguracionTodus

		Todus {}
	}

	Component {
		id: pantallaConfiguracionDescargas

		Descargas {}
	}

	Component {
		id: pantallaConfiguracionPublicaciones

		Publicaciones {}
	}

	Component {
		id: pantallaConfiguracionProgramacion

		Programacion {}
	}

	Component {
		id: pantallaConfiguracionNotificaciones

		Notificaciones {}
	}

	Component {
		id: pantallaConfiguracionProxy

		Proxy {}
	}

	Component {
		id: pantallaConfiguracionAvanzadas

		Avanzadas {}
	}

	Component {
		id: pantallaConfiguracionInformacion

		Informacion {}
	}

	Component.onCompleted: {
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
	}
}
