#include "configuraciones.hpp"
#include "main.hpp"
#include <openssl/evp.h>


Configuraciones::Configuraciones(QObject *padre)
	: QObject(padre) {
}

bool Configuraciones::existente(const QString &llave) {
	return _configuraciones.contains(llave);
}

QVariant Configuraciones::valor(const QString &llave, const QVariant &valorPredeterminado) {
	QVariant resultado = _configuraciones.value(llave, valorPredeterminado);

	if (llave == "todus/telefono" || llave == "proxy/contrasena") {
		std::string contrasena = { 0x33, 0x65, 0x62, 0x38, 0x34, 0x64, 0x62, 0x32, 0x32, 0x37, 0x39, 0x63, 0x35, 0x34, 0x30, 0x66, 0x66, 0x38, 0x38, 0x37, 0x39, 0x66, 0x39, 0x64, 0x33, 0x65, 0x38, 0x30, 0x63, 0x30, 0x61, 0x34 };

		resultado = descifrarTexto(resultado.toByteArray(), QString::fromStdString(contrasena));
	}

	return resultado;
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
			if (llave == "todus/telefono" || llave == "proxy/contrasena") {
				std::string contrasena = { 0x33, 0x65, 0x62, 0x38, 0x34, 0x64, 0x62, 0x32, 0x32, 0x37, 0x39, 0x63, 0x35, 0x34, 0x30, 0x66, 0x66, 0x38, 0x38, 0x37, 0x39, 0x66, 0x39, 0x64, 0x33, 0x65, 0x38, 0x30, 0x63, 0x30, 0x61, 0x34 };

				_configuraciones.setValue(llave, cifrarTexto(valor.toString(), QString::fromStdString(contrasena)));
			} else {
				_configuraciones.setValue(llave, valor.toByteArray());
			}
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
			if (llave == "todus/telefono" || llave == "proxy/contrasena") {
				std::string contrasena = { 0x33, 0x65, 0x62, 0x38, 0x34, 0x64, 0x62, 0x32, 0x32, 0x37, 0x39, 0x63, 0x35, 0x34, 0x30, 0x66, 0x66, 0x38, 0x38, 0x37, 0x39, 0x66, 0x39, 0x64, 0x33, 0x65, 0x38, 0x30, 0x63, 0x30, 0x61, 0x34 };

				_configuraciones.setValue(llave, cifrarTexto(valor.toString(), QString::fromStdString(contrasena)));
			} else {
				_configuraciones.setValue(llave, valor.toString());
			}
			break;
		default:
			_configuraciones.setValue(llave, valor);
			break;
	}
}

void Configuraciones::eliminar(const QString &llave) {
	_configuraciones.remove(llave);
}

/**
 * @brief Cifra un texto
 */
QByteArray Configuraciones::cifrarTexto(const QString &datos, const QString &contrasena) {
	std::string datosDescifrados = datos.toStdString();
	int longitudDatosDescifrados1 = 0;
	int longitudDatosDescifrados2 = 0;
	std::string vectorInicializacion = { 0x34, 0x61, 0x30, 0x31, 0x62, 0x35, 0x61, 0x38, 0x34, 0x31, 0x30, 0x38, 0x38, 0x65, 0x31, 0x30 };
	std::string contrasenaStd = contrasena.toStdString();
	EVP_CIPHER_CTX *evpContextoCifrado;
	std::string resultados;

	evpContextoCifrado = EVP_CIPHER_CTX_new();
	if (EVP_EncryptInit_ex(evpContextoCifrado, EVP_aes_256_cbc(), nullptr, (const unsigned char *)&contrasenaStd[0], (const unsigned char *)&vectorInicializacion[0]) != -1) {
		resultados.resize(512);

		if (EVP_EncryptUpdate(evpContextoCifrado, (unsigned char *)&resultados[0], &longitudDatosDescifrados1, (const unsigned char *)&datosDescifrados[0], static_cast<int>(datosDescifrados.size())) != -1) {
			if (EVP_EncryptFinal_ex(evpContextoCifrado, (unsigned char *)&resultados[0] + longitudDatosDescifrados1, &longitudDatosDescifrados2) != -1) {
				resultados.resize(longitudDatosDescifrados1 + longitudDatosDescifrados2);
			}
		}
	}

	EVP_CIPHER_CTX_free(evpContextoCifrado);

	return QByteArray::fromStdString(resultados);
}

/**
 * @brief Descifra un texto
 */
QString Configuraciones::descifrarTexto(const QByteArray &datos, const QString &contrasena) {
	std::string datosCifrados = datos.toStdString();
	int longitudDatosCifrados1 = 0;
	int longitudDatosCifrados2 = 0;
	std::string vectorInicializacion = { 0x34, 0x61, 0x30, 0x31, 0x62, 0x35, 0x61, 0x38, 0x34, 0x31, 0x30, 0x38, 0x38, 0x65, 0x31, 0x30 };
	std::string contrasenaStd = contrasena.toStdString();
	EVP_CIPHER_CTX *evpContextoCifrado;
	std::string resultados;

	evpContextoCifrado = EVP_CIPHER_CTX_new();
	if (EVP_DecryptInit_ex(evpContextoCifrado, EVP_aes_256_cbc(), nullptr, (const unsigned char *)&contrasenaStd[0], (const unsigned char *)&vectorInicializacion[0]) != -1) {
		resultados.resize(512);

		if (EVP_DecryptUpdate(evpContextoCifrado, (unsigned char *)&resultados[0], &longitudDatosCifrados1, (const unsigned char *)&datosCifrados[0], static_cast<int>(datosCifrados.size())) != -1) {
			if (EVP_DecryptFinal_ex(evpContextoCifrado, (unsigned char *)&resultados[0] + longitudDatosCifrados1, &longitudDatosCifrados2) != -1) {
				resultados.resize(longitudDatosCifrados1 + longitudDatosCifrados2);
			}
		}
	}

	EVP_CIPHER_CTX_free(evpContextoCifrado);

	return QString::fromStdString(resultados);
}
