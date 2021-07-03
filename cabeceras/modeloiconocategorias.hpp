#ifndef _MODELOICONOCATEGORIAS_HPP
#define _MODELOICONOCATEGORIAS_HPP

#include <QSqlQueryModel>


class ModeloIconoCategorias : public QSqlQueryModel {
	Q_OBJECT

	public:
		ModeloIconoCategorias(QObject *padre = nullptr);

		QHash<int, QByteArray> roleNames() const;
		QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const;

		Q_INVOKABLE QVariantMap obtener(int fila) const;
		Q_INVOKABLE void filtrar(const QString &criterio);
		Q_INVOKABLE void eliminarFiltros();
};

#endif // _MODELOICONOCATEGORIAS_HPP
