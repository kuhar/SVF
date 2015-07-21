/* SVF - Static Value-Flow Analysis Framework 
Copyright (C) 2015 Yulei Sui
Copyright (C) 2015 Jingling Xue

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * SaberCheckerAPI.h
 *
 *  Created on: Apr 23, 2014
 *      Author: Yulei Sui
 */

#ifndef SABERCHECKERAPI_H_
#define SABERCHECKERAPI_H_

#include "Util/AnalysisUtil.h"

/*
 * Saber Checker API class contains interfaces for various bug checking
 * memory leak detection e.g., alloc free
 * incorrect file operation detection, e.g., fopen, fclose
 */
class SaberCheckerAPI {

public:
    enum CHECKER_TYPE {
        CK_DUMMY = 0,		/// dummy type
        CK_ALLOC,		/// memory allocation
        CK_FREE,      /// memory deallocation
        CK_FOPEN,		/// File open
        CK_FCLOSE		/// File close
    };

    typedef llvm::StringMap<CHECKER_TYPE> TDAPIMap;

private:
    /// API map, from a string to threadAPI type
    TDAPIMap tdAPIMap;

    /// Constructor
    SaberCheckerAPI () {
        init();
    }

    /// Initialize the map
    void init();

    /// Static reference
    static SaberCheckerAPI* ckAPI;

    /// Get the function type of a function
    inline CHECKER_TYPE getType(const llvm::Function* F) const {
        if(F) {
            TDAPIMap::const_iterator it= tdAPIMap.find(F->getName().str());
            if(it != tdAPIMap.end())
                return it->second;
        }
        return CK_DUMMY;
    }

public:
    /// Return a static reference
    static SaberCheckerAPI* getCheckerAPI() {
        if(ckAPI == NULL) {
            ckAPI = new SaberCheckerAPI();
        }
        return ckAPI;
    }

    /// Return true if this call is a memory allocation
    //@{
    inline bool isMemAlloc(const llvm::Function* fun) const {
        return getType(fun) == CK_ALLOC;
    }
    inline bool isMemAlloc(const llvm::Instruction *inst) const {
        return getType(analysisUtil::getCallee(inst)) == CK_ALLOC;
    }
    inline bool isMemAlloc(llvm::CallSite cs) const {
        return isMemAlloc(cs.getInstruction());
    }
    //@}

    /// Return true if this call is a memory deallocation
    //@{
    inline bool isMemDealloc(const llvm::Function* fun) const {
        return getType(fun) == CK_FREE;
    }
    inline bool isMemDealloc(const llvm::Instruction *inst) const {
        return getType(analysisUtil::getCallee(inst)) == CK_FREE;
    }
    inline bool isMemDealloc(llvm::CallSite cs) const {
        return isMemDealloc(cs.getInstruction());
    }
    //@}

    /// Return true if this call is a file open
    //@{
    inline bool isFOpen(const llvm::Function* fun) const {
        return getType(fun) == CK_FOPEN;
    }
    inline bool isFOpen(const llvm::Instruction *inst) const {
        return getType(analysisUtil::getCallee(inst)) == CK_FOPEN;
    }
    inline bool isFOpen(llvm::CallSite cs) const {
        return isFOpen(cs.getInstruction());
    }
    //@}

    /// Return true if this call is a file close
    //@{
    inline bool isFClose(const llvm::Function* fun) const {
        return getType(fun) == CK_FCLOSE;
    }
    inline bool isFClose(const llvm::Instruction *inst) const {
        return getType(analysisUtil::getCallee(inst)) == CK_FCLOSE;
    }
    inline bool isFClose(llvm::CallSite cs) const {
        return isFClose(cs.getInstruction());
    }
    //@}

};


#endif /* SABERCHECKERAPI_H_ */
