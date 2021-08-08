import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionTodusPPFInformacion"
	property string titulo: "Programa Piscina de Fichas"

	function mostrar() {
		vistaApilable.push(this)
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de información del Programa de Fichas de ATDS3"
	Accessible.description: "Aquí se muestran informaciones sobre el Programa de Fichas de ATDS3"
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

				Image {
					Layout.fillWidth: true
					fillMode: Image.PreserveAspectFit
					sourceSize.height: 80
					sourceSize.width: 80
					source: "qrc:/svg/atom.svg"
				}

				Label {
					Accessible.role: Accessible.Paragraph
					Accessible.name: text
					Accessible.description: "Texto informativo el Programa de Fichas de ATDS3"
					Layout.alignment: Qt.AlignLeft
					Layout.topMargin: 40
					Layout.fillWidth: true
					text: `<h3>¿Qué es el Programa Piscina de Fichas de ATDS3?</h3>
<p>El <b>Programa Piscina de Fichas de ATDS3</b> es un mecanismo que le permite a todos los usuarios participantes, aportar su ficha de acceso de la red toDus y utilizar fichas de accesos de otros usuarios participantes, en modalidad rotatoria, una vez que haya alcanzado el límite de publicaciones/descargas, logrando así, evadir de forma efectiva las limitaciones impuestas en los servidores de toDus.</p>
<h3>¿Qué es la Piscina Local?</h3>
<p>La <b>Piscina Local</b> es un listado en donde puede almacenar fichas de acceso que obtenga por otros medios, para así, crear una piscina de fichas para su uso privado. Puede utilizar la Piscina Local y desactivar el uso del servidor de PPF de Internet, para no compartir su ficha de acceso y, aún así, disfrutar del uso de fichas rotatorias.</p>
<h3>¿Cómo puede el Programa Piscina de Fichas de ATDS3 ayudar a la Comunidad S3 Cubana?</h3>
<p>Utilizando el <b>Programa Piscina de Fichas de ATDS3</b> todos podrán utilizar las fichas de acceso de otros usuarios participantes una vez alcanzado el límite de publicaciones/descargas en su cuenta, y así, continuar publicando o descargando contenido. Esto es muy útil, mayormente para los creadores de contenidos, ya que, utilizando las fichas de acceso de los demás, podrán publicar bastante contenido que se podrán descargar posteriormente de forma gratuita. <i>Sin los creadores de contenidos, la Comunidad S3 Cubana verá llegar su fin.</i></p>
<h3>¿Cuáles informaciones se transmiten y hacia dónde?</h3>
<p>Lamentablemente no se puede utilizar un servidor ubicado en nuestro país debido a que puede traer consecuencias negativas, por lo que se utiliza un servidor en Internet para establecer la conexión al <b>Programa Piscina de Fichas de ATDS3</b>.</p>
<p>Las únicas informaciones que se transmiten son:</p>
<ol>
  <li>Ficha de acceso del usuario.</li>
  <li>Tiempo de expiración de la ficha de acceso.</li>
</ol>
<p>El consumo del uso de este programa es mínimo y aporta muchos beneficios.</p>`
					wrapMode: Label.WordWrap
				}
			}
		}
	}
}
