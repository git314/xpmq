# X-poorman's-quantiles (xpmq)

### Description

Many of the current quantile forecasting algorithms are tied to models. There are 
cases where this is not desirable.

For example, it may be that a new forecaster entered a code base with
a model running which is not probabilistic in nature. One may ask
if new models must be made to ascertain uncertainty. 

`xpmq` attempts to be a fast library for computing model-independent 
quantiles with simple methods which are relatively cheap.

The purpose of this library is not to 
provide ground-breakingly accurate quantile forecasts, but possibly 
'good-enough' quantile forecasts which run fast for many thousands of 
time series. 

### Dependencies
`Armadillo`


### Installation

clone and cd into the directory and run `sudo make clean`

### Manpage

```
Welcome to Xpoor-man's quantiles!
Calcuate rough, cheap, and fast quantiles for any point forecast.
Usage:
  xpmq [OPTION...]

  -e, --example                See example commands.
  -p, --past arg               Path to historical or past (aka training) data
                               csv
  -f, --future arg             Path to future (forecasts) data csv
  -q, --quantiles arg          Path to quantile (forecasts) data csv
  -o, --output arg             Path to path to desired output file
  -m, --method arg             Method for quantile creation. [0-3] (default:
                               0)
  -l, --lag arg                Lags to seasonality (default: 1)
  -n, --no_negative_quantiles  No Negatives. If quantiles need to be positive
                               (default: true)
  -v, --version                print xpmq version
  -a, --armadillo_version      print Armadillo Version
  -h, --help                   Print usage
  -d, --debug                  Enable debugging
```

### Usage 

```
xpmq -p examples/data_m5t1/insample.csv -q examples/data_m5/qs.csv -f examples/data_m5t1/forecast.csv -o o_m5.csv -l 7 -n
```

![xpmq-gif](imgs/xpmq.gif)

# Features

1. CLI first 
2. Armadillo C++ with links to OpenBLAS
3. Build for large data
4. Tested on M5 data
5. Computationally cheap
6. Model independent

### Docker Image

Location: TBD

Examples: TBD

### FAQ

For help see `xpmq -h`

### LICENSE

GLP version 3
