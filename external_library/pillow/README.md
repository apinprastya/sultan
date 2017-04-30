# Pillow - A small, light and fluffy web server.

Pillow is a small http server library built with Qt. It is meant to be embedded in Qt apps, where it can assist in exposing an http interface to the app, or to be used as the basis for implementing a small web server app using Qt and/or QtScript.

## Pillow is 

* An event based http server, not relying on worker threads to server requests
* A simple code base, easy to work with, easy to extend
* Fast, with very few memory allocations per request
* Memory efficient, reusing and sharing existing buffers when possible
* Portable, tested on Linux and Windows
* IO channel independant, able to communicate through TCP sockets, SSL TCP sockets, Unix domain sockets, named pipes, memory buffers, etc.
* Including a few useful middleware-like modules

## Pillow is not

Pillow is not a web application framework. Pillow does not provide a full framework to develop web applications; it only deals with receiving client requests, handling them to you and sending back your response. Whether you develop a C++ or QtScript/Javascript app framework, or integrate Ruby and a Rack interface to run Rails app on top of it is up to you. The Pillow core does not go there.

Pillow is not meant to be used in a hostile environment (such as a public server). While it contains a few basic mecanism to defend itself against malicious request, I did not develop Pillow with security as my main concern. Pillow requires the Qt event loop, which while not specifically optimized for networking code, allows for using all of Qt's asynchronous classes.

## Examples

### A simple file server:
	
	HttpServer* server = new HttpServer(QHostAddress::Any, 4567);
	HttpHandlerFile* handler = new HttpHandlerFile("/some/public/path");
	connect(server, SIGNAL(requestReady(Pillow::HttpRequest*)),
	        handler, SLOT(handleRequest(Pillow::HttpRequest*)));

### A simple QtScript/Javascript web application:
	
	HttpServer* server = new HttpServer(QHostAddress(QHostAddress::Any), 4567);	
	QScriptEngine* scriptEngine = new QScriptEngine(server);
	HttpHandlerStack* handler = new HttpHandlerStack(server);
		new HttpHandlerLog(handler);
		new HttpHandlerQtScriptFile(scriptEngine, "test.js", "handleRequest", true, handler);
		new HttpHandler404(handler);
	connect(server, SIGNAL(requestReady(Pillow::HttpRequest*)),
	        handler, SLOT(handleRequest(Pillow::HttpRequest*)));

## Building

Tested with: Qt 4.8 and Qt 5.5

Tested on the following OS/Compiler/Qt combinations:

* Kubuntu 15.10 x64 (GCC 5.2.1, Qt 5.5.1, Qt 4.8.6)
* Windows 7 (MSVC 10/Visual Studio 2010, Qt 5.4, Qt 4.8.4)
* Windows 10 (MSVC 12/Visual Studio 2013 update 4, Qt 5.5.1)

Dependencies: At lest the following Qt development modules: QtCore, QtNetwork (with SSL support enabled) and QtScript. I have plans to make the QtNetwork SSL and QtScript dependencies optional. However, they should be included out of the box in the Qt dev packages of most Linux distros, and they are included in the official Qt packages for Windows. 

From a shell or command line prompt:

Build:

	cd /path/to/pillow
	qmake	# Or the full path to Qt's qmake
	make	# Or nmake, if you are building with MSVC.
	# ... Wait a few seconds
	# The pillowcore lib has been placed in the "lib" folder and the various examples
	# have been also compiled in their own folder.

Test:

	cd tests
	./tests # Or ./release/tests.exe on Windows
	# If all tests pass, you are good to go. If some fails, please post a bug or help fix it!
	# Note that some tests are especially flaky on Windows.

Integrate:

- Link to "libpillowcore.a" or "pillowcore.lib".
- Add /path/to/pillow/pillowcore to your headers include path.

## License

Ruby License, http://www.ruby-lang.org/en/LICENSE.txt.

## Credits

* The parser is a slightly modified version from the one included in Thin http://code.macournoyer.com/thin/ by Marc-André Cournoyer. Thin is copyrighted free software by Marc-André Cournoyer, distributed under the terms of the Ruby License.
* The parser is originally from Mongrel http://mongrel.rubyforge.org by Zed Shaw. The Mongrel http parser is copyrighted free software by Zed A. Shaw <zedshaw at zedshaw dot com>, redistributed under the terms of the Ruby License.
* Pillow is copyright Alexandre Cossette-Pacheco <acossette at gmail dot com>. 
