/** @file brass_positionlist.h
 * @brief A position list in a brass database.
 */
/* Copyright (C) 2005,2006,2008,2009,2010,2011,2013 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef XAPIAN_HGUARD_BRASS_POSITIONLIST_H
#define XAPIAN_HGUARD_BRASS_POSITIONLIST_H

#include <xapian/types.h>

#include "bitstream.h"
#include "brass_lazytable.h"
#include "pack.h"
#include "backends/positionlist.h"

#include <string>

using namespace std;

class BrassPositionListTable : public BrassLazyTable {
  public:
    static string make_key(Xapian::docid did, const string & term) {
	string key;
	pack_string_preserving_sort(key, term);
	pack_uint_preserving_sort(key, did);
	return key;
    }

    /** Create a new BrassPositionListTable object.
     *
     *  This method does not create or open the table on disk - you
     *  must call the create() or open() methods respectively!
     *
     *  @param dbdir		The directory the brass database is stored in.
     *  @param readonly		true if we're opening read-only, else false.
     */
    BrassPositionListTable(const string & dbdir, bool readonly)
	: BrassLazyTable("position", dbdir + "/position.", readonly,
			 DONT_COMPRESS) { }

    /** Pack a position list into a string.
     *
     *  @param s The string to append the position list data to.
     */
    void pack(string & s, const std::vector<Xapian::termpos> & vec) const;

    /** Set the position list for term tname in document did.
     */
    void set_positionlist(Xapian::docid did, const string & tname,
			  const string & s) {
	add(make_key(did, tname), s);
    }

    /// Delete the position list for term tname in document did.
    void delete_positionlist(Xapian::docid did, const string & tname) {
	del(make_key(did, tname));
    }

    /// Return the number of entries in specified position list.
    Xapian::termcount positionlist_count(Xapian::docid did,
					 const string & term) const;
};

/** A position list in a brass database. */
class BrassPositionList : public PositionList {
    /// Interpolative decoder.
    BitReader rd;

    /// Current entry.
    Xapian::termpos current_pos;

    /// Last entry.
    Xapian::termpos last;

    /// Number of entries.
    Xapian::termcount size;

    /// Have we started iterating yet?
    bool have_started;

    /// Advance to next term position.
    void next_internal();

    /// Copying is not allowed.
    BrassPositionList(const BrassPositionList &);

    /// Assignment is not allowed.
    void operator=(const BrassPositionList &);

  public:
    /// Default constructor.
    BrassPositionList() { }

    /// Construct and initialise with data.
    BrassPositionList(const BrassTable * table, Xapian::docid did,
		      const string & tname) {
	(void)read_data(table, did, tname);
    }

    /** Fill list with data, and move the position to the start.
     *
     *  @return true if position data was read.
     */
    bool read_data(const string & data);

    /** Fill list with data, and move the position to the start.
     *
     *  @return true if position data was read.
     */
    bool read_data(const BrassTable * table, Xapian::docid did,
		   const string & tname);

    /// Returns size of position list.
    Xapian::termcount get_size() const;

    /** Returns current position.
     *
     *  Either next() or skip_to() must have been called before this
     *  method can be called.
     */
    Xapian::termpos get_position() const;

    /// Advance to the next term position in the list.
    void next();

    /// Advance to the first term position which is at least termpos.
    void skip_to(Xapian::termpos termpos);

    /// True if we're off the end of the list
    bool at_end() const;
};

#endif /* XAPIAN_HGUARD_BRASS_POSITIONLIST_H */
