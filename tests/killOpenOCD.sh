#!/bin/bash

echo "Will kill all existing OpenOCD and fpServer instances"

function kill_all_existing {
    # kill it even from other users, as only instance of OpenOCD can run at the same time
    # make sure you do not run these scripts in paraler 

    PIDS=`pidof $1`
    if [ "$PIDS" ];
    then
        if [[ "$(command -v sudo)" ]];
        then
            # use regular kill if sudo is not present
            kill $PIDS
        else
            # use sudo if it's found on the system
            sudo kill $PIDS
        fi 
    fi
    sleep 3

    PIDS=`pidof $1`
    if [ "$PIDS" ];
    then
        # agressive kill
        if [[ "$(command -v sudo)" ]];
        then
            # use regular kill if sudo is not present
            kill -9 $PIDS
        else
            # use sudo if it's found on the system
            sudo kill -9 $PIDS
        fi 
    fi
    sleep 2
    echo "Is $1 still running? PIDs:"
    pidof $1
}

echo "Making sure NO OpenOCD/fpServer is running"

kill_all_existing openocd
kill_all_existing fpServer
