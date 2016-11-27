#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "misc.h"             // project folder
#include "globals.h"        // Globale Variablen.

#ifdef __cplusplus
extern "C" {
#endif

#ifndef POWER_HUB_H_   /* Include guard */
#define POWER_HUB_H_

void power_hub_init();
void power_hub_task();

#endif

#ifdef __cplusplus
}
#endif