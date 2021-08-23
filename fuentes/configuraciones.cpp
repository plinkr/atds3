/*!
 * \file configuraciones.cpp
 * \brief Lectura y escritura de las configuraciones de la aplicacion
 * \ingroup cu.atds3
 * \author Leinier Cruz Salfrán
 * Contact: leiniercs@gmail.com
 */

#include "configuraciones.hpp"
#include "main.hpp"
#include <openssl/evp.h>


/*!
 * \brief Inicializa la instancia de la clase
 * \param padre Puntero al objeto padre
 */
Configuraciones::Configuraciones(QObject *padre)
	: QObject(padre) {
}

/*!
 * \brief Verifica si existe la llave suministrada en las configuraciones de la aplicacion
 * \param llave Llave a verificar
 * \return Verdadero si existe la llave, falso en caso contrario
 */
bool Configuraciones::existente(const QString &llave) {
	return _configuraciones.contains(llave);
}

/*!
 * \brief Obtiene el valor de la configuracion de la llave suministrada
 * \param llave Llave a leer
 * \param valorPredeterminado Valor prederminado a devolver en caso que no existe la llave en la configuracion
 * \return Valor de la llave obtenida de la configuracion de la aplicacion
 */
QVariant Configuraciones::valor(const QString &llave, const QVariant &valorPredeterminado) {
	QVariant resultado = _configuraciones.value(llave, valorPredeterminado);

	if (llave == "todus/telefono" || llave == "proxy/contrasena") {
		std::string contrasena { 0x41, 0x54, 0x44, 0x53, 0x33, 0x5f, 0x38, 0x32, 0x31, 0x31, 0x31, 0x32, 0x33, 0x32, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33 };

		resultado = descifrarTexto(resultado.toByteArray(), QString::fromStdString(contrasena));
	}

	return resultado;
}

/*!
 * \brief Establece el valor suministrado a la llave suministrada en la configuracion de la aplicacion
 * \param llave Llave a escribir
 * \param valor Valor a establecer en la llave suministrada
 */
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
				std::string contrasena { 0x41, 0x54, 0x44, 0x53, 0x33, 0x5f, 0x38, 0x32, 0x31, 0x31, 0x31, 0x32, 0x33, 0x32, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33 };

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
				std::string contrasena { 0x41, 0x54, 0x44, 0x53, 0x33, 0x5f, 0x38, 0x32, 0x31, 0x31, 0x31, 0x32, 0x33, 0x32, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33, 0x33, 0x30, 0x34, 0x5f, 0x41, 0x54, 0x44, 0x53, 0x33 };

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

/*!
 * \brief Elimina una llave determinada de las configuraciones de la aplicacion
 * \param llave Llave a eliminar
 */
void Configuraciones::eliminar(const QString &llave) {
	_configuraciones.remove(llave);
}

/*!
 * \brief Cifra el texto suministrado con la contraseña suministrada
 * \param datos Datos a cifrar
 * \param contrasena Contraseña a utilizar en el cifrado
 * \return Datos cifrados
 */
QByteArray Configuraciones::cifrarTexto(const QString &datos, const QString &contrasena) {
	std::string datosDescifrados = datos.toStdString();
	int longitudDatosDescifrados1 = 0;
	int longitudDatosDescifrados2 = 0;
	std::string vectorInicializacion { 0x34, 0x61, 0x30, 0x31, 0x62, 0x35, 0x61, 0x38, 0x34, 0x31, 0x30, 0x38, 0x38, 0x65, 0x31, 0x30 };
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

/*!
 * \brief Descifra el texto suministrado con la contraseña suministrada
 * \param datos Datos a descifrar
 * \param contrasena Contraseña a utilizar en el descifrado
 * \return Datos descifrados
 */
QString Configuraciones::descifrarTexto(const QByteArray &datos, const QString &contrasena) {
	std::string datosCifrados = datos.toStdString();
	int longitudDatosCifrados1 = 0;
	int longitudDatosCifrados2 = 0;
	std::string vectorInicializacion { 0x34, 0x61, 0x30, 0x31, 0x62, 0x35, 0x61, 0x38, 0x34, 0x31, 0x30, 0x38, 0x38, 0x65, 0x31, 0x30 };
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
