/*! @file hash_compacting.h
 *  @brief Include this file to create hash tables that assign each unique key an ID.
 */

#ifndef HASH_COMPACTING__H
#define HASH_COMPACTING__H

#include "hash_table.h"

namespace CudaHT {
namespace CuckooHashing {

//! @class CompactingHashTable
/*! @brief Provides O(1) translation between keys and unique identifiers.
 *  @ingroup PublicInterface
 */
class CompactingHashTable : public HashTable {
 public:
  CompactingHashTable();
  virtual ~CompactingHashTable();

  //! Initializes the compacting hash table's memory.
  /*! See \ref HashTable::Initialize() for an explanation of the parameters.
   */
  virtual bool Initialize(const unsigned max_input_size,
                          const float    space_usage   = 1.25,
                          const unsigned num_functions = 4);

  //! Builds a compacting hash table.
  /*! See \ref HashTable::Build() for an explanation of the parameters.
   *  Keys may be repeated; the structure will compact them down into a list of
   *  unique keys and assign each a unique key an ID from 0 to K-1, where K is
   *  the number of unique keys.
   *  The values are not used with this structure and are ignored.
   */
  virtual bool Build(const unsigned  input_size,
                     const unsigned *d_keys,
                     const unsigned *d_vals);

  //! Queries the hash table for the unique identifiers of the query keys.
  /*! @param[in] n_queries        Number of keys in the query set.
   *  @param[in] d_query_keys     Device memory array containing all of the query keys.
   *  @param[in] d_query_results  Values for the query keys.
   *
   *  \ref kNotFound is returned for any query key that failed to be found in the table.
   */
  virtual void Retrieve(const unsigned  n_queries,
                        const unsigned *d_query_keys,
                              unsigned *d_query_results);

  //! Releases all of the memory.
  virtual void Release();

  //! Returns the number of unique keys found in the input.
  inline unsigned         get_unique_keys_size()   const {return unique_keys_size_;}

  //! Returns the unique keys found in the input.
  inline const unsigned*  get_unique_keys()        const {return d_unique_keys_;}

 private:
  //! Number of unique keys found in the hash table.
  unsigned  unique_keys_size_;

  //! Compacted list of the unique keys.
  unsigned *d_unique_keys_;

  // Scratch space.
  size_t    scanplan_;
  unsigned *d_scratch_cuckoo_keys_;
  unsigned *d_scratch_counts_;
  unsigned *d_scratch_unique_ids_;
};

};  // namespace CuckooHashing
};  // namespace CudaHT

#endif
