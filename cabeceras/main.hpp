#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkProxy>
#include <iostream>


/*!
 * \brief Tipos de registros
 */
enum TiposRegistro {
	Ninguno,
	Critico,
	Error,
	Informacion,
	Depuracion
};

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
	public:
		QNetworkAccessManager *create(QObject *padre) override;
};

/*!
 * \brief Nombre de la organización
 */
extern QString _organizacionNombre;

/*!
 * \brief Nombre del dominio
 */
extern QString _organizacionDominio;

/*!
 * \brief Nombre corto de la aplicación
 */
extern QString _aplicacionNombreCorto;

/*!
 * \brief Título descriptivo de la aplicación
 */
extern QString _aplicacionTitulo;

/*!
 * \brief Versión de la aplicación
 */
extern QString _aplicacionVersion;

/*!
 * \brief Agente de usuario a utilizar en las conexiones al servidor HTTP de la red toDus
 */
extern QString _agenteUsuarioTodus;

/*!
 * \brief Número de la versión a utilizar en las conexiones al servidor HTTP de la red toDus
 */
extern QString _numeroVersionTodus;

/*!
 * \brief Ruta del directorio predeterminada en donde se almacenarán las descargas y archivos de publicación
 */
extern QString _rutaDescargaPredeterminada;

/*!
 * \brief Referencia al objeto raíz QML
 */
extern QObject *_qmlRaiz;


/*!
 * \brief Obtiene las configuraciones del proxy definidas por el usuario
 * \return Información del proxy a utilizar en  las clases de acceso a la red
 */
extern QNetworkProxy _obtenerProxy();

/*!
 * \brief Emite un registro
 * \param tipo Tipo de registro a emitir
 * \param modulo Nombre del módulo que emite el registro
 * \return Referencia al flujo de salida
 */
extern std::ostream &emitirRegistro(TiposRegistro tipo, const QString &modulo);

#endif // _MAIN_HPP
