/*
* Compute the quantile function for the normal distribution. 
* Copyright (C) 2020 developers at deepsleeping.xyz
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

extern "C"{
#include "heads/qnorm.h"
}

#include <iostream>
#include <armadillo>
#define ARIMA_USE_OPENMP
#include "heads/cxxopts/include/cxxopts.hpp"
#include "heads/cpptqdm/tqdm.h"

using namespace std;
using namespace arma;

// function to print example command
std::string print_example ()
{
  std::string s;
  s = "\nxpmq -p examples/data/historical.csv -q examples/data/qs.csv -f examples/data/forecast.csv -o output.csv -m 0 -l 1 -n\n";
  return s;
}

std::string print_version ()
{
  std::string s;
  s = "\nversion: 0.0.1\n";
  return s;
}

std::string print_armadillo_version ()
{
  std::string a;
  std::string b;
  std::string c;
  std::string s;
  a = "Armadillo version: ";
  b = arma_version::as_string();
  c = "\n";
  s = a + b + c;
  return s;
}


int main(int argc, char** argv)
  {

  // begin cxx options
  cxxopts::Options options("xpmq", "Welcome to Xpoor-man's quantiles!\nCalcuate rough, cheap, and fast quantiles for any point forecast.");
  options.add_options()
      ("e,example", "See example. For all options `man xpmq`", cxxopts::value<bool>()->default_value("false"))
      ("p,past", "Path to historical or past (aka training) data csv", cxxopts::value<std::string>())
      ("f,future", "Path to future (forecasts) data csv", cxxopts::value<std::string>())
      ("q,quantiles", "Path to quantile (forecasts) data csv", cxxopts::value<std::string>())
      ("o,output", "Path to path to desired output file", cxxopts::value<std::string>())
      ("m,method", "Method for quantile creation. [0-3]", cxxopts::value<int>()->default_value("0"))
      ("l,lag", "Lags to seasonality", cxxopts::value<int>()->default_value("1"))
      ("n,no_negative_quantiles", "No Negatives. If quantiles need to be positive", cxxopts::value<bool>()->default_value("true"))
      ("v,version", "print xpmq version")
      ("a,armadillo_version", "print Armadillo Version")
      ("h,help", "Print usage")
      ("d,debug", "Enable debugging")
  ;

  auto result = options.parse(argc, argv);
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    exit(0);
  }
  bool debug = result["debug"].as<bool>();
  int lag;
  std::string future_path;
  std::string past_path;
  std::string quantiles_path;
  std::string example;
  std::string version;
  std::string armadillo_version;
  std::string output_path;
  std::string open_blas_threads;
  if (result.count("future"))
    future_path = result["future"].as<std::string>();
  if (result.count("past"))
    past_path = result["past"].as<std::string>();
  if (result.count("quantiles"))
    quantiles_path = result["quantiles"].as<std::string>();
  if (result.count("output"))
    output_path = result["output"].as<std::string>();
  if (result.count("lag"))
    lag = result["lag"].as<int>();
  if (result.count("example"))
    example = print_example();
    cout << example;
  if (result.count("version"))
    version = print_version();
    cout << version;
  if (result.count("armadillo_version"))
    armadillo_version = print_armadillo_version();
    cout << armadillo_version;
  char *openblas_threads = getenv("OPENBLAS_NUM_THREADS");
  openblas_threads == NULL ? std::string("None Found") : std::string(openblas_threads);
  cout << "OPENBLAS_NUM_THREADS: " << open_blas_threads << endl;
  // end cxx options
  
  // declare matrix to read
  mat past;
  vec qs;
  mat future;

  // load files
  cout << "reading " << past_path << endl;
  if(result.count("n")){
    cout << "logging past" << endl;
    past.load(past_path, csv_ascii);
    past=log1p(past);
    // past.load(past_path, csv_ascii);
  }else{
    past.load(past_path, csv_ascii);
  }
  cout << "reading " << quantiles_path << endl;
    qs.load(quantiles_path, csv_ascii);
  cout << "reading " << future_path << endl;
  if(result.count("n")){
    cout << "past and history will be log transformed for positive quantiles" << endl;
    future.load(future_path, csv_ascii);
    future = log1p(future);
  }else{
    future.load(future_path, csv_ascii);
  }

  // past.print("data:");
  cout << "past data ncols: " << past.n_cols << endl;
  cout << "past data nrows: " << past.n_rows << endl;
  cout << "forecast data ncols: " << future.n_cols << endl;
  cout << "forecast data nrows: " << future.n_rows << endl;

  int nrows = past.n_rows;
  float nlags = (float)nrows / (float)lag; 

  cout << "number of lag chunks (data_nrows / lag_size): " << nlags << endl;  // show data

  vec vec_stddev(nlags - 1, 1, fill::zeros);
  int past_ncols = past.n_cols;

  // define vars needed for next loop
  int nf = future.n_rows;     // number of rows in f
  int nqs = qs.n_rows;        // number of rows in qs
  int ni = past.n_cols;       // number of independent time series
  int qs_f = nf * nqs;
  int qs_x_f = nf * nqs * ni; // number of rows in final product

  cout << "expected rows in output (forecast nrows * #quantiles * #time series columns): " << qs_x_f << endl;

  // initialize with 0s
  arma::vec quantile_fcast(qs_x_f, fill::zeros);           // vector of quantile forecasts
  arma::vec quantile_fcast_u(qs_x_f, fill::zeros);         // u is the quoted name of the quantile 
  arma::vec quantile_fcast_index(qs_x_f, fill::zeros);     // 1 fcast index has many quantiles
  arma::vec quantile_fcast_index_ts(qs_x_f, fill::zeros);  // for each column make rows (tidy format)

  tqdm bar;
  for( int j = 0; j < past_ncols; j++ ){

    // print progress bar
    bar.progress(j, past_ncols);

    // get past for each column
    vec past_col_j = past.col(j);

    // some std -> armadillo type conversion code
    typedef std::vector<double> stdvec; // stdvec = vertical array of stdev
    std::vector<double> f = arma::conv_to< stdvec >::from(future.col(j));

    for(int i = 1; i < nlags; i++){
      // lag past by i rows
      // keep only `l` rows
      // difference the past by the lagged
      // calculate the sd and store sd in vector
      vec r = shift(past_col_j, i * lag);    // r = lagged vec
      int l = nrows - (i * lag);             // number of rows to show in tail
      vec r_clip = r.tail_rows(l);           // grab only the tail of lags - the non-zeroes
      vec v_clip = past_col_j.tail_rows(l);  // grab only the tail of actuals - the non-zeroes
      vec diff = v_clip - r_clip;            // calculate the difference of actuals vs lag
      double sd = stddev(diff);              // this is a scalar. double may not be enough percision
      vec_stddev.row(i-1) = sd;     
    }

    for(int i = 0; i < nf; i++){

      // calculate the stddev index needed to be applied to the forecast index
      // index needed is the integer part of the i / lags. 
      arma::vec q_vec(nqs, fill::zeros);  // for each column make rows (tidy format)
      int h = (float)i / (float)lag;  // integer part of i/lag
      
      for(int z = 0; z < nqs; z++){
        // calculate qnorm for each quantile in qs
        q_vec[z] = qnorm(qs[z], f[i], vec_stddev[h]);
      }
      int n = q_vec.n_rows;

      for(int k = 0; k < n; k++){

        // fill all vectors
        quantile_fcast[(k + (nqs * i + (j * qs_f)))] = q_vec[k];
        quantile_fcast_u[(k + (nqs * i + (j * qs_f)))] = qs[k];
        quantile_fcast_index[(k + (nqs * i + (j * qs_f)))] = i;
        quantile_fcast_index_ts[(k + (nqs * i + (j * qs_f)))] = j;

      }//end k

    }//end i

  }// end j
  bar.finish();
 
  // join all rows to one matrix
  mat iuf = join_rows(quantile_fcast_index_ts, quantile_fcast_index, quantile_fcast_u, quantile_fcast);

  // print final meta data
  cout << "result ncols: " << iuf.n_cols << endl;
  cout << "result nrows: " << iuf.n_rows << endl;

  // save in CSV format with a header
  field<std::string> header(iuf.n_cols);
  header(0) = "ts_index";
  header(0) = "qs_index";
  header(1) = "u"; // u = quantile name 
  header(2) = "q"; // q = quantile forecast for a given u

  cout << "writing " << output_path << endl;
  iuf.save( csv_name(output_path, header) );

  return 0;
  }
