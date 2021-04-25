#ifndef VENTANAPRINCIPAL_HPP
#define VENTANAPRINCIPAL_HPP

#include <QMainWindow>


class VentanaPrincipal : public QMainWindow
{
	Q_OBJECT

	public:
		VentanaPrincipal(QWidget *parent = nullptr);
		~VentanaPrincipal();

	private:
		/**
		 * @brief Construye la interfaz de usuario
		 */
		void construirIU();
};

#endif // VENTANAPRINCIPAL_HPP
