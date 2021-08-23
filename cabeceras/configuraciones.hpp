#ifndef CONFIGURACIONES_HPP
#define CONFIGURACIONES_HPP

#include <QSettings>


class Configuraciones : public QObject {
	Q_OBJECT

	public:
		/*!
		 * \brief Inicializa la instancia de la clase
		 * \param padre Puntero al objeto padre
		 */
		Configuraciones(QObject *padre = nullptr);

		/*!
		 * \brief Verifica si existe la llave suministrada en las configuraciones de la aplicacion
		 * \param llave Llave a verificar
		 * \return Verdadero si existe la llave, falso en caso contrario
		 */
		Q_INVOKABLE bool existente(const QString &llave);

		/*!
		 * \brief Obtiene el valor de la configuracion de la llave suministrada
		 * \param llave Llave a leer
		 * \param valorPredeterminado Valor prederminado a devolver en caso que no existe la llave en la configuracion
		 * \return Valor de la llave obtenida de la configuracion de la aplicacion
		 */
		Q_INVOKABLE QVariant valor(const QString &llave, const QVariant &valorPredeterminado = QVariant());

		/*!
		 * \brief Establece el valor suministrado a la llave suministrada en la configuracion de la aplicacion
		 * \param llave Llave a escribir
		 * \param valor Valor a establecer en la llave suministrada
		 */
		Q_INVOKABLE void establecerValor(const QString &llave, const QVariant &valor);

		/*!
		 * \brief Elimina una llave determinada de las configuraciones de la aplicacion
		 * \param llave Llave a eliminar
		 */
		Q_INVOKABLE void eliminar(const QString &llave);

	private:
		/*!
		 * \brief Interfaz de las configuraciones nativas del sistema
		 */
		QSettings _configuraciones;

		/*!
		 * \brief Cifra el texto suministrado con la contraseña suministrada
		 * \param datos Datos a cifrar
		 * \param contrasena Contraseña a utilizar en el cifrado
		 * \return Datos cifrados
		 */
		QByteArray cifrarTexto(const QString &datos, const QString &contrasena);

		/*!
		 * \brief Descifra el texto suministrado con la contraseña suministrada
		 * \param datos Datos a descifrar
		 * \param contrasena Contraseña a utilizar en el descifrado
		 * \return Datos descifrados
		 */
		QString descifrarTexto(const QByteArray &datos, const QString &contrasena);
};

#endif // HTTP2_HPP
