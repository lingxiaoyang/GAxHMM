for f in waves/*.d; do NAME=`basename $f .d`; echo $NAME; ./addhtkheader.pl 44100 512 16 9 waves/$NAME.d > train/$NAME.feat; done
