import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionAvanzadas"
	property string titulo: "Avanzadas"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 4
	property alias avanzadasServidorAutentificacion: avanzadasServidorAutentificacion
	property alias avanzadasPuertoAutentificacion: avanzadasPuertoAutentificacion
	property alias avanzadasServidorMensajeria: avanzadasServidorMensajeria
	property alias avanzadasPuertoMensajeria: avanzadasPuertoMensajeria
	property alias avanzadasServidorS3: avanzadasServidorS3
	property alias avanzadasPuertoS3: avanzadasPuertoS3
	property alias avanzadasAgenteUsuario: avanzadasAgenteUsuario
	property alias avanzadasNumeroVersion: avanzadasNumeroVersion

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración avanzada"
	Accessible.description: "Aquí se definen opciones avanzadas que definen el comportamiento de la conexión a los servidores de toDus"
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
					columns: 4

					Subtitulo {
						Layout.columnSpan: 4
						Layout.topMargin: 20
						titulo: "Conexiones"
					}

					Label {
						Layout.columnSpan: 4
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Servidor de autentificación:"
					}
					EntradaLineaTexto {
						id: avanzadasServidorAutentificacion
						Accessible.name: "Servidor de autentificación"
						Accessible.description: "Define el nombre DNS a utilizar para la conexión al servidor de autentificación de toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						focus: true
						placeholderText: "auth.todus.cu"

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorAutentificacion", text.trim())
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						text: ":"
					}
					EntradaLineaTexto {
						id: avanzadasPuertoAutentificacion
						Accessible.name: "Puerto del servidor de autentificación"
						Accessible.description: "Define el número del puerto del servidor de autentificación"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.minimumWidth: 100
						placeholderText: "443"
						validator: IntValidator { bottom: 1; top: 65535; }

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorAutentificacionPuerto", parseInt(text.trim()))
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Servidor de mensajería:"
					}
					EntradaLineaTexto {
						id: avanzadasServidorMensajeria
						Accessible.name: "Servidor de mensajería"
						Accessible.description: "Define el nombre DNS a utilizar para la conexión al servidor de mensajería de toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						placeholderText: "im.todus.cu"

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorMensajeria", text.trim())
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						text: ":"
					}
					EntradaLineaTexto {
						id: avanzadasPuertoMensajeria
						Accessible.name: "Puerto del servidor de mensajería"
						Accessible.description: "Define el número del puerto del servidor de mensajería"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.minimumWidth: 100
						placeholderText: "1756"
						validator: IntValidator { bottom: 1; top: 65535; }

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorMensajeriaPuerto", parseInt(text.trim()))
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Servidor S3:"
					}
					EntradaLineaTexto {
						id: avanzadasServidorS3
						Accessible.name: "Servidor S3"
						Accessible.description: "Define el nombre DNS a utilizar para la conexión al servidor S3 de toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 2 : 1
						Layout.fillWidth: true
						placeholderText: "s3.todus.cu"

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorS3", text.trim())
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						text: ":"
					}
					EntradaLineaTexto {
						id: avanzadasPuertoS3
						Accessible.name: "Puerto del servidor S3"
						Accessible.description: "Define el número del puerto del servidor S3"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.minimumWidth: 100
						placeholderText: "443"
						validator: IntValidator { bottom: 1; top: 65535; }

						onEditingFinished: configuraciones.establecerValor("avanzadas/servidorS3Puerto", parseInt(text.trim()))
					}

					Subtitulo {
						Layout.columnSpan: 4
						Layout.topMargin: 40
						titulo: "Otras"
					}

					Label {
						Layout.columnSpan: 4
					}
					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Agente de usuario:"
					}
					EntradaLineaTexto {
						id: avanzadasAgenteUsuario
						Accessible.name: "Agente de usuario"
						Accessible.description: "Define el agente de usuario a utilizar en las conexiones a los servidores de toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 4 : 3
						Layout.fillWidth: true
						placeholderText: _agenteUsuarioTodus

						onEditingFinished: configuraciones.establecerValor("avanzadas/agenteUsuario", text.trim())
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Número de versión:"
					}
					EntradaLineaTexto {
						id: avanzadasNumeroVersion
						Accessible.name: "Número de versión"
						Accessible.description: "Define el número de versión a utilizar en las conexiones a los servidores de toDus"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical > 1 ? 4 : 3
						Layout.fillWidth: true
						placeholderText: _numeroVersionTodus

						onEditingFinished: configuraciones.establecerValor("avanzadas/numeroVersion", text.trim())
					}

					Subtitulo {
						Layout.columnSpan: 4
						Layout.topMargin: 40
						titulo: "Restablecimiento"
					}
					Label {
						Layout.columnSpan: 4
					}
					Button {
						Accessible.role: Accessible.Button
						Accessible.name: text
						Accessible.description: "Esta acción eliminará la base de datos y todas las configuraciones de ATDS3. Una vez eliminadas, se cerrará la aplicación. Una vez que inicie nuevamente, toda la base de dato sy configuraciones estarán creadas desde cero."
						Layout.alignment: Qt.AlignCenter
						Layout.columnSpan: 4
						flat: true
						hoverEnabled: true
						padding: 20
						spacing: 20
						icon.source: "qrc:/svg/redo.svg"
						text: "Restablecer datos de fábrica"

						onClicked: utiles.restablecerDatosFabrica()
					}
					Label {
						Layout.columnSpan: 4
						Layout.fillWidth: true
						font.pointSize: ventanaPrincipal.font.pointSize - 2
						text: "Esta acción eliminará la base de datos y todas las configuraciones de ATDS3. Al finalizar, se cerrará la aplicación. Una vez que inicie nuevamente, toda la base de datos y configuraciones estarán creadas desde cero."
						wrapMode: Label.WordWrap
					}
				}
			}
		}
	}

	Component.onCompleted: {
		avanzadasServidorAutentificacion.text = configuraciones.valor("avanzadas/servidorAutentificacion", "")
		avanzadasPuertoAutentificacion.text = configuraciones.valor("avanzadas/servidorAutentificacionPuerto", "")
		avanzadasServidorMensajeria.text = configuraciones.valor("avanzadas/servidorMensajeria", "")
		avanzadasPuertoMensajeria.text = configuraciones.valor("avanzadas/servidorMensajeriaPuerto", "")
		avanzadasServidorS3.text = configuraciones.valor("avanzadas/servidorS3", "")
		avanzadasPuertoS3.text = configuraciones.valor("avanzadas/servidorS3Puerto", "")
		avanzadasAgenteUsuario.text = configuraciones.valor("avanzadas/agenteUsuario", "")
		avanzadasNumeroVersion.text = configuraciones.valor("avanzadas/numeroVersion", "")
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		avanzadasServidorAutentificacion.forceActiveFocus()
	}
}
