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

#ifndef MX_NODE_HPP
#define MX_NODE_HPP

#include "mx.hpp"
#include "../fx/mx_function.hpp"
#include "../matrix/matrix.hpp"
#include <vector>

namespace CasADi{

/** \brief Node class for MX objects
  \author Joel Andersson 
  \date 2010
  A regular user is not supposed to work with this Node class. Use Option directly.
*/
class MXNode : public SharedObjectNode{
friend class MX;
friend class MXFunctionInternal;

public:
  
  /** \brief  Constructor */
  explicit MXNode();

/** \brief  Destructor */
  virtual ~MXNode();

/** \brief  Clone function */
//  virtual MXNode* clone() const;

/** \brief  Print description */
  virtual void print(std::ostream &stream) const;

/** \brief  Evaluate the function and store the result in the node */
  virtual void evaluate(int fsens_order, int asens_order) = 0;

/** \brief  Initialize */
  virtual void init();
  
/** \brief  Get the name */
  virtual const std::string& getName() const;
  
/** \brief  Check if symbolic */
  virtual bool isSymbolic() const;

/** \brief  Check if constant */
  virtual bool isConstant() const;
    
/** \brief  Set/get input/output */
  void setOutput(const vector<double>& x);
  void getOutput(vector<double>& x) const;
  void setFwdSeed(const vector<double>& x, int dir=0);
  void getFwdSens(vector<double>& val, int dir=0) const;
  void setAdjSeed(const vector<double>& x, int dir=0);
  void getAdjSens(vector<double>& val, int dir=0) const;

  /** \brief  dependencies - functions that have to be evaluated before this one */
  MX& dep(int ind=0);
  const MX& dep(int ind=0) const;
  
  /** \brief  Number of dependencies */
  int ndep() const;

  /** \brief  Numerical value */
  const Matrix<double>& output() const;
  Matrix<double>& output();
  
  const std::vector<double>& input(int ind) const;
  std::vector<double>& input(int ind);
  
  const std::vector<double>& fwdSeed(int ind, int dir=0) const;
  std::vector<double>& fwdSeed(int ind, int dir=0);

  const std::vector<double>& adjSeed(int dir=0) const;
  std::vector<double>& adjSeed(int dir=0);
  
  const std::vector<double>& fwdSens(int dir=0) const;
  std::vector<double>& fwdSens(int dir=0);

  const std::vector<double>& adjSens(int ind, int dir=0) const;
  std::vector<double>& adjSens(int ind, int dir=0);

  protected:
    /// Set size
    void setSize(int nrow, int ncol);
    
    /// Set unary dependency
    void setDependencies(const MX& dep);
    
    /// Set binary dependencies
    void setDependencies(const MX& dep1, const MX& dep2);
    
    /// Set ternary dependencies
    void setDependencies(const MX& dep1, const MX& dep2, const MX& dep3);
    
    /// Set multiple dependencies
    void setDependencies(const std::vector<MX>& dep);
    
    //! Number of derivatives
    int maxord_;
    
    //! Number of derivative directions
    int nfdir_, nadir_;
    
    /** \brief  dependencies - functions that have to be evaluated before this one */
    std::vector<MX> dep_;
    
    /// Get size
    int size1() const;
    
    /// Get size
    int size2() const;
    
  private:
    /** \brief  Numerical value of output */
   Matrix<double> output_;

    /** \brief  Numerical value of forward sensitivities */
   std::vector<Matrix<double> > forward_sensitivities_;

    /** \brief  Numerical value of adjoint seeds */
   std::vector<Matrix<double> > adjoint_seeds_;

};

} // namespace CasADi


#endif // MX_NODE_HPP
