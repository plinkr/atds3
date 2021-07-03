#ifndef _MODELOTAREAS_HPP
#define _MODELOTAREAS_HPP

#include <QSqlTableModel>


class ModeloTareas : public QSqlTableModel {
	Q_OBJECT

	public:
		ModeloTareas(QObject *padre = nullptr);
		~ModeloTareas();

		QHash<int, QByteArray> roleNames() const;
		QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const;
		bool setData(const QModelIndex &indice, const QVariant &valor, int rol = Qt::EditRole);

		Q_INVOKABLE void establecerFiltroPaquete(int paquete);
		Q_INVOKABLE QVariantMap obtener(int fila) const;
		QVariantMap obtenerDesdeId(int id) const;
		void actualizarEnlaces(int id, const QString &enlace, const QString &enlaceFirmado);
		void actualizarEnlaceFirmado(int id, const QString &enlace);
		void actualizarEstado(int id, int estado);
		void actualizarCampo(int fila, const QString &campo, const QVariant &valor);
		void actualizarCampos(int fila, const QVariantMap &campos);
		bool corregirFila(int &fila, int id);
};

#endif // _MODELOTAREAS_HPP
