#!/bin/bash

LineId=
StationId=
StationEnName=
StationCnName=
LocationType=
LocationId=

MaxLine=99
MaxStation=9999

>basi.sql

for((LineId=90; LineId<=$MaxLine; LineId++))
do
	for((StationId=100; StationId<=$MaxStation; StationId++))
	do
	echo "insert into BASI_STATION_INFO values('$LineId', '$StationId',\
	'StationEnName', 'StationCnName', '01', '0246');" >> basi.sql
	done
done
