#!/bin/sh

#----- Print Usage !! -----#
if test "$#" -ne 1
then echo " "
     echo " Print_Usage !!"  
     echo " "  
     echo " make_ql_all.sh  year"
     echo " make_ql_all.sh  2003"
     echo " "  
     exit 0
fi

YEAR=$1

AWK='/usr/bin/awk'
JULCAL='/home/ste/hosokawa/go/julcal_n'
MAKEQL='/home/ste/hosokawa/go/make_ql_plot.sh'
SOURCEPATH='/data1/g3/nisitani/sddata/hokkaido/fitacf'
SUMPLOTDIR='/home/ste/hosokawa/sum/'

JDAY=1
#JDAY=300
if [ `expr $YEAR % 4` -eq 0 ]
then MAX_JDAY=366
else MAX_JDAY=365
fi

while test $JDAY -le $MAX_JDAY
do MONTH=`$JULCAL $JDAY $YEAR | $AWK '{printf ("%2.2d",$1)}'`
   DAY=`$JULCAL $JDAY $YEAR | $AWK '{printf ("%2.2d",$2)}'`
   INPUTDATE=$YEAR$MONTH$DAY
   SOURCEDIR=$SOURCEPATH/$YEAR/$MONTH/$DAY
   if [ -d $SOURCEDIR ] ; then
     FILENUM=`ls -1 $SOURCEDIR |wc -l`
     if [ $FILENUM -ne 0 ] ; then
       PNGFILE=${INPUTDATE}_hok_ql.png
       PSFILE=${INPUTDATE}_hok_ql.ps
       if [ ! -s $SUMPLOTDIR/$PNGFILE ] ; then
         $MAKEQL $INPUTDATE 2> /dev/null
         convert -density 100 $PSFILE $PNGFILE
         mv $PSFILE $PNGFILE $SUMPLOTDIR
       fi
     fi
   fi
   JDAY=`expr $JDAY + 1`
done

exit 0
