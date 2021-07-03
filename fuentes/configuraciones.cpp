#include "configuraciones.hpp"
#include "main.hpp"


Configuraciones::Configuraciones(QObject *padre)
	: QObject(padre) {
}

bool Configuraciones::existente(const QString &llave) {
	return _configuraciones.contains(llave);
}

QVariant Configuraciones::valor(const QString &llave, const QVariant &valorPredeterminado) {
	return _configuraciones.value(llave, valorPredeterminado);
}

void Configuraciones::establecerValor(const QString &llave, const QVariant &valor) {
	if (valor.type() == QVariant::Type::String) {
		if (valor.toString().isEmpty() == true) {
			_configuraciones.remove(llave);
			return;
		}
	}

	switch (valor.type()) {
		case QVariant::Type::Bool:
			_configuraciones.setValue(llave, valor.toInt());
			break;
		case QVariant::Type::ByteArray:
			_configuraciones.setValue(llave, valor.toByteArray());
			break;
		case QVariant::Type::Double:
			_configuraciones.setValue(llave, valor.toDouble());
			break;
		case QVariant::Type::Int:
			_configuraciones.setValue(llave, valor.toInt());
			break;
		case QVariant::Type::LongLong:
			_configuraciones.setValue(llave, valor.toLongLong());
			break;
		case QVariant::Type::UInt:
			_configuraciones.setValue(llave, valor.toUInt());
			break;
		case QVariant::Type::ULongLong:
			_configuraciones.setValue(llave, valor.toULongLong());
			break;
		case QVariant::Type::String:
			_configuraciones.setValue(llave, valor.toString());
			break;
		default:
			_configuraciones.setValue(llave, valor);
			break;
	}
}

void Configuraciones::eliminar(const QString &llave) {
	_configuraciones.remove(llave);
}
