import qbs.base 1.0

DynamicLibrary {
	name: "pillowcore"

	files: [
		"ByteArrayHelpers.h", "HttpHandlerProxy.h", "HttpHelpers.h", "HttpClient.h", "HttpHandlerQtScript.h", "HttpServer.h", "HttpConnection.h", "HttpHandlerSimpleRouter.h", "HttpsServer.h", "HttpHandler.h", "HttpHeader.h", "pch.h",
		"HttpClient.cpp", "HttpConnection.cpp", "HttpHandler.cpp", "HttpHandlerProxy.cpp", "HttpHandlerSimpleRouter.cpp", "HttpHandlerQtScript.cpp", "HttpHeader.cpp", "HttpHelpers.cpp", "HttpServer.cpp", "HttpsServer.cpp", "parser/parser.c", "parser/http_parser.c"
	]

	Depends { name: 'cpp' }
	Depends { name: 'Qt'; submodules: ["core", "network", "script", "declarative"] }

	ProductModule {
		Depends { name: "cpp" }
		cpp.includePaths: ['.']
		cpp.cxxFlags: ["-std=c++0x"]
	}

	cpp.precompiledHeader: "pch.h"
	cpp.cxxFlags: ["-std=c++0x", "-Winvalid-pch"]
	cpp.staticLibraries: ["z"]
}

