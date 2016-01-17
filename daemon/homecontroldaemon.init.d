#!/bin/sh
#
# skeleton        example file to build /etc/init.d/ scripts.
#                This file should be used to construct scripts for /etc/init.d.
#
#                Written by Miquel van Smoorenburg <miquels@cistron.nl>.
#                Modified for Debian GNU/Linux
#                by Ian Murdock <imurdock@gnu.ai.mit.edu>.
#
# Version:        @(#)skeleton  1.8  03-Mar-1998  miquels@cistron.nl
#

### BEGIN INIT INFO
# Provides:          homecontroldaemon
# Required-Start:    $remote_fs $syslog $network
# Required-Stop:     $remote_fs $syslog $network
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description:
# Description:
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
NAME=homecontroldaemon
DAEMON=/usr/sbin/$NAME
DESC="homecontroldaemon"

case "$1" in
  start)
    # master switch
        start-stop-daemon --start --quiet --exec $DAEMON /dev/ttyHomecontrol >/dev/null 2>&1 &
    ;;
  stop)
    start-stop-daemon --stop --quiet --exec $DAEMON
    ;;
  force-reload|reload)
    exit 3
    ;;
  restart)
    $0 stop && $0 start
    ;;
  status)
    status_of_proc $DAEMON $NAME
    exit $?
    ;;
  *)
    echo "Usage: /etc/init.d/$NAME {start|stop|restart|status}" >&2
    exit 1
    ;;
esac

exit 0
