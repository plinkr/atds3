#ifndef MODELOENTRADAS_HPP
#define MODELOENTRADAS_HPP

#include <QSqlTableModel>


class QSqlDatabase;

class ModeloEntradas : public QSqlTableModel {
	Q_OBJECT

	public:
		ModeloEntradas(QObject *padre = nullptr, const QSqlDatabase &baseDatos = QSqlDatabase());

		void eliminarFila(int fila);
};

#endif // MODELOENTRADAS_HPP
