#include "npapi-test.h"
#include "stubs.h"
//#include "file-io.h"
#include <sstream>

#if defined(OS_WIN)
#define SIZE_T_FORMAT_STRING "lu"
#else
#define SIZE_T_FORMAT_STRING "zu"
#endif

NPPluginFuncs *pluginFuncs = NULL;
NPNetscapeFuncs *browserFuncs = NULL;
NPObject *javascriptListener = NULL;

#define LOG_NAME "npapi-test.log" 

static NPClass JavascriptListener_NPClass = {
  NP_CLASS_STRUCT_VERSION_CTOR,
  Allocate,
  Deallocate,
  StubInvalidate,
  HasJavascriptMethod,
  InvokeJavascript,
  StubInvokeDefault,
  StubHasProperty,
  StubGetProperty,
  StubSetProperty,
  StubRemoveProperty,
  StubEnumerate,
  StubConstruct
}; //NPClass JavascriptListener_NPClass

void log(char * path, char * message){
  FILE * pFile = fopen (path,"ab");
  if (pFile!=NULL){
    fputs (message, pFile);
    fclose (pFile);
  }
}


#if defined(OS_WIN)
NPError __stdcall NP_GetEntryPoints(NPPluginFuncs *plugin_funcs) {
  log(LOG_NAME, "NP_GetEntryPoints\n");
  SetPluginFuncs(plugin_funcs);
  return NPERR_NO_ERROR;
}
NPError __stdcall NP_Initialize(NPNetscapeFuncs *browser_funcs) {
  log(LOG_NAME, "NP_Initialize\n");
  SetBrowserFuncs(browser_funcs);
  return NPERR_NO_ERROR;
}
#elif defined(OS_LINUX)
extern "C" {
NPError NP_Initialize(NPNetscapeFuncs *browser_funcs, NPPluginFuncs *plugin_funcs) {
  log(LOG_NAME, "NP_Initialize\n");
  NPError error = SetPluginFuncs(plugin_funcs);
  if (error != NPERR_NO_ERROR) {
    ResetFuncs();
    return error;
  }
  error = SetBrowserFuncs(browser_funcs);
  if (error != NPERR_NO_ERROR) {
    ResetFuncs();
  }
  return error;
}

char *NP_GetMIMEDescription(void) {
  log(LOG_NAME, "NP_GetMIMEDescription\n");
  return (char *)"application/x-npapi-test::test npapi extension;";
}

NPError NP_GetValue(void *instance, NPPVariable variable, void *value) {
  log(LOG_NAME, "NP_GetValue\n");
  switch(variable) {
    case NPPVpluginNameString: {
      *((char **)value) = (char *)"Test npapi";
      break;
    }
    case NPPVpluginDescriptionString: {
      *((char **)value) = (char *)"Plugin to allow npapi test";
      break;
    }
    default: {
      return NPERR_INVALID_PARAM;
    }
  }
  return NPERR_NO_ERROR;
}
}
#endif

extern "C" {
NPError NP_Shutdown(void) {
  log(LOG_NAME, "NP_Shutdown\n");
  if (javascriptListener != NULL) {
    browserFuncs->releaseobject(javascriptListener);
  }
  return NPERR_NO_ERROR;
}
}

NPError SetPluginFuncs(NPPluginFuncs *plugin_funcs) {
  log(LOG_NAME, "SetPluginFuncs\n");
  if (pluginFuncs != NULL) {
    return NPERR_INVALID_FUNCTABLE_ERROR;
  }
  pluginFuncs = plugin_funcs;
  
  plugin_funcs->newp = NewInstance;
  plugin_funcs->destroy = Destroy;
  plugin_funcs->setwindow = SetWindow;
  plugin_funcs->newstream = StubNewStream;
  plugin_funcs->destroystream = StubDestroyStream;
  plugin_funcs->asfile = StubStreamAsFile;
  plugin_funcs->writeready = StubWriteReady;
  plugin_funcs->write = StubWrite;
  plugin_funcs->print = StubPrint;
  plugin_funcs->event = HandleEvent;
  plugin_funcs->urlnotify = StubURLNotify;
  plugin_funcs->getvalue = GetValue;
  plugin_funcs->setvalue = SetValue;
  
  return NPERR_NO_ERROR;
}

