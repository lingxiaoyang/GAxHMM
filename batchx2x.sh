for f in waves/*.cmp; do NAME=`basename $f .cmp`; echo $NAME; /usr/local/SPTK/bin/x2x +af waves/$NAME.cmp > waves/$NAME.d; done
