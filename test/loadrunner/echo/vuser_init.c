/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Mon Feb 28 13:58:57
 *********************************************************************/

#include "lrs.h"


vuser_init()
{
    lrs_startup(257);
	lrs_create_socket("socket0", "TCP", "RemoteHost=192.168.1.251:1980",  LrsLastArg);

    return 0;
}

