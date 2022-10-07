//
// Copyright Metaspex - 2022
// mailto:admin@metaspex.com
//

#ifndef BENCHRISK_ONTOLOGY_HPP
#define BENCHRISK_ONTOLOGY_HPP

#include <hx2a/root.hpp>
#include <hx2a/slot.hpp>
#include <hx2a/own.hpp>

// For the Foundation Ontology generic matrix type.
#include <hx2a/components/matrix.hpp>
// For the Foundation Ontology complete list of ISO 4217 currencies.
#include <hx2a/components/currency.hpp>

using namespace std;
using namespace hx2a;

namespace benchrisk {

  class calculation;
  using calculation_p = ptr<calculation>;
  using calculation_r = rfr<calculation>;

  // The type in the matrix can be float or double, or in a non-floating point application
  // anything which can be stored in a Metaspex slot.
  // Large matrices will be broken down in individual documents.
  // When part of a matrix is modified, Metaspex will save only the corresponding chunk,
  // not the whole matrix.
  using floating_point_type = float;
  using matrix_type = matrix<floating_point_type, "mat">;
  using matrix_p = ptr<matrix_type>;
  using matrix_r = rfr<matrix_type>;

  using row_type = matrix_type::row;
  using row_p = ptr<row_type>;
  using row_r = rfr<row_type>;

  // A number of document metadata are automatically generated and saved by Metaspex.
  // Among them:
  // - Creation timestamp.
  // - Last modification timestamp.
  // This is why they are not declared in the matrix document, as they are already present.
  
  class calculation: public root<>
  {
  public:

    HX2A_ROOT(calculation, "calc", 1, root);

    calculation(reserved_t, const doc_id& docid):
      root(reserved, docid),
      _currency(*this),
      _sequence_number(*this),
      _application(*this),
      _machine_name(*this),
      _computation_start_timestamp(*this),
      _expiry_date(*this),
      _extra_keys(*this),
      _matrix(*this)
    {
    }

    calculation(
		currency::code cc,
		unsigned int sn,
		string ap,
		string mn,
		time_t cs,
		time_t ed
		);

    size_type get_rows_size() const {
      return _matrix.get()->get_rows_size();
    }

    size_type get_columns_size() const {
      return _matrix.get()->get_columns_size();
    }

    currency::code get_currency() const {
      return _currency;
    }

    const currency::info* get_currency_info() const {
      return currency::get_info(_currency);
    }

    unsigned int get_sequence_number() const {
      return _sequence_number;
    }

    string get_application() const {
      return _application;
    }

    time_t get_expiry_date() const {
      return _expiry_date;
    }

    time_t get_computation_start_timestamp() const {
      return _computation_start_timestamp;
    }

    matrix_r get_matrix() const {
      return *_matrix.get();
    }

    void make(size_t rowsn, size_t colsn);
    
  private:

    // Metaspex's Foundation Ontology contains all currencies of the world, as defined by ISO 4217 standard.
    slot<currency::code, "c"> _currency;
    slot<unsigned int, "s">   _sequence_number;
    slot<string, "a">         _application;
    slot<string, "m">         _machine_name;
    slot<time_t, "t">         _computation_start_timestamp;
    slot<time_t, "e">         _expiry_date;
    slot_vector<string, "E">  _extra_keys;
    own<matrix_type, "r">     _matrix;
  };
  
}

#endif

