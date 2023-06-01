#ifndef __INFRARED_EMITTER_H
#define __INFRARED_EMITTER_H

#include "sys.h"

void InfraredEmitterInit(void);
void InfraredSendData(u8 data);

void airInfraredSendData(void);

void allowFraRecive(void);
void banFraRecive(void);
void infraSendAnyLengData(u8 *dataGroup,u8 length);
void cleanAllFraRecFlag(void);
void sendGLAirFraSign(u8 *airValues1,u8 *airValues2);
#endif