NPError SetBrowserFuncs(NPNetscapeFuncs *browser_funcs) {
  log(LOG_NAME, "SetBrowserFuncs\n");
  if (browserFuncs != NULL) {
    return NPERR_INVALID_FUNCTABLE_ERROR;
  }
  browserFuncs = browser_funcs;
  return NPERR_NO_ERROR;
}

NPError NewInstance(NPMIMEType pluginType,
                        NPP instance,
                        uint16 mode,
                        int16 argc,
                        char *argn[],
                        char *argv[],
                        NPSavedData *saved){
  log(LOG_NAME, "NewInstance\n");                        
}                        

NPError SetWindow(NPP instance, NPWindow *window) {
  log(LOG_NAME, "SetWindow\n");
  return NPERR_NO_ERROR;
}

static NPError Destroy(NPP instance, NPSavedData **save) {
  log(LOG_NAME, "Destroy\n");
  return NPERR_NO_ERROR;
}

static int16 HandleEvent(NPP instance, void* event) {
  log(LOG_NAME, "HandleEvent\n");
  return false;
}

void ResetFuncs(void) {
  log(LOG_NAME, "ResetFuncs\n");
  browserFuncs = NULL;
  pluginFuncs = NULL;
}

NPObject *Allocate(NPP instance, NPClass *clazz) {
  log(LOG_NAME, "Allocate\n");
  NPObject *obj = (NPObject *)new NPClassWithNPP;
  obj->_class = clazz;
  obj->referenceCount = 0;
  return obj;
}

void Deallocate(NPObject *obj) {
  log(LOG_NAME, "Deallocate\n");
  delete (NPClassWithNPP *)obj;
}

NPError SetValue(NPP instance, NPNVariable variable, void *ret_alue){
	log(LOG_NAME, "SetValue\n");
	return NPERR_NO_ERROR;
}

void SetInstance(NPP instance, NPObject *passedObj) {
  log(LOG_NAME, "SetInstance\n");
  NPClassWithNPP *obj = (NPClassWithNPP *)passedObj;
  obj->npp = instance;
}

NPP GetInstance(NPObject *passedObj) {
  log(LOG_NAME, "GetInstance\n");
  NPClassWithNPP *obj = (NPClassWithNPP *)passedObj;
  return obj->npp;
}

NPError GetValue(NPP instance, NPPVariable variable, void *value) {
  log(LOG_NAME, "GetValue\n");
  switch (variable) {
    case NPPVpluginScriptableNPObject: {
      javascriptListener = (NPObject *)browserFuncs->createobject(instance, (NPClass *)&JavascriptListener_NPClass);
      *((NPObject **)value) = javascriptListener;
      SetInstance(instance, javascriptListener);
      break;
    }
    case NPPVpluginNeedsXEmbed: {
      *((bool *)value) = true;
      break;
    }
    default: {
      return NPERR_INVALID_PARAM;
    }
  }
  return NPERR_NO_ERROR;
}

bool HasJavascriptMethod(NPObject *npobj, NPIdentifier name) {
  log(LOG_NAME, "HasJavascriptMethod\n");
  //const char *method = browser_funcs_->utf8fromidentifier(name);
  return true;
}

