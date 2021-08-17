package cu.atds3.android;

import org.qtproject.qt5.android.QtNative;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtActivity;


public class ActividadExtendida extends QtActivity  {
	public static boolean aplicacionInicializada;
	public static boolean existeIntencionPendiente;
	public static final int CODIGO_ABRIR_ARCHIVO_DESCARGA = 0x01;
	public static final int CODIGO_SELECCIONAR_RUTA_DESCARGA = 0x02;
	public static final String ETIQUETA = "atds3";

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Intent intencion = getIntent();
		if (intencion != null){
			String accion = intencion.getAction();
			if (accion != null){
				existeIntencionPendiente = true;
			}
		}
	}

	@Override
	public void onDestroy() {
		System.exit(0);
	}

	@Override
	public void onNewIntent(Intent intencion) {
		super.onNewIntent(intencion);
		setIntent(intencion);
		if(aplicacionInicializada == true) {
//			procesarIntencion();
		} else {
			existeIntencionPendiente = true;
		}
	}

	@Override
	public void onActivityResult(int codigoSolicitud, int codigoResultado, Intent datosResultado) {
		super.onActivityResult(codigoSolicitud, codigoResultado, datosResultado);

		if (codigoResultado == Activity.RESULT_OK) {
			switch (codigoSolicitud) {
				case ActividadExtendida.CODIGO_ABRIR_ARCHIVO_DESCARGA:
					Log.d(ActividadExtendida.ETIQUETA, "onActivityResult(CODIGO_ABRIR_ARCHIVO_DESCARGA)");
					break;
				case ActividadExtendida.CODIGO_SELECCIONAR_RUTA_DESCARGA:
					//otorgarPermisosDirectorio(datosResultado.getData());
					break;
			}
		}
	}

/*
	public void checkPendingIntents(String workingDir) {
		aplicacionInicializada = true;
		workingDirPath = workingDir;
		if(hayIntencionPendiente == true) {
			hayIntencionPendiente = false;
			procesarIntencion();
		}
	}

	private void procesarIntencion(){
		Intent intencion = getIntent();
		Uri intencionURI;
		String intencionEsquema;
		String intencionAccion;

		// we are listening to android.intent.action.SEND or VIEW (see Manifest)
		if (intencion.getAction().equals("android.intent.action.SEND")){
			Bundle comboyado = intencion.getExtras();
			intencionURI = (Uri)comboyado.get(Intent.EXTRA_STREAM);
		} else {
			return;
		}

		if (intencionURI == null){
			return;
		}

		intencionEsquema = intencionURI.getScheme();
		if (intencionURI == null){
			return;
		}
		if(intencionEsquema.equals("file")){
			setFileUrlReceived(intentUri.toString());
			return;
		}
		if(!intencionEsquema.equals("content")){
			return;
		}

		ContentResolver rC = this.getContentResolver();
		MimeTypeMap mime = MimeTypeMap.getSingleton();
		String archivoExtension = mime.getExtensionFromMimeType(rC.getType(intencionURI));
		String mimeTipo = rC.getType(intencionURI);
		String nombre = QShareUtils.getContentName(cR, intencionURI);
		String archivoRuta = QSharePathResolver.getRealPathFromURI(this, intencionURI);
		if (checkFileExits(archivoRuta)) {
			setFileUrlReceived(archivoRuta);
			return;
		}

		archivoRuta = QShareUtils.createFile(rC, intencionURI, workingDirPath);
		if(archivoRuta == null) {
			return;
		}
		setFileReceivedAndSaved(archivoRuta);
	}
*/
	public void seleccionarRutaDescarga() {
		Intent intencion = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
		intencion.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

		startActivityForResult(intencion, ActividadExtendida.CODIGO_SELECCIONAR_RUTA_DESCARGA);
	}

	public void otorgarPermisosDirectorio(String ruta) {
		try {
//			Uri uri = Uri.parse(ruta);

			grantUriPermission(getPackageName(), Uri.parse(ruta), Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
//			getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
