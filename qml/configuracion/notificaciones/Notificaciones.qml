import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import cu.atds3.librerias 1.6
import "qrc:/qml/comun"


Page {
	property string nombre: "ConfiguracionNotificaciones"
	property string titulo: "Notificaciones"
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias temaSonidos: temaSonidos
	property alias notificacionConexionDisponibleTodus: notificacionConexionDisponibleTodus
	property alias notificacionConexionPerdidaTodus: notificacionConexionPerdidaTodus
	property alias notificacionInicializacionPaquete: notificacionInicializacionPaquete
	property alias notificacionFinalizacionExitosaPaquete: notificacionFinalizacionExitosaPaquete
	property alias notificacionFinalizacionErroneaPaquete: notificacionFinalizacionErroneaPaquete
	property alias notificacionInicializacionTarea: notificacionInicializacionTarea
	property alias notificacionFinalizacionExitosaTarea: notificacionFinalizacionExitosaTarea
	property alias notificacionFinalizacionErroneaTarea: notificacionFinalizacionErroneaTarea

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de configuración de las notificaciones"
	Accessible.description: "Aquí se definen las opciones que definen el comportamiento de las notificaciones"
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

					Label {
						Layout.columnSpan: 2
					}

					Label {
						Layout.alignment: alineacionColumnasVistaVertical
						Layout.columnSpan: columnasIntegradasVistaVertical
						text: "Tema de sonidos:"
					}
					ComboBox {
						id: temaSonidos
						Accessible.role: Accessible.ComboBox
						Accessible.name: "Tema de los sonidos"
						Accessible.description: "Define el tema de los sonidos a utilizar para las notificaciones audibles de los eventos de ATDS3"
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true
						hoverEnabled: true
						focusPolicy: Qt.WheelFocus
						textRole: "titulo"
						valueRole: "valor"
						wheelEnabled: true
						model: ListModel {
							ListElement { titulo: "Encantado"; valor: "encantado" }
						}
					}

					Label {
						Layout.columnSpan: columnasIntegradasVistaVertical
						visible: columnasIntegradasVistaVertical === 1
					}
					ColumnLayout {
						Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
						Layout.columnSpan: columnasIntegradasVistaVertical
						Layout.fillWidth: true

						Notificacion {
							id: notificacionConexionDisponibleTodus
							titulo: "Conexión disponible a toDus"
							descripcion: "Define si se notifica visualmente cuando se detecte que se pudo establecer conexión con los servidores de toDus."
							sonido: "conexion-disponible-todus.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualConexionDisponibleTodus", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleConexionDisponibleTodus", audible)
						}

						Notificacion {
							id: notificacionConexionPerdidaTodus
							titulo: "Conexión perdida con toDus"
							descripcion: "Define si se notifica visualmente cuando se detecte que se perdió la conexión con los servidores de toDus."
							sonido: "conexion-perdida-todus.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualConexionPerdidaTodus", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleConexionPerdidaTodus", audible)
						}

						Notificacion {
							id: notificacionInicializacionPaquete
							titulo: "Inicializacion de un paquete"
							descripcion: "Define si se notifica visualmente cuando se inicie un paquete."
							sonido: "incializacion-paquete.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualInicializacionPaquete", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleInicializacionPaquete", audible)
						}

						Notificacion {
							id: notificacionFinalizacionExitosaPaquete
							titulo: "Finalización exitosa de un paquete"
							descripcion: "Define si se notifica visualmente cuando se finalice exitosamente un paquete."
							sonido: "finalizacion-exitosa-paquete.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualFinalizacionExitosaPaquete", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleFinalizacionExitosaPaquete", audible)
						}

						Notificacion {
							id: notificacionFinalizacionErroneaPaquete
							titulo: "Finalización errónea de un paquete"
							descripcion: "Define si se notifica visualmente cuando se finalice erróneamente un paquete."
							sonido: "finalizacion-erronea-paquete.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualFinalizacionErroneaPaquete", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleFinalizacionErroneaPaquete", audible)
						}

						Notificacion {
							id: notificacionInicializacionTarea
							titulo: "Inicializacion de una tarea"
							descripcion: "Define si se notifica visualmente cuando se inicie una tarea."
							sonido: "incializacion-tarea.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualInicializacionTarea", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleInicializacionTarea", audible)
						}

						Notificacion {
							id: notificacionFinalizacionExitosaTarea
							titulo: "Finalización exitosa de una tarea"
							descripcion: "Define si se notifica visualmente cuando se finalice exitosamente una tarea."
							sonido: "finalizacion-exitosa-tarea.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualFinalizacionExitosaTarea", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleFinalizacionExitosaTarea", audible)
						}

						Notificacion {
							id: notificacionFinalizacionErroneaTarea
							titulo: "Finalización errónea de una tarea"
							descripcion: "Define si se notifica visualmente cuando se finalice erróneamente una tarea."
							sonido: "finalizacion-erronea-tarea.mp3"

							onVisualCambiado: configuraciones.establecerValor("notificaciones/visualFinalizacionErroneaTarea", visual)
							onAudibleCambiado: configuraciones.establecerValor("notificaciones/audibleFinalizacionErroneaTarea", audible)
						}
					}
				}
			}
		}
	}

	Component.onCompleted: {
		temaSonidos.currentIndex = temaSonidos.indexOfValue(configuraciones.valor("notificaciones/temaSonidos", "encantado"))
		notificacionConexionDisponibleTodus.visual = configuraciones.valor("notificaciones/visualConexionDisponibleTodus", false)
		notificacionConexionDisponibleTodus.audible = configuraciones.valor("notificaciones/audibleConexionDisponibleTodus", false)
		notificacionConexionPerdidaTodus.visual = configuraciones.valor("notificaciones/visualConexionPerdidaTodus", true)
		notificacionConexionPerdidaTodus.audible = configuraciones.valor("notificaciones/audibleConexionPerdidaTodus", true)
		notificacionInicializacionPaquete.visual = configuraciones.valor("notificaciones/visualInicializacionPaquete", false)
		notificacionInicializacionPaquete.audible = configuraciones.valor("notificaciones/audibleInicializacionPaquete", false)
		notificacionFinalizacionExitosaPaquete.visual = configuraciones.valor("notificaciones/visualFinalizacionExitosaPaquete", false)
		notificacionFinalizacionExitosaPaquete.audible = configuraciones.valor("notificaciones/audibleFinalizacionExitosaPaquete", false)
		notificacionFinalizacionErroneaPaquete.visual = configuraciones.valor("notificaciones/visualFinalizacionErroneaPaquete", true)
		notificacionFinalizacionErroneaPaquete.audible = configuraciones.valor("notificaciones/audibleFinalizacionErroneaPaquete", true)
		notificacionInicializacionTarea.visual = configuraciones.valor("notificaciones/visualInicializacionTarea", false)
		notificacionInicializacionTarea.audible = configuraciones.valor("notificaciones/audibleInicializacionTarea", false)
		notificacionFinalizacionExitosaTarea.visual = configuraciones.valor("notificaciones/visualFinalizacionExitosaTarea", false)
		notificacionFinalizacionExitosaTarea.audible = configuraciones.valor("notificaciones/audibleFinalizacionExitosaTarea", false)
		notificacionFinalizacionErroneaTarea.visual = configuraciones.valor("notificaciones/visualFinalizacionErroneaTarea", true)
		notificacionFinalizacionErroneaTarea.audible = configuraciones.valor("notificaciones/audibleFinalizacionErroneaTarea", true)
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		temaSonidos.forceActiveFocus()
	}
}
