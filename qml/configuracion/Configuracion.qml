import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import "qrc:/qml/comun"
import "qrc:/qml/configuracion/apariencia"
import "qrc:/qml/configuracion/todus"
import "qrc:/qml/configuracion/descargas"
import "qrc:/qml/configuracion/publicaciones"
import "qrc:/qml/configuracion/notificaciones"
import "qrc:/qml/configuracion/proxy"
import "qrc:/qml/configuracion/avanzadas"
import "qrc:/qml/configuracion/informacion"


Page {
	property string nombre: "Configuracion"
	property string titulo: "Configuración"
	property alias pantallaConfiguracionTodus: pantallaConfiguracionTodus
	property alias pantallaConfiguracionDescargas: pantallaConfiguracionDescargas
	property alias pantallaConfiguracionPublicaciones: pantallaConfiguracionPublicaciones
	property alias pantallaConfiguracionProxy: pantallaConfiguracionProxy
	property alias pantallaConfiguracionAvanzadas: pantallaConfiguracionAvanzadas
	property alias pantallaConfiguracionInformacion: pantallaConfiguracionInformacion

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de las configuraciones de ATDS3"
	Accessible.description: "Aquí se muestran los accesos a las diferentes secciones de configuración de ATDS3"
	header: BarraBotones { titulo: parent.titulo }

	Flickable {
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
					id: repetidor

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
					Button {
						Accessible.role: Accessible.Button
						Accessible.name: `${model.titulo}. ${model.descripcion}`
						Layout.fillWidth: true
//						Layout.preferredHeight: tamanoIconos + 40
						clip: true
						display: AbstractButton.TextUnderIcon
						focusPolicy: Qt.StrongFocus
						hoverEnabled: true
						flat: true
						icon.height: tamanoIconos
						icon.width: tamanoIconos
						icon.source: model.icono
						spacing: 10
						text: `<h3>${model.titulo}</h3><p>${model.descripcion}</p>`
						width: listadoConfiguraciones.width

						onClicked: {
							switch (index) {
								case 0: // Estilo
									dialogoConfiguracionApariencia.open()
									break;
								case 1: // toDus
									pantallaConfiguracionTodus.mostrar()
									break;
								case 2: // Descargas
									pantallaConfiguracionDescargas.mostrar()
									break;
								case 3: // Publicaciones
									pantallaConfiguracionPublicaciones.mostrar()
									break;
								case 4: // Notificaciones
									pantallaConfiguracionNotificaciones.mostrar()
									break;
								case 5: // Proxy
									pantallaConfiguracionProxy.mostrar()
									break;
								case 6: // Avanzadas
									pantallaConfiguracionAvanzadas.mostrar()
									break;
								case 7: // Informacion
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
		visible: false
	}

	Todus {
		id: pantallaConfiguracionTodus
		visible: false
	}

	Descargas {
		id: pantallaConfiguracionDescargas
		visible: false
	}

	Publicaciones {
		id: pantallaConfiguracionPublicaciones
		visible: false
	}

	Notificaciones {
		id: pantallaConfiguracionNotificaciones
		visible: false
	}

	Proxy {
		id: pantallaConfiguracionProxy
		visible: false
	}

	Avanzadas {
		id: pantallaConfiguracionAvanzadas
		visible: false
	}

	Informacion {
		id: pantallaConfiguracionInformacion
		visible: false
	}
}