bool InvokeJavascript_NoArgs(NPObject *npobj, const char *methodName, NPVariant *&result) {
  log(LOG_NAME, "InvokeJavascript_NoArgs\n");
  bool success = false;
  if (!strcmp(methodName, "getPlatform")) {
    //getPlatform() : string
#if defined(OS_WIN)
    success = SetReturnValue("windows", 7, *result);
#elif defined(OS_LINUX)
    success = SetReturnValue("linux", 5, *result);
#endif 
  } /*else if (!strcmp(methodName, "getTempPath") || !strcmp(methodName, "getTmpPath")) {
    char *value = NULL;
    size_t len = 0;
    if (getTempPath(value, len)) {
      success = SetReturnValue(value, len, *result);
      delete[] value;
    }
  }*/
  return success;
}
/*
bool InvokeJavascript_OneArg(NPObject *npobj, const char *methodName, const NPVariant &arg, NPVariant *&result) {
  if (!(NPVARIANT_IS_STRING(arg))) {
    return false;
  }

  bool success = false;

  const char *argStringValue = stringFromNpVariant(arg);

  if (!strcmp(methodName, "fileExists")) {
    //fileExists(filename : string) : bool
    success = SetReturnValue(fileExists(argStringValue), *result);
  } else if (!strcmp(methodName, "isDirectory")) {
    //isDirectory(filename : string) : bool
    success = SetReturnValue(isDirectory(argStringValue), *result);
  } else if (!strcmp(methodName, "createDirectory")) {
    if (!createDirectory(argStringValue)) {
      //TODO: Throw a particular exception
      success = false;
    } else {
      success = true;
    }
  } else if (!strcmp(methodName, "getTextFile")) {
    //getTextFile(filename : string) : string
    char *value = NULL;
    size_t len = 0;
    if (getFile(argStringValue, value, len, false)) {
      success = SetReturnValue(value, len, *result);
      delete[] value;
    }
  } else if (!strcmp(methodName, "getBinaryFile")) {
    //getBinaryFile(filename : string) : array<byte>
    char *value = NULL;
    size_t len = 0;
    if (getFile(argStringValue, value, len, true)) {
      success = SetArrayReturnValue(value, len, GetInstance(npobj), result);
      delete[] value;
    }
  } else if (!strcmp(methodName, "removeFile")) {
    //removeFile(filename : string) : void
    success = removeFile(argStringValue);
  } else if (!strcmp(methodName, "listFiles")) {
    //listFiles(filename : string) : array<object>
    std::vector<FileEntry *> *entries;
    if (listFiles(argStringValue, entries)) {
      success = SetArrayReturnValue(*entries, GetInstance(npobj), result);
      deleteFileEntries(entries);
    } else {
      success = false;
    }
  }

  delete[] argStringValue;
  return success;
}
*/
bool InvokeJavascript_TwoArgs(NPObject *npobj, const char *methodName, const NPVariant &arg1, const NPVariant &arg2, NPVariant *&result) {
  bool success = false;
  /*
  if (!strcmp(methodName, "saveTextFile") && NPVARIANT_IS_STRING(arg1) && NPVARIANT_IS_STRING(arg2)) {
    const char *filename = stringFromNpVariant(arg1);
    const char *contents = stringFromNpVariant(arg2);
    success = saveText(filename, contents, arg2.value.stringValue.UTF8Length);
    delete[] contents;
    delete[] filename;
  } else if (!strcmp(methodName, "saveBinaryFile") && NPVARIANT_IS_STRING(arg1) && NPVARIANT_IS_OBJECT(arg2)) {
    const char *filename = stringFromNpVariant(arg1);
    size_t length;
    const char *bytes = byteArrayFromNpVariant(arg2, GetInstance(npobj), length);
    success = saveBinaryFile(filename, bytes, length);
    delete[] bytes;
    delete[] filename;
  }
  */
  return success;
}

bool InvokeJavascript(NPObject *npobj,
                      NPIdentifier name,
                      const NPVariant *args,
                      uint32_t argCount,
                      NPVariant *result) {
  log(LOG_NAME, "InvokeJavascript\n");                                            
  const char *methodName = browserFuncs->utf8fromidentifier(name);
  bool success = false;
  switch (argCount) {
  case 0:
    success = InvokeJavascript_NoArgs(npobj, methodName, result);
    break;
  case 1:
    success = InvokeJavascript_OneArg(npobj, methodName, args[0], result);
    break;
  case 2:
    success = InvokeJavascript_TwoArgs(npobj, methodName, args[0], args[1], result);
    break;
  }
  browserFuncs->memfree((void *)methodName);
  return success;
}

