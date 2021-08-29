import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionDonar"
	property string titulo: "Realizar una donación"

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de información para realizar una donación"
	Accessible.description: "Aquí se muestran informaciones requeridas para realizar una donación al creador de la aplicación ATDS3"
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
						sourceSize.height: 128
						sourceSize.width: 128
						source: "qrc:/svg/donate.svg"
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
						Layout.topMargin: 40
						horizontalAlignment: Qt.AlignHCenter
						text: '<h2>Gracias por utilizar esta hermosa y útil aplicación</h2>'
						wrapMode: Label.WordWrap
					}
					Label {
						Accessible.role: Accessible.Paragraph
						Accessible.name: text
						Accessible.description: "Texto informativo sobre cómo realizar una donación al creador de la aplicación ATDS3"
						Layout.fillWidth: true
						Layout.topMargin: 20
						font.pointSize: ventanaPrincipal.font.pointSize + 1
						text: {
							return `<p>Leinier Cruz Salfrán &lt;leiniercs@gmail.com&gt;, creador de la aplicación <i>ATDS3</i>, solicita a todos el apoyo y contribución para continuar entregándole esta herramienta útil para publicar y descargar contenido hacia/desde la red toDus (S3).</p>
<p>El proceso de creación de una aplicación es costoso y, la mayoría de las veces, cuando la aplicación es gratuita y no se cuenta con un inversor, se torna casi imposible mantener y continuar entregando un producto de calidad. Es por ello que se enumera un listado de vías para que usted pueda contribuir con una donación monetaria, cualquiera que sea la cantidad que esté a su alcance, para así, garantizar el contínuo desarrollo de <i>ATDS3</i>.</p>
<h3>Enzona</h3>
<p><b>Usuario:</b> lsalfran33</p>`
						}
						wrapMode: Label.WordWrap
					}
					Image {
						sourceSize.height: 300
						sourceSize.width: 300
						source: "qrc:/png/enzona_lsalfran33.png"
						fillMode: Image.PreserveAspectFit
					}
					Label {
						Layout.fillWidth: true
						Layout.topMargin: 40
						text: `<h3>Transferencia móvil</h3>
<p><b>Número:</b> +53 (5) 513-8989</p>`
					}
                }
			}
		}
	}

	Component.onCompleted: {
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
	}
}
