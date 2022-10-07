//
// Copyright Metaspex - 2022
// mailto:admin@metaspex.com
//

#include <stdlib.h>
#include <iomanip>

#include <boost/timer/timer.hpp>

#include <hx2a/batch.hpp>
#include <hx2a/database_definition.hpp>
#include <hx2a/conf.hpp>

#include "benchrisk/ontology.hpp"

using namespace std;
using namespace hx2a;
using namespace benchrisk;

int main(){
  // Number of calculations saved. Accessing logical names defined in the configuration file.
  int count                  = config::get_number("number_of_calculations"); // Typically 10 calculations.
  size_t rowsn               = config::get_number("number_of_rows");         // Typically 5,000 rows.
  size_t colsn               = config::get_number("number_of_columns");      // Typically 250 columns.
  // Period of commits (number of calculations committed per commit).
  unsigned int commit_period = config::get_number("commit_period");          // Typically 1.

  int matrix_doc_size = 205410; // Typical figure in bytes, as measured from database.
  int row_doc_size = 3485; // Same.

  srand(0); // So that it is repeatable.

  cout << "Storing " << dec << count << " calculations, each of size ~" << setprecision(3) << (float)(matrix_doc_size + rowsn * row_doc_size) / 1000000 << " MB (" << (float)matrix_doc_size / 1000000 << " MB of calculation document and " << (float)(rowsn * row_doc_size) / 1000000 << " MB in rows documents), for a total volume of ~" << setprecision(3) << count * (float)(matrix_doc_size + rowsn * row_doc_size)/1000000 << " MB.\n"
	      << "Each matrix contains random floating point contents and has a size of " << rowsn << " x " << colsn << " (rows x columns).\n"
	      << "Saving a total of " << count * (1 + rowsn) << " documents.\n"
	      << "Committing at a period of " << commit_period << " calculations, with " << commit_period * (1 + rowsn) << " documents at each commit.\n";

  {
    database_id db_id = config::get_id("benchrisk");
    config::database_definition* db = config::get_database_definition(db_id);
    
    if (db){
      hx2a::ostringstream out;
      out << *db;
      cout << "Database definition " << out.str() << '\n';
    }
    else{
      cout << "Unknown database definition.\n";
    }
  }

  // Connecting to a physical database. The connection is made through a logical name, "benchrisk". Its
  // physical implementation is described in the configuration file.
  db::connector cnx("benchrisk");

  calculation_p c;
  
  {
    time_t t;

    // Timers.
    ::boost::timer::auto_cpu_timer tt("All (including construction): %ws wall, %us user + %ss system = %ts CPU (%p%)\n"); // Total.
    ::boost::timer::auto_cpu_timer ct("Commit exclusively:           %ws wall, %us user + %ss system = %ts CPU (%p%)\n"); // Only commit.
    ct.stop(); // With ct, we count only the commits, not the building of the calculation.
    // The commit time is in the first timer displayed (it is the shortest).
    
    for (int i = 0 ; i < count ; ++i){
      t = time();
      
      c = make_ptr<calculation>(
				currency::HX2A_EUR,
				i,
				"Risk Calculator",
				"machine 1",
				t,
				t + 8 * 3600 // 8 hours.
				);
      
      c->make(rowsn, colsn);
      
      if ((i % commit_period) == 0){
	ct.resume();
	commit();
	ct.stop();
	cout << '+' << flush;
      }
    }

    cout << '\n';

    cout << "Final commit...\n";

    ct.resume();
    commit();
  }

  // Displaying the id of the last one... 
  cout << "The last calculation has the document identifier: \"" << c->get_id() << "\".\n";
  // And the id of its first row. It should load the target document as commit has been done.
  cout << "Its first row has the document identifier: \"" << (*c->get_matrix().get().cbegin())->get_id() << "\".\n";
  cout << "Completed!\n";
  
  return 0;
}
