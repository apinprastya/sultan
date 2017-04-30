import qbs.base 1.0

Project {
	references: [
		"pillowcore/pillowcore.qbs",
		"tests/tests.qbs",
		"examples/clientbench/clientbench.qbs",
		"examples/declarative/declarative.qbs",
		"examples/fileserver/fileserver.qbs",
		"examples/qtscript/qtscript.qbs",
		"examples/simple/simple.qbs",
		"examples/simplessl/simplessl.qbs",
	]
}
