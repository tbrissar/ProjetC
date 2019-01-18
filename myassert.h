/*
 * Utiliser uniquement la macro myassert
 *    arg1 : booleen : si false, une erreur est déclenchée et le
 *                     programme s'arrête
 *    arg2 : string  : message à afficher en cas d'erreur
 *    note : definir la macro NDEBUG désactive le myassert
 */
#ifndef MYASSERT_H
#define MYASSERT_H

#include <stdbool.h>

void myassert_func(bool condition, const char *filename,
                   const char *functionName, int line);

#ifndef NDEBUG
   #define myassert(condition) myassert_func((condition), __FILE__, __func__, __LINE__)
#else
   #define myassert(condition)
#endif

#endif
