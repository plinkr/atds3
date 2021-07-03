#ifndef _MODELOSONIDOS_HPP
#define _MODELOSONIDOS_HPP

#include <QSqlQueryModel>


class ModeloSonidos : public QSqlQueryModel {
	Q_OBJECT

	public:
		ModeloSonidos(QObject *padre = nullptr);

		QHash<int, QByteArray> roleNames() const;
		QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const;

		Q_INVOKABLE QVariantMap obtener(int fila) const;
};

#endif // _MODELOSONIDOS_HPP
