// Tests for a Boost Spirit-based LEF parser, part of EDASkel, a sample EDA app
// Copyright (C) 2010 Jeffrey Elliot Trull <linmodemstudent@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// This file contains some simple unit tests for the LEF parser
#define BOOST_TEST_MODULE basic LEF tests
#include <boost/test/included/unit_test.hpp>

#include "../lefparser.h"
#include <string>
#include <iostream>

lefparser<std::string::const_iterator> lefp;
lefskipper<std::string::const_iterator> skp;

BOOST_AUTO_TEST_CASE( case_check ) {

  std::string testlef("NAMESCASESENSITIVE ON ;");
  std::string::const_iterator beg = testlef.begin();
  std::string::const_iterator end = testlef.end();
  lef result;
  BOOST_CHECK( phrase_parse(beg, end, lefp, skp, result) );  // we should match
  BOOST_CHECK( (beg == end) );                         // we should consume all input
  // did not use BOOST_CHECK_EQUAL b/c it wants to output these on failure, and there is no operator<< defined
  BOOST_CHECK( result.macros.empty() );

}

BOOST_AUTO_TEST_CASE( macro_basic_check ) {

  std::string testlef("MACRO INX2\nCLASS CORE ;\n FOREIGN INX2 0.0 -1.0 ;\nORIGIN 0.0 1.0 ;\nSIZE 1.0 BY 10.0 ;\nSYMMETRY X Y ;\nEND INX2");
  std::string::const_iterator beg = testlef.begin();
  std::string::const_iterator end = testlef.end();
  lef result;
  BOOST_CHECK( phrase_parse(beg, end, lefp, skp, result) );  // we should match
  BOOST_CHECK( (beg == end) );                         // we should consume all input
  BOOST_REQUIRE_EQUAL( result.macros.size(), 1 );
  BOOST_CHECK_EQUAL( result.macros[0].name, "INX2" );
  BOOST_CHECK_EQUAL( result.macros[0].class_, SITECLASS_CORE );
  BOOST_REQUIRE( result.macros[0].foreign );
  BOOST_CHECK_EQUAL( result.macros[0].foreign->name, "INX2" );
  BOOST_CHECK_CLOSE( result.macros[0].foreign->pt.x, 0.0, 0.001f );
  BOOST_CHECK_CLOSE( result.macros[0].foreign->pt.y, -1.0, 0.001f );
  BOOST_REQUIRE( result.macros[0].origin );
  BOOST_CHECK_CLOSE( result.macros[0].origin->x, 0.0, 0.001f );
  BOOST_CHECK_CLOSE( result.macros[0].origin->y, 1.0, 0.001f );
  BOOST_REQUIRE( result.macros[0].size );
  BOOST_CHECK_CLOSE( result.macros[0].size->width, 1.0, 0.001f );
  BOOST_CHECK_CLOSE( result.macros[0].size->height, 10.0, 0.001f );
  BOOST_REQUIRE( result.macros[0].symmetry );
  BOOST_REQUIRE_EQUAL( result.macros[0].symmetry->size(), 2 );
  BOOST_CHECK_EQUAL( (*result.macros[0].symmetry)[0], SITESYM_X );
  BOOST_CHECK_EQUAL( (*result.macros[0].symmetry)[1], SITESYM_Y );
}

BOOST_AUTO_TEST_CASE( site_basic_check ) {

  std::string testlef("SITE MYSITENAME CLASS PAD ; SYMMETRY R90 ; SIZE 11.01 BY 22 ; END MYSITENAME");
  std::string::const_iterator beg = testlef.begin();
  std::string::const_iterator end = testlef.end();
  lef result;
  BOOST_CHECK( phrase_parse(beg, end, lefp, skp, result) );
  BOOST_CHECK( (beg == end) );
  BOOST_REQUIRE_EQUAL( result.sites.size(), 1 );
  BOOST_CHECK_EQUAL( result.sites[0].name, "MYSITENAME" );
  BOOST_CHECK_EQUAL( result.sites[0].class_, SITECLASS_PAD );
  BOOST_REQUIRE( result.sites[0].symmetry );
  BOOST_CHECK_EQUAL( result.sites[0].symmetry, SITESYM_R90 );
  BOOST_CHECK_CLOSE( result.sites[0].width, 11.01, 0.001f );
  BOOST_CHECK_CLOSE( result.sites[0].height, 22.0, 0.001f );

}
