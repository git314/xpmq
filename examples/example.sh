#!/bin/bash

# small single time series with lag of 1
xpmq -p data/historical.csv -q data/qs.csv -f data/forecast.csv -o data/o.csv -l 1 -n

# small 2 time series with lag of 1
xpmq -p data2/historical.csv -q data2/qs.csv -f data2/forecast.csv -o data2/o.csv -l 1 -n

# small 2 time series with lag of 7 (weekly seasonality)
xpmq -p data3/historical.csv -q data3/qs.csv -f data3/forecast.csv -o data3/o.csv -l 7 -n

# small 3 time series with lag of 7 (weekly seasonality) from the M5 data set 
xpmq -p data_m5t1/insample.csv -q data_m5t1/qs.csv -f data_m5t1/forecast.csv -o data_m5t1/o.csv -l 7 -n

# larger 35 time series with lag of 7 (weekly seasonality) from the M5 data set 
xpmq -p data_m5t2/insample.csv -q data_m5t2/qs.csv -f data_m5t2/forecast.csv -o data_m5t2/o.csv -l 7 -n
