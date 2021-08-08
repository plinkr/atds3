import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtMultimedia 5.12

RowLayout {
	required property string titulo
	required property string descripcion
	required property string sonido
	property bool visual: false
	property bool audible: false

	signal visualCambiado()
	signal audibleCambiado()

	Accessible.role: Accessible.List
	Accessible.name: titulo
	Accessible.description: descripcion
	width: parent.width

	Label {
		Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		Layout.fillWidth: true
		text: titulo
	}
	Button {
		Accessible.role: Accessible.Button
		Accessible.name: visual === true ? `Notificación visual activada. Haga clic aquí para desactivar la notificación` : `Notificación visual desactivada. Haga clic aquí para activar la notificación`
		flat: true
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.color: visual === true ? Material.accentColor : Material.iconDisabledColor
		icon.height: 16
		icon.width: 16
		icon.source: visual === true ? "qrc:/svg/eye.svg" : "qrc:/svg/eye-slash.svg"

		onClicked: {
			visual = !visual
			visualCambiado()
		}
	}
	Button {
		Accessible.role: Accessible.Button
		Accessible.name: visual === true ? `Notificación audible activada. Haga clic aquí para desactivar la notificación` : `Notificación audible desactivada. Haga clic aquí para activar la notificación`
		flat: true
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.color: audible === true ? Material.accentColor : Material.iconDisabledColor
		icon.height: 16
		icon.width: 16
		icon.source: audible === true ? "qrc:/svg/volume-up.svg" : "qrc:/svg/volume-mute.svg"

		onClicked: {
			audible = !audible
			audibleCambiado()
		}
	}
	ToolSeparator {
		leftPadding: 0
		rightPadding: 0
	}
	Button {
		Accessible.role: Accessible.Button
		Accessible.name: "Reproducir el audio de la notificación"
		flat: true
		focusPolicy: Qt.StrongFocus
		hoverEnabled: true
		icon.height: 16
		icon.width: 16
		icon.source: reproductor.playbackState === Audio.PlayingState ? "qrc:/svg/pause.svg" : "qrc:/svg/play.svg"

		onClicked: reproductor.play()
	}
	Audio {
		id: reproductor
		audioRole: Audio.NotificationRole
		source: `qrc:/mp3/${configuraciones.valor("notificaciones/temaSonidos", "encantado")}/${sonido}`
	}
}
