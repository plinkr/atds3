#ifndef BASEDATOS_HPP
#define BASEDATOS_HPP

#include <QThread>
#include <QSqlQuery>
#include <QSqlRecord>


class BaseDatos : public QThread
{
	Q_OBJECT

	public:
		BaseDatos(QObject *padre = nullptr);

		void run() override;

	public slots:
		void ejecutarInstruccion(const QString &instruccion, QSqlQuery *resultados);

	private:
		/**
		 * @brief Inicializa la base de datos
		 */
		void inicializarBaseDatos();
};

#endif // BASEDATOS_HPP
