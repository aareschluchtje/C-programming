//
// Created by janco on 25-2-16.
//

#ifndef _Ntp_H
#define _Ntp_H

#include "typedefs.h"
#include <time.h>

extern bool NtpIsSyncing(void);
extern void NtpInit(void);
extern int NtpSync(void);
extern void setTimeZone(int timezone);
extern bool NtpTimeIsValid(void);

void NtpCheckValidTime(void);
void NtpWriteTimeToEeprom(tm time_struct);

void setTimeZone(int timezone);
int getTimeZone(void);
#endif /* _Ntp_H */
