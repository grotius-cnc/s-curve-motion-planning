#ifndef SCURVE_GLOBAL_H
#define SCURVE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SCURVE_LIBRARY)
#  define SCURVE_EXPORT Q_DECL_EXPORT
#else
#  define SCURVE_EXPORT Q_DECL_IMPORT
#endif

#endif // SCURVE_GLOBAL_H
