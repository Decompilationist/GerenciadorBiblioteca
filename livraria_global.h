
#ifndef LIVRARIA_GLOBAL_H
#define LIVRARIA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIVRARIA_LIBRARY)
#  define LIVRARIA_EXPORT Q_DECL_EXPORT
#else
#  define LIVRARIA_EXPORT Q_DECL_IMPORT
#endif

#endif // LIVRARIA_GLOBAL_H
