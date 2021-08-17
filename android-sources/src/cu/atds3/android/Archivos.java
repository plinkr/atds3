package cu.atds3.android;

import android.content.Context;
import android.content.ContentResolver;
import android.content.Intent;
import android.os.ParcelFileDescriptor;
import android.net.Uri;
import android.util.Log;

// https://developer.android.com/training/data-storage/shared/documents-files
public class Archivos {
	public static void solicitarPermisoRutaArbol(Context contexto, String ruta) {
		try {
			Uri rutaArbol = Uri.parse(ruta);

			contexto.grantUriPermission(contexto.getPackageName(), rutaArbol, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
			contexto.getContentResolver().takePersistableUriPermission(rutaArbol, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static String abrirArchivo(Context contexto, String ruta, String modo) {
		int fd = -1;

		try {
			Uri uri = Uri.parse(ruta);

			contexto.grantUriPermission(contexto.getPackageName(), uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
			contexto.getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
			ParcelFileDescriptor pfd = contexto.getContentResolver().openFileDescriptor(uri, modo);

			fd = pfd.detachFd();
		} catch (Exception e) {
			e.printStackTrace();
			fd = -1;
		}

		return String.valueOf(fd);
	}
}
