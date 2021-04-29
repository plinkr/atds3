#ifndef DESCARGA_HPP
#define DESCARGA_HPP

#include "modeloentradas.hpp"
#include <QThread>


class Descarga : public QThread {
	Q_OBJECT

	public:
		Descarga(int id, const QString &enlace, const QString &nombre, const QString &ruta, QObject *padre = nullptr);

		void run() override;

	private:
		int _id;
		QString _enlace;
		QString _nombre;
		QString _ruta;
};

#endif // DESCARGA_HPP