NPVariant *eval(NPP instance, const char *scriptString) {
  log(LOG_NAME, "eval\n");
  NPString script;
  script.UTF8Characters = scriptString;
  script.UTF8Length = strlen(script.UTF8Characters);
  
  NPObject *window = NULL;
  browserFuncs->getvalue(instance, NPNVWindowNPObject, &window);
  NPVariant *result = new NPVariant();
  browserFuncs->evaluate(instance, window, &script, result);
  browserFuncs->releaseobject(window);
  return result;
}

bool SetReturnValue(const bool value, NPVariant &result) {
  log(LOG_NAME, "SetReturnValue\n");
  BOOLEAN_TO_NPVARIANT(value, result);
  return true;
}

bool SetReturnValue(const char *value, const size_t len, NPVariant &result) {
  log(LOG_NAME, "SetReturnValue\n");
  const size_t dstLen = len + 1;
  char *resultString = (char *)browserFuncs->memalloc(dstLen);
  if (!resultString) {
    return false;
  }
  memcpy(resultString, value, len);
  resultString[dstLen - 1] = 0;
  STRINGN_TO_NPVARIANT(resultString, len, result);
  return true;
}

bool SetArrayReturnValue(const char *value, const size_t len, NPP instance, NPVariant *result) {
  std::ostringstream str;
  str << "(function() { return [";
  if (len > 0) {
    str << (int)value[0];
  }
  for (size_t i = 1; i < len; ++i) {
    str << "," << (int)value[i];
  }
  str << "]; })()";
  *result = *eval(instance, str.str().c_str());
  return true;
}
/*
void dumpFileEntryAsJson(std::ostringstream &str, const FileEntry *file) {
  str << "{\"name\": \"" << file->name << "\", \"type\": \"" << (file->isDirectory ? "directory" : "file") << "\"}";
}
bool SetArrayReturnValue(const std::vector<FileEntry *> &files, NPP instance, NPVariant *result) {
  std::ostringstream str;
  str << "(function() { return [";
  if (files.size() > 0) {
    dumpFileEntryAsJson(str, files[0]);
  }
  for (size_t i = 1; i < files.size(); ++i) {
    str << ",";
    dumpFileEntryAsJson(str, files[i]);
  }
  str << "]; })()";
  *result = *eval(instance, str.str().c_str());
  return true;
}
*/
const char *stringFromNpVariant(const NPVariant &var) {
  char *argStringValue = new char[var.value.stringValue.UTF8Length + 1];
  memcpy(argStringValue, var.value.stringValue.UTF8Characters, var.value.stringValue.UTF8Length);
  argStringValue[var.value.stringValue.UTF8Length] = '\0';
  return argStringValue;
}

const char *byteArrayFromNpVariant(const NPVariant &var, const NPP &npp, size_t &length) {
  bool success = false;

  NPVariant lengthVariant;
  success = browserFuncs->getproperty(npp, var.value.objectValue, browserFuncs->getstringidentifier("length"), &lengthVariant);
  if (!success || lengthVariant.value.doubleValue > MAX_FILE_SIZE || lengthVariant.value.doubleValue < 0) {
    return NULL;
  }
  length = (size_t)lengthVariant.value.doubleValue;
  
  char *bytes = new char[length];
  if (bytes == NULL) {
    return NULL;
  }

  NPVariant element;
  char buffer[MAX_FILE_SIZE_WIDTH_IN_DECIMAL_WITH_SPACE_FOR_NULL_TERMINATOR];
  for (size_t i = 0; i < length; ++i) {
    sprintf(buffer, "%" SIZE_T_FORMAT_STRING, i);
    success = browserFuncs->getproperty(npp, var.value.objectValue, browserFuncs->getstringidentifier(buffer), &element);
    if (!success) {
      return false;
    }
    bytes[i] = (char)element.value.doubleValue;
  }

  return bytes;
}

//void deleteFileEntries(std::vector<FileEntry *> *entries) {
//  entries->clear();
//}
