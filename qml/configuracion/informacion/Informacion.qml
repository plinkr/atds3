import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionInformacion"
	property string titulo: "Información"

	function mostrar() {
		vistaApilable.push(this)
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de información de la aplicación"
	Accessible.description: "Aquí se muestran informaciones variadas sobre la aplicación ATDS3"
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

				ColumnLayout {
					Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
					Layout.fillWidth: true

					Image {
						Layout.alignment: Qt.AlignCenter
						sourceSize.height: tamanoIconos === 48 ? 256 : 128
						sourceSize.width: tamanoIconos === 48 ? 256 : 128
						source: "qrc:/svg/atds3.svg"
						fillMode: Image.PreserveAspectFit
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
						Layout.topMargin: 30
						horizontalAlignment: Qt.AlignHCenter
//						color: Material.accent
						font.bold: true
						font.pointSize: ventanaPrincipal.font.pointSize + 4
						text: _aplicacionTitulo
						wrapMode: Label.WordWrap
					}
                    Label {
						Layout.fillWidth: true
						horizontalAlignment: Qt.AlignHCenter
						font.bold: true
						text: `v${_aplicacionVersion}`
						wrapMode: Label.WordWrap
					}

                    Label {
						Accessible.role: Accessible.Paragraph
						Accessible.name: text
						Accessible.description: "Texto informativo sobre ATDS3"
						Layout.fillWidth: true
						Layout.topMargin: 30
						font.pointSize: ventanaPrincipal.font.pointSize + 1
						text: {
							return `<p><i>ATDS3</i> es una aplicación para escritorio y móviles que automatiza el proceso de publicación y descarga de archivos hacia/desde los servidores de la red toDus (S3).</p>
<p><i>ATDS3</i> posee las siguientes características:</p>
<ul>
  <li><b>Multiplataforma</b>: Puede ser utilizado en cualquier sistema operativo que en donde la librería Qt pueda funcionar: UNIX (FreeBSD, NetBSD, OpenBSD), Linux, Android, macOS, iOS, Windows.</li>
  <li><b>Fácil de usar</b>: Es fácil de usar gracias a la intuitiva interfaz de usuario que posee.</li>
  <li><b>Configurable</b>: Ofrece diversas opciones configurables que definen el comportamiento de diversas secciones.</li>
  <li><b>Completamente asincrónico</b>: Es totalmente asincrónico, por lo que podrá realizar varias operaciones de publicaciones y descargas al mismo tiempo.</li>
  <li><b>Ajustado al protocolo de red de toDus</b>: Está ajustado lo mayormente posible al protocolo de red que utiliza toDus, incluyendo inicios de sesión partiendo del número telefónico.</li>
  <li><b>Inteligente</b>: Tiene escrito código para comportarse de forma inteligente dependiendo de la situación de la red.</li>
</ul>`
						}
						wrapMode: Label.WordWrap
					}
                    Label {
						Layout.fillWidth: true
						Layout.topMargin: 20
						font.pointSize: ventanaPrincipal.font.pointSize + 1
                        text: "<p><b>Redes sociales</b></p><p><br/><i>Telegram: <a href=\"http://t.me/atds3\">https://t.me/atds3</a></i></p>"
                        wrapMode: Label.WordWrap

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                        }

                        onLinkActivated: Qt.openUrlExternally(link)
					}
                }
			}
		}
	}
}