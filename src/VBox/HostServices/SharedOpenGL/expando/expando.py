# $Id$
# This script generates calls for display list compilation
# and state management.
import sys

sys.path.append( "../../glapi_parser" )
import apiutil

apiutil.CopyrightC()

print """
/* DO NOT EDIT - THIS FILE AUTOMATICALLY GENERATED BY expando.py SCRIPT */
#include <stdio.h>
#include "cr_error.h"
#include "cr_spu.h"
#include "cr_dlm.h"
#include "expandospu.h"
"""

allFunctions = []
generatedFunctions = []

for func_name in apiutil.GetDispatchedFunctions(sys.argv[1]+"/APIspec.txt"):
	if apiutil.FindSpecial("expando", func_name):
		allFunctions.append(func_name)
	elif apiutil.CanCompile(func_name) or apiutil.SetsClientState(func_name):
		generatedFunctions.append(func_name)
		allFunctions.append(func_name)

for func_name in generatedFunctions:
	params = apiutil.Parameters(func_name)
	return_type = apiutil.ReturnType(func_name)
	basicCallString = apiutil.MakeCallString(params)
	declarationString = apiutil.MakeDeclarationString(params)
	dlmCallString = basicCallString
	chromiumProps = apiutil.ChromiumProps(func_name)

	needClientState = 0
	if apiutil.UsesClientState(func_name):
		dlmCallString = basicCallString + ", clientState"
		needClientState = 1

	needDL = 0
	if apiutil.CanCompile(func_name):
		needDL = 1

	print 'static %s EXPANDOSPU_APIENTRY expando%s( %s )' % ( return_type, func_name, declarationString)
	print '{'
	if needDL:
		print '\tGLenum dlMode = crDLMGetCurrentMode();'
	if needClientState:
		print '\tCRContext *stateContext = crStateGetCurrent();'
		print '\tCRClientState *clientState = NULL;'
		print '\tif (stateContext != NULL) {'
		print '\t\tclientState = &(stateContext->client);'
		print '\t}'

	if needDL:
		if "checklist" in chromiumProps:
			print '\tif (dlMode != GL_FALSE && crDLMCheckList%s(%s)) {' % (func_name, basicCallString)
		else:
			print '\tif (dlMode != GL_FALSE) {'
		print '\t\tcrDLMCompile%s(%s);' % (func_name, dlmCallString)
		# If we're only compiling, return now.
		print '\t\tif (dlMode == GL_COMPILE) return %s;' % '0' if return_type != "void" else ""
		print '\t}'

	# If it gets this far, we're either just executing, or executing
	# and compiling.  Either way, pass the call to the super SPU,
	# and to the state tracker (if appropriate; note that we only
	# track client-side state, not all state).
	if return_type != "void":
	    print '\t%s rc = expando_spu.super.%s(%s);' % (return_type, func_name, basicCallString)
	else:
	    print '\texpando_spu.super.%s(%s);' % (func_name, basicCallString)
	if apiutil.SetsClientState(func_name):
		print '\tcrState%s( %s );' % (func_name, basicCallString)	
	
	if return_type != "void":
	    print "\treturn rc;"

	print '}'
	print ''

# Generate the table of named functions. including all the static generated
# functions as well as the special functions.
print 'SPUNamedFunctionTable _cr_expando_table[] = {'
for func_name in allFunctions:
	print '\t{ "%s", (SPUGenericFunction) expando%s },' % (func_name, func_name )
print '\t{ NULL, NULL }'
print '};'
