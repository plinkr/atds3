#ifndef _MODELOCATEGORIAS_HPP
#define _MODELOCATEGORIAS_HPP

#include <QSqlTableModel>


class ModeloCategorias : public QSqlTableModel {
	Q_OBJECT

	public:
		ModeloCategorias(QObject *padre = nullptr);
		~ModeloCategorias();

		QHash<int, QByteArray> roleNames() const;
		QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const;
		bool setData(const QModelIndex &indice, const QVariant &valor, int rol = Qt::EditRole);

		Q_INVOKABLE QVariantMap obtener(int fila) const;
		Q_INVOKABLE QVariantMap obtenerDesdeId(int id) const;
		Q_INVOKABLE void agregar(const QString &titulo, const QString &icono, const QString &canalTodus, bool descargarAutomaticamente);
		Q_INVOKABLE void actualizar(int fila, const QString &titulo, const QString &icono, const QString &canalTodus, bool descargarAutomaticamente);
		Q_INVOKABLE void eliminar(int fila);
};

#endif // _MODELOCATEGORIAS_HPP
