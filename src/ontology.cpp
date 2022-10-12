//
// Copyright Metaspex - 2022
// mailto:admin@metaspex.com
//

#include <stdlib.h>

#include "benchrisk/ontology.hpp"

namespace benchrisk {

  calculation::calculation(
			   currency::code cc,
			   unsigned int sn,
			   string ap,
			   string mn,
			   time_t cs,
			   time_t ed
  ):
    hx2a_base(standard),
    _currency(*this, cc),
    _sequence_number(*this, sn),
    _application(*this, ap),
    _machine_name(*this, mn),
    _computation_start_timestamp(*this, cs),
    _expiry_date(*this, ed),
    _extra_keys(*this, 25),
    _matrix(*this)
  {
  }

  void calculation::make(size_t rowsn, size_t colsn){
    matrix_r mat = make_rfr<matrix_type>();
    _matrix = &mat.get();
    mat.get().resize(rowsn, colsn);
  
    // Filling up the keys.
    for (auto& k: _extra_keys){ k = "FILLER"; }

    // Filling up the rows and columns with random heavy floating-point numbers and some amount of zeros.
    for (auto& r: *_matrix.get()){
      for (auto& d: *r.get()){
	if (rand() % 3){
	  d = 0;
	}
	else{
	  d = floating_point_type(rand() % 1000000) + floating_point_type(rand() % 100) / floating_point_type(100.0);
	}
      }
    }
  }
  
}
