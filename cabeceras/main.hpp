#ifndef MAIN_HPP
#define MAIN_HPP


#include <QString>
#include <QSharedPointer>
#include "gestordescargas.hpp"


/**
 * @brief Título de la aplicación
 */
extern QString _aplicacionTitulo;

/**
 * @brief Nombre corto de la aplicación
 */
extern QString _aplicacionNombreCorto;

/**
 * @brief Versión de la aplicación
 */
extern QString _aplicacionVersion;

/**
 * @brief Ruta en donde almacenar las descargas
 */
extern QString _rutaDescargas;

/**
 * @brief Listado de categorías
 */
enum _ListadoCategorias {
	Programas = 0x03,
	Musica = 0x04,
	Videos = 0x05,
	Otros = 0x06
};

/**
 * @brief Listado de estados
 */
enum _ListadoEstados {
	EnEsperaPausar = 0x01,
	Pausada = 0x02,
	EnEsperaIniciar = 0x03,
	Iniciada = 0x04,
	Finalizada = 0x05
};

struct _NuevaDescarga {
	QString enlace;
	QString nombre;
	int categoria;
	bool iniciar;
};

#endif // MAIN_HPP

