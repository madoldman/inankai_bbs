#!/bin/sh
# $Id: archie.sh,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
echo
echo "ËÑÑ°ÖÐ, ÇëÉÔºò....."
echo "Firebird BBS ARCHIE Service." `date` > /home/bbs/tmp/archie.$2.$3
echo "Search for: $1" >> /home/bbs/tmp/archie.$2.$3
exec /usr/local/bin/archie $1 >> /home/bbs/tmp/archie.$2.$3
