# Administrador de Transferencias para toDus (S3)

`ATDS3` es una aplicación para escritorio que automatiza el proceso de descarga y subida de archivos desde/hacia los servidores de la red toDus (S3).

## Tabla de contenidos
- [Características](#caracteristicas)
- [Dependencias](#dependencias)
- [Instalando dependencias](#instalando-dependencias)
- [Construcción](#construccion)
- [Licencia](#licencia)

<a name="caracteristicas"></a>
## Características

`ATDS3` posee las siguientes características::

* **Multiplataforma**: `ATDS3` puede ser utilizado en cualquier sistema operativo que en donde la librería Qt pueda funcionar: UNIX (FreeBSD, NetBSD, OpenBSD), Linux, macOS y Windows.
* **Fácil de usar**: `ATDS3` es fácil de usar gracias a la intuitiva interfaz de usuario que posee.
* **Configurable**: `ATDS3` ofrece diversas opciones configurables que definen el comportamiento de diversas secciones.
* **Completamente asincrónico**: `ATDS3` es totalmente asincrónico, por lo que podrá realizar varias operaciones de descargas y subidas al mismo tiempo.
* **Ajustado al protocolo de red de toDus**: `ATDS3` Está ajustado lo mayormente posible al protocolo de red que utiliza toDus, incluyendo inicios de sesión partiendo del número telefónico.
* **Inteligene**: `ATDS3` tiene escrito código para comportarse de forma inteligente dependiendo de la situación de la red y cómo descargar los archivos.

<a name="dependencias"></a>
## Dependencias
`ATDS3` depende de:
 
* Compilador C++ compatible con el estándar C++17 (Clang 3.4+, GCC 4.9+, MSVC 19.0+ (Visual Studio 2015+), Intel C++ Compiler 17+)
* qmake
* make (UNIX y Linux)
* nmake (Windows)
* Qt
  * Qt 5 Core
  * Qt 5 GUI
  * Qt 5 SQL (SQLite3)
  * Qt 5 Network
  * Qt 5 Widgets
* OpenSSL
* Google Protocol Buffers

<a name="instalando-dependencias"></a>
### Instalando dependencias

<a name="freebsd"></a>
#### FreeBSD
* Instalar las [dependencias](#dependencias) utilizando el administrador de paquetes binarios:
```
pkg install qt5-qmake qt5-core qt5-gui qt5-sql qt5-sqldrivers-sqlite3 qt5-network qt5-widgets openssl protobuf
```

<a name="macos"></a>
#### macOS
* Instalar las herramientas de línea de comandos de Xcode vía `xcode-select --install`.
* Instalar las [dependencias](#dependencias) utilizando [Homebrew](https://brew.sh):
```
brew install qt5-qmake qt5-core qt5-gui qt5-sql qt5-sqldrivers-sqlite3 qt5-network qt5-widgets openssl protobuf
```

<a name="linux"></a>
#### Linux
* Instalar las [dependencias](#dependencias) utilizando el gestor de paquetes.
```
apt install g++ make qtbase5-dev qt5-default libssl-dev libprotobuf-dev protobuf-compiler
```

<a name="windows"></a>
#### Windows
* Descargar e instalar Microsoft Visual Studio 2015 o superior (Herramientas de compilación para C+, SDK de Windows, Paquete de idioma Inglés y Git).
* Descargar e instalar [Qt Opensource 6.0](https://www.qt.io/download-qt-installer) o superior (Librerías de Qt para MSVC2019 y formato de imágenes).
* Instalar [vcpkg](https://github.com/Microsoft/vcpkg#quick-start) y las dependencias de `ATDS3` como se muestra a continuación:
```
C:\
cd C:\Qt
git clone https://github.com/Microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe install openssl:x64-windows protobuf:x64-windows
```

<a name="construccion"></a>
## Construcción

La forma mas simple de construir `ATDS3`:

```
mkdir construccion
cd construccion
qmake ../atds3.pro CONFIG+=release
```

En UNIX y Linux:

```
make
```

En Windows:

```
nmake
```

<a name="licencia"></a>
## Licencia
`ATDS3` está licenciado bajo la licencia BSD versión 3. Vea [LICENSE](LICENSE.txt) para más información.
