// from: https://github.com/wkh237/react-native-fetch-blob/blob/master/android/src/main/java/com/RNFetchBlob/Utils/PathResolver.java
// MIT License, see: https://github.com/wkh237/react-native-fetch-blob/blob/master/LICENSE
// original copyright: Copyright (c) 2017 xeiyan@gmail.com
// src slightly modified to be used into Qt Projects: (c) 2017 ekke@ekkes-corner.org
// Ajustado por: Leinier Cruz Salfran <leiniercs@gmail.com>

package org.ekkescorner.utils;

import android.content.ContentUris;
import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.lang.NumberFormatException;
import android.util.Log;


public class QSharePathResolver {
	public static String getRealPathFromURI(final Context context, final Uri uri) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT && DocumentsContract.isDocumentUri(context, uri)) { // DocumentProvider
			if (isExternalStorageDocument(uri)) { // ExternalStorageProvider
				final String docId = DocumentsContract.getDocumentId(uri);
				final String[] split = docId.split(":");
				String ruta = Environment.getExternalStorageDirectory().getAbsolutePath();

				if ("primary".equalsIgnoreCase(split[0]) == false) {
					ruta = ruta.replace("emulated/0", split[0]);
				}
				ruta += "/" + split[1];

				return ruta;
			} else if (isDownloadsDocument(uri)) { // DownloadsProvider
				final String id = DocumentsContract.getDocumentId(uri);
				long longId = 0;

				try {
					longId = Long.valueOf(id);
				} catch (NumberFormatException nfe) {
					return getDataColumn(context, uri, null, null);
				}

				final Uri contentUri = ContentUris.withAppendedId(Uri.parse("content://downloads/public_downloads"), longId);

				return getDataColumn(context, contentUri, null, null);
			} else if (isMediaDocument(uri)) { // MediaProvider
				final String docId = DocumentsContract.getDocumentId(uri);
				final String[] split = docId.split(":");
				Uri contentUri = null;

				if ("image".equals(split[0])) {
					contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
				} else if ("video".equals(split[0])) {
					contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
				} else if ("audio".equals(split[0])) {
					contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
				}

				final String selection = "_id=?";
				final String[] selectionArgs = new String[] { split[1] };

				return getDataColumn(context, contentUri, selection, selectionArgs);
			} else if ("content".equalsIgnoreCase(uri.getScheme())) { // content://
				// Return the remote address
				if (isGooglePhotosUri(uri)) {
					return uri.getLastPathSegment();
				}

				return getDataColumn(context, uri, null, null);
			} else { // Other Providers
				try {
					InputStream attachment = context.getContentResolver().openInputStream(uri);

					if (attachment != null) {
						String filename = getContentName(context.getContentResolver(), uri);

						if (filename != null) {
							File file = new File(context.getCacheDir(), filename);
							FileOutputStream tmp = new FileOutputStream(file);
							byte[] buffer = new byte[1024];

							while (attachment.read(buffer) > 0) {
								tmp.write(buffer);
							}

							tmp.close();
							attachment.close();
							return file.getAbsolutePath();
						}
					}
				} catch (Exception e) {
					// TODO SIGNAL shareError()
					return null;
				}
			}
		} else if (uri.toString().startsWith("content://com.android.externalstorage.documents/tree/")) { // ExternalStorageProvider
			String enlace = uri.toString();
			enlace = enlace.replace("content://com.android.externalstorage.documents/tree/", "");
			final String[] split = enlace.split(":");
			String ruta = Environment.getExternalStorageDirectory().getAbsolutePath();

			if (split[0].endsWith("primary") == false) {
				ruta = ruta.replace("emulated/0", split[0]);
			}
			ruta += "/" + split[1];

			return ruta;
		} else if ("content".equalsIgnoreCase(uri.getScheme())) { // MediaStore (and general)
			// Return the remote address
			if (isGooglePhotosUri(uri)) {
				return uri.getLastPathSegment();
			}
			return getDataColumn(context, uri, null, null);
		} else if ("file".equalsIgnoreCase(uri.getScheme())) { // File
			return uri.getPath();
		}

		return null;
	}

	private static String getContentName(ContentResolver resolver, Uri uri) {
		Cursor cursor = resolver.query(uri, null, null, null, null);
		cursor.moveToFirst();
		int nameIndex = cursor.getColumnIndex(MediaStore.MediaColumns.DISPLAY_NAME);

		if (nameIndex >= 0) {
			String name = cursor.getString(nameIndex);
			cursor.close();
			return name;
		}

		cursor.close();
		return null;
	}

	/**
	* Get the value of the data column for this Uri. This is useful for
	* MediaStore Uris, and other file-based ContentProviders.
	*
	* @param context The context.
	* @param uri The Uri to query.
	* @param selection (Optional) Filter used in the query.
	* @param selectionArgs (Optional) Selection arguments used in the query.
	* @return The value of the _data column, which is typically a file path.
	*/
	public static String getDataColumn(Context context, Uri uri, String selection, String[] selectionArgs) {
		Cursor cursor = null;
		String result = null;
		final String column = "_data";
		final String[] projection = { column };

		try {
			cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs, null);
			if (cursor != null && cursor.moveToFirst()) {
				final int index = cursor.getColumnIndexOrThrow(column);
				result = cursor.getString(index);
			}
		} catch (Exception ex) {
			ex.printStackTrace();
			return null;
		} finally {
			if (cursor != null) {
				cursor.close();
			}
		}

		return result;
	}

	/**
	* @param uri The Uri to check.
	* @return Whether the Uri authority is ExternalStorageProvider.
	*/
	public static boolean isExternalStorageDocument(Uri uri) {
		return "com.android.externalstorage.documents".equals(uri.getAuthority());
	}

	/**
	* @param uri The Uri to check.
	* @return Whether the Uri authority is DownloadsProvider.
	*/
	public static boolean isDownloadsDocument(Uri uri) {
		return "com.android.providers.downloads.documents".equals(uri.getAuthority());
	}

	/**
	* @param uri The Uri to check.
	* @return Whether the Uri authority is MediaProvider.
	*/
	public static boolean isMediaDocument(Uri uri) {
		return "com.android.providers.media.documents".equals(uri.getAuthority());
	}

	/**
	* @param uri The Uri to check.
	* @return Whether the Uri authority is Google Photos.
	*/
	public static boolean isGooglePhotosUri(Uri uri) {
		return "com.google.android.apps.photos.content".equals(uri.getAuthority());
	}
}
