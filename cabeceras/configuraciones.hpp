#ifndef CONFIGURACIONES_HPP
#define CONFIGURACIONES_HPP

#include <QSettings>


class Configuraciones : public QObject {
	Q_OBJECT

	public:
		Configuraciones(QObject *padre = nullptr);

		Q_INVOKABLE bool existente(const QString &llave);
		Q_INVOKABLE QVariant valor(const QString &llave, const QVariant &valorPredeterminado = QVariant());
		Q_INVOKABLE void establecerValor(const QString &llave, const QVariant &valor);
		Q_INVOKABLE void eliminar(const QString &llave);

	private:
		QSettings _configuraciones;
};

#endif // HTTP2_HPP
