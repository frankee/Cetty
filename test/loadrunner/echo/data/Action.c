/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Mon Feb 28 13:58:57
 *********************************************************************/

#include "lrs.h"


Action()
{
    lrs_create_socket("socket0", "TCP", "RemoteHost=SVCTAG-6L7FS2X:1980",  LrsLastArg);

    lrs_send("socket0", "buf0", LrsLastArg);

    lrs_receive("socket0", "buf1", LrsLastArg);

    lrs_send("socket0", "buf2", LrsLastArg);

    lrs_receive("socket0", "buf3", LrsLastArg);

    lrs_send("socket0", "buf4", LrsLastArg);

    lrs_receive("socket0", "buf5", LrsLastArg);

    lrs_send("socket0", "buf6", LrsLastArg);

    return 0;
}

