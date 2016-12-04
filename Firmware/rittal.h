#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "defines.h"          // project folder
#include "structs.h"	// project folder
#include "globals.h"          // Globale Variablen.

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RITTAL_H_   /* Include guard */
#define RITTAL_H_

void rittal_init();
void rittal_task();

#endif // RITTAL_H_ 

#ifdef __cplusplus
}
#endif