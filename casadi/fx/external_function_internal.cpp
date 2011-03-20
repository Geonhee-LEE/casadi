/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "external_function_internal.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

// The following works for Linux, something simular is needed for Windows
#ifdef WITH_JIT 
#include <dlfcn.h>
#endif // WITH_JIT 

namespace CasADi{

using namespace std;

ExternalFunctionInternal::ExternalFunctionInternal(const std::string& bin_name) : bin_name_(bin_name){
#ifdef WITH_JIT 

  // Load the dll
  handle_ = 0;
  handle_ = dlopen(bin_name_.c_str(), RTLD_LAZY);    
  if (!handle_) {
    stringstream ss;
    ss << "ExternalFunctionInternal: Cannot open function: " << bin_name_ << ". error code: "<< dlerror();
    throw CasadiException(ss.str());
  }
  dlerror(); // reset error

  // Initialize and get the number of inputs and outputs
  initPtr init = (initPtr)dlsym(handle_, "init");
  if(dlerror()) throw CasadiException("ExternalFunctionInternal: no \"init\" found");
  int n_in, n_out;
  int flag = init(&n_in, &n_out);
  if(flag) throw CasadiException("ExternalFunctionInternal: \"init\" failed");
  
  // Pass to casadi
  input_.resize(n_in);
  output_.resize(n_out);

  // Get the size of the inputs
  getInputSizePtr getInputSize = (getInputSizePtr)dlsym(handle_, "getInputSize");
  if(dlerror()) throw CasadiException("ExternalFunctionInternal: no \"getInputSize\" found");

  for(int i=0; i<n_in; ++i){
    int nrow, ncol;
    flag = getInputSize(i,&nrow,&ncol);
    if(flag) throw CasadiException("ExternalFunctionInternal: \"getInputSize\" failed");
    input(i).resize(nrow,ncol);
  }

  // Get the size of the inputs
  getOutputSizePtr getOutputSize = (getOutputSizePtr)dlsym(handle_, "getOutputSize");
  if(dlerror()) throw CasadiException("ExternalFunctionInternal: no \"getOutputSize\" found");

  for(int i=0; i<n_out; ++i){
    int nrow, ncol;
    flag = getOutputSize(i,&nrow,&ncol);
    if(flag) throw CasadiException("ExternalFunctionInternal: \"getOutputSize\" failed");
    output(i).resize(nrow,ncol);
  }
  
  evaluate_ = (evaluatePtr) dlsym(handle_, "evaluate");
  if(dlerror()) throw CasadiException("ExternalFunctionInternal: no \"evaluate\" found");
  
#else // WITH_JIT 
  throw CasadiException("WITH_JIT  not activated");
#endif // WITH_JIT 
  
}
    
ExternalFunctionInternal* ExternalFunctionInternal::clone() const{
  throw CasadiException("Error ExternalFunctionInternal cannot be cloned");
}

ExternalFunctionInternal::~ExternalFunctionInternal(){
#ifdef WITH_JIT 
  // close the dll
  if(handle_) dlclose(handle_);
#endif // WITH_JIT 
}

void ExternalFunctionInternal::evaluate(int nfdir, int nadir){
#ifdef WITH_JIT 
  int flag = evaluate_(&input_array_[0],&output_array_[0]);
  if(flag) throw CasadiException("ExternalFunctionInternal: \"evaluate\" failed");
#endif // WITH_JIT 
}
  
void ExternalFunctionInternal::init(){
  // Call the init function of the base class
  FXInternal::init();

  // Get pointers to the inputs
  input_array_.resize(input_.size());
  for(int i=0; i<input_array_.size(); ++i)
    input_array_[i] = &input(i)[0];

  // Get pointers to the outputs
  output_array_.resize(output_.size());
  for(int i=0; i<output_array_.size(); ++i)
    output_array_[i] = &output(i)[0];
}




} // namespace CasADi

