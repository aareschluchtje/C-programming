//
// Created by janco on 25-2-16.
//

#ifndef _Network_H
#define _Network_H
#include "typedefs.h"
bool hasNetworkConnection(void);
bool NetworkIsReceiving(void);
extern void NetworkInit(void);
extern void httpGet(char address[], void (*parser)(char*));
char* getMacAdress(void);

#endif /* _Network_H */
