package cu.atds3.android;

import org.qtproject.qt5.android.QtNative;

import android.app.Activity;
import android.content.Context;
import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.Settings;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import java.io.File;
//import androidx.documentfile.provider.DocumentFile;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.ekkescorner.utils.QSharePathResolver;


public class ActividadExtendida extends QtActivity  {
	public static native void notificarSeleccionRutaDescarga(String ruta);
	public static native void notificarSeleccionArchivoDescarga(int descriptor);
	public static native void notificarArchivoCompartido(String ruta);

	public static final String ETIQUETA = "atds3";

	public boolean aplicacionInicializada = false;
	public boolean existeIntencionPendiente = false;

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
			procesarIntencion();
		} else {
			existeIntencionPendiente = true;
		}
	}

	@Override
	public void onActivityResult(int codigoSolicitud, int codigoResultado, Intent datosResultado) {
		super.onActivityResult(codigoSolicitud, codigoResultado, datosResultado);

		try {
			if (codigoResultado == Activity.RESULT_OK) {
				switch (codigoSolicitud) {
					case Constantes.CODIGO_SELECCIONAR_ARCHIVO_DESCARGA:
						notificarSeleccionArchivoDescarga(obtenerDescriptorArchivo(datosResultado.getData()));
						break;
					case Constantes.CODIGO_SELECCIONAR_RUTA_EXTERNO:
	//					notificarSeleccionRutaDescarga(uri.toString());

						//val documentsTree = DocumentFile.fromTreeUri(getApplication(), directoryUri) ?: return
						//val childDocuments = documentsTree.listFiles().toCachingList()
						break;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void procesarIntencion(){
		Intent intencion = getIntent();
		Uri intencionURI;
		String intencionEsquema;
		String intencionAccion;

		existeIntencionPendiente = false;

		if (intencion.getAction().equals("android.intent.action.SEND")){
			Bundle comboyado = intencion.getExtras();
			intencionURI = (Uri)comboyado.get(Intent.EXTRA_STREAM);
		} else {
			return;
		}

		if (intencionURI == null){
			return;
		}

		notificarArchivoCompartido(intencionURI.toString());
/*
		intencionEsquema = intencionURI.getScheme();
		if (intencionEsquema == null){
			return;
		}
		if(intencionEsquema.equals("file")){
			notificarArchivoCompartido(intencionURI.toString());
			return;
		}
		if(!intencionEsquema.equals("content")){
			return;
		}

		String archivoRuta = QSharePathResolver.getRealPathFromURI(this, intencionURI);
		notificarArchivoCompartido(archivoRuta);
*/
/*
		archivoRuta = QShareUtils.createFile(rC, intencionURI, workingDirPath);
		if(archivoRuta == null) {
			return;
		}
		setFileReceivedAndSaved(archivoRuta);
*/
	}

	public void solicitarAdministracionCompletaArchivos() {
		try {
			Intent intencion = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
			Uri uri = Uri.fromParts("package", getPackageName(), null);

			intencion.setData(uri);
			startActivity(intencion);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String obtenerRutaSistema(int indice) {
		File[] efd = getExternalFilesDirs(null);
		File ruta = efd[0];
		String resultado;

		if (indice < efd.length) {
			ruta = efd[indice];
		}

		for (int i = 0; i < 4; i++) {
			ruta = ruta.getParentFile();
		}

		resultado = "file://" + ruta.getAbsolutePath() + "/ATDS3";

		crearDirectorio(resultado);

		aplicacionInicializada = true;
		if (existeIntencionPendiente == true) {
			procesarIntencion();
		}

		return resultado;
	}

	public void seleccionarArchivoDescarga() {
		Intent intencion = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		intencion.addCategory(Intent.CATEGORY_OPENABLE);
		intencion.setType("text/plain");
		intencion.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION | Intent.FLAG_GRANT_READ_URI_PERMISSION);

		startActivityForResult(intencion, Constantes.CODIGO_SELECCIONAR_ARCHIVO_DESCARGA);
	}

	public void seleccionarRutaExterno() {
		File[] efd = getExternalFilesDirs(null);
		File ruta = efd[0];
		String resultado;

		if (efd.length > 1) {
			ruta = efd[1];
		}

		for (int i = 0; i < 4; i++) {
			ruta = ruta.getParentFile();
		}

		resultado = "file://" + ruta.getAbsolutePath();

		Intent intencion = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
		intencion.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION | Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
		intencion.putExtra(DocumentsContract.EXTRA_INITIAL_URI, Uri.parse(resultado));

		startActivityForResult(intencion, Constantes.CODIGO_SELECCIONAR_RUTA_EXTERNO);
	}

	public void otorgarPermisosDirectorio(String ruta) {
		grantUriPermission(getPackageName(), Uri.parse(ruta), Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
	}

	private void otorgarPermisosUri(Uri uri) {
		try {
			getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

//			grantUriPermission(getPackageName(), uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
//			getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void crearDirectorio(String ruta) {
		try {
			File directorio = new File(ruta);

			if (directorio.exists() == false) {
				directorio.mkdirs();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private int obtenerDescriptorArchivo(Uri uri) {
		int da = -1;

		try {
			ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "rw");

			da = pfd.detachFd();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return da;
	}
}
