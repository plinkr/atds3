#ifndef _UTILES_HPP
#define _UTILES_HPP

#include <QObject>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTcpSocket>


class Utiles : public QObject {
	Q_OBJECT

	public:
		enum EstadosTodus {
			Desconocido,
			Disponible,
			Perdido
		};

		Utiles(QObject *padre = nullptr);

		Q_INVOKABLE void iniciarMonitorizacionConexionTodus();
		Q_INVOKABLE void crearBandejaIcono();
		Q_INVOKABLE void notificar(const QString &llave, bool valorPredeterminado, const QString &titulo, const QString &mensaje, const QString &sonido);
		Q_INVOKABLE void restablecerDatosFabrica();
		Q_INVOKABLE QString rutaDesdeURI(const QString &uri);

	private slots:
		void mostrarOcultarVentana(QSystemTrayIcon::ActivationReason razon);
		void verificarDisponibilidadTodus();
		void eventoConexionTodus();
		void eventoErrorConexionTodus(QAbstractSocket::SocketError errorSocalo);

	private:
		QSettings _configuraciones;
		QSystemTrayIcon _bandejaIcono;
		QTimer _temporizadorVerificadorDisponibilidadTodus;
		QTcpSocket _socaloTCPDisponibilidadTodus;
		EstadosTodus _estadoDisponibilidadTodus;
};

#endif // _UTILES_HPP
