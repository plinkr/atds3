import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "qrc:/qml/comun"


Page {
	property string nombre: "AgregarCategoria"
	property string titulo: "Crear categoría"
	property bool creando: true
	property int alineacionColumnasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
	property int columnasIntegradasVistaVertical: ventanaPrincipal.orientacionHorizontal === true ? 1 : 2
	property alias categoriaTitulo: categoriaTitulo
	property alias categoriaIcono: categoriaIcono
//	property alias categoriaCanalTodus: categoriaCanalTodus
//	property alias categoriaDescargarAutomaticamente: categoriaDescargarAutomaticamente

	function crear() {
		titulo = "Crear categoría"
		creando = true
		categoriaTitulo.text = ''
		busquedaIconos.text = ''
		modeloIconoCategorias.eliminarFiltros()
		categoriaIcono.currentIndex = 0
//		categoriaCanalTodus.text = ''
//		categoriaDescargarAutomaticamente.checked = false
	}

	function editar(indice) {
		let categoria = modeloCategorias.obtener(indice)

		titulo = "Editar categoría"
		creando = false
		categoriaTitulo.text = categoria.titulo
		busquedaIconos.text = categoria.icono.substr(0, categoria.icono.length - 4)
		modeloIconoCategorias.filtrar(busquedaIconos.text)
		categoriaIcono.currentIndex = 0
//		categoriaCanalTodus.text = categoria.canalTodus
//		categoriaDescargarAutomaticamente.checked = categoria.descargarAutomaticamente
	}

	Accessible.role: Accessible.Pane
	Accessible.name: "Pantalla de creación y edición de una categoría"
	Accessible.description: "Aquí se muestran los elementos necesarios para crear y editar una categoría"
	id: panel
	header: BarraBotones {
		titulo: parent.titulo

		RowLayout {
			anchors.fill: parent

			Label {
				Layout.fillWidth: true
			}

			ToolButton {
				Accessible.role: Accessible.Button
				Accessible.name: "Aplicar los cambios"
				Accessible.description: "Haga clic aquí para aplicar los cambios de creación o edición de una categoría"
				icon.source: "qrc:/svg/check.svg"
				focusPolicy: Qt.StrongFocus
				hoverEnabled: true
				enabled: categoriaTitulo.text.length > 0 && categoriaIcono.currentIndex !== -1

				onClicked: {
					let icono = modeloIconoCategorias.obtener(categoriaIcono.currentIndex)

					if (creando === true) {
//						modeloCategorias.agregar(categoriaTitulo.text, icono.ruta, categoriaCanalTodus.text, categoriaDescargarAutomaticamente.checked)
						modeloCategorias.agregar(categoriaTitulo.text.trim(), icono.ruta, "", false)
                    } else {
//						modeloCategorias.actualizar(listadoCategorias.currentIndex, categoriaTitulo.text, icono.ruta, categoriaCanalTodus.text, categoriaDescargarAutomaticamente.checked)
						modeloCategorias.actualizar(listadoCategorias.currentIndex, categoriaTitulo.text.trim(), icono.ruta, "", false)
                    }

					vistaApilable.pop();

					if (creando === true) {
						listadoCategorias.currentIndex = modeloCategorias.rowCount() - 1
					}
				}
			}
		}
	}

	Flickable {
		id: deslizante
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		contentHeight: contenido.height
		ScrollIndicator.vertical: ScrollIndicator { }

		ColumnLayout {
			id: contenido
			width: parent.width

			GridLayout {
				Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.maximumWidth: 720
				Layout.margins: 20
				columns: 2
				columnSpacing: 10

				Label {
					Layout.columnSpan: 2
					font.bold: true
					font.pointSize: ventanaPrincipal.font.pointSize + 2
					text: "Categoría"
				}

				Label {
					Layout.columnSpan: 2
					Layout.topMargin: 5
				}
				Label {
					Layout.alignment: alineacionColumnasVistaVertical
					Layout.columnSpan: columnasIntegradasVistaVertical
					text: "Título:"
				}
				EntradaLineaTexto {
					id: categoriaTitulo
					Accessible.name: "Título de la categoría"
					Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					focus: true
					placeholderText: "Título de la categoría..."
				}

				Label {
					Layout.alignment: alineacionColumnasVistaVertical
					Layout.columnSpan: columnasIntegradasVistaVertical
					text: "Icono:"
				}
				EntradaLineaTexto {
					id: busquedaIconos
					Accessible.name: "Ícono de la categoría"
					Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					placeholderText: "Criterio de filtrado..."

					onTextEdited: modeloIconoCategorias.filtrar(busquedaIconos.text)
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillHeight: columnasIntegradasVistaVertical === 1 ? true : false
					visible: columnasIntegradasVistaVertical === 1
				}
				GridView {
					id: categoriaIcono
					Accessible.role: Accessible.List
					Accessible.name: "Listado de íconos"
					Accessible.description: "Aquí puede seleccionar el ícono de su agrado para la categoría"
					Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					Layout.minimumHeight: 240
					Layout.preferredHeight: panel.height - (orientacionHorizontal === true ? 260 : 290)
					boundsBehavior: Flickable.StopAtBounds
					clip: true
					cellHeight: 54
					cellWidth: 70
					highlightFollowsCurrentItem: true
					model: modeloIconoCategorias
					delegate: ItemDelegate {
						clip: true
						highlighted: GridView.isCurrentItem
						hoverEnabled: true
						icon.height: 32
						icon.width: 32
						icon.source: `qrc:/svg/${model.ruta}`
						spacing: 5

						onPressed: categoriaIcono.currentIndex = index
					}

					ScrollIndicator.vertical: ScrollIndicator {}
				}
/*
				Label {
					Layout.columnSpan: 2
					Layout.topMargin: 30
					font.bold: true
					font.pointSize: ventanaPrincipal.font.pointSize + 2
					text: "toDus"
				}

				Label {
					Layout.columnSpan: 2
					Layout.topMargin: 5
				}
				Label {
					Layout.alignment: alineacionColumnasVistaVertical
					Layout.columnSpan: columnasIntegradasVistaVertical
					text: "Canal de toDus:"
				}
				TextField {
					id: categoriaCanalTodus
					hoverEnabled: true
					Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					placeholderText: "https://chat.todus.cu/..."
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					visible: columnasIntegradasVistaVertical === 1
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					font.pointSize: ventanaPrincipal.font.pointSize - 2
					text: "Define el canal de toDus que se debe asociar a esta categoría. ATDS3 monitorizará los mensajes enviados al canal en búsqueda de archivos."
					wrapMode: Label.WordWrap
				}

				Label {
					Layout.columnSpan: 2
					Layout.topMargin: 5
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					visible: columnasIntegradasVistaVertical === 1
				}
				Switch {
					id: categoriaDescargarAutomaticamente
					hoverEnabled: true
					Layout.alignment: Qt.AlignLeft | Qt.AlignBaseline
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					text: "Descargar archivos automáticamente"
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					visible: columnasIntegradasVistaVertical === 1
				}
				Label {
					Layout.columnSpan: columnasIntegradasVistaVertical
					Layout.fillWidth: true
					font.pointSize: ventanaPrincipal.font.pointSize - 2
					text: "Define si se deben descargar los archivos enviado al canal de toDus previamente definido de forma automática."
					wrapMode: Label.WordWrap
				}
*/
			}
		}
	}

	Component.onCompleted: {
		deslizante.contentY = 1
		deslizante.flick(0, 1)
		deslizante.contentY = 0
		categoriaTitulo.forceActiveFocus()
	}
}
