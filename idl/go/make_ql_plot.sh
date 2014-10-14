#!/bin/sh

#----- Print Usage !! -----#
if test "$#" -ne 1
then echo " "
     echo " Print_Usage !!"  
     echo " "  
     echo " make_ql_plot.sh  date"
     echo " make_ql_plot.sh  20061220"
     echo " "  
     exit 0
fi

GO=/home/ste/hosokawa/go/go_ps
DATE=$1

echo plot_hok_ql,$DATE >  tmp.go
echo exit              >> tmp.go

$GO tmp.go

rm -f tmp.go

