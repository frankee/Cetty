/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Mon Feb 28 13:58:57
 *********************************************************************/

#include "lrs.h"


Action()
{
    

    lrs_send("socket0", "buf4", LrsLastArg);

    lrs_receive("socket0", "buf5", LrsLastArg);

	lr_think_time(1);

    return 0;
}

