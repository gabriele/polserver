/*
History
=======

2005/03/01 Shinigami: added MAX_NUMER_REALMS check to prevent core crash (see MSGBF_SUB18_ENABLE_MAP_DIFFS)
2008/12/17 MuadDub:   Added check when loading Realms for no realms existing via int counter.
2009/08/25 Shinigami: STLport-5.2.1 fix: shadowrealm_count definition fixed

Notes
=======

*/

#include "realms.h"

#include "storage.h"
#include "uofile.h"
#include "los.h"
#include "polcfg.h"
#include "globals/uvars.h"

#include "../plib/realm.h"
#include "../plib/mapserver.h"
#include "../plib/systemstate.h"

#include "../clib/dirlist.h"
#include "../clib/passert.h"
#include "../clib/strutil.h"

#include "../clib/timer.h"
#include "../clib/logfacility.h"

namespace Pol {
  namespace Core {
	bool load_realms()
	{
	  Plib::Realm* temprealm;
	  int realm_counter = 0;
	  for ( Clib::DirList dl( Plib::systemstate.config.realm_data_path.c_str() ); !dl.at_end(); dl.next() )
	  {
		std::string realm_name = dl.name();
		if ( realm_name[0] == '.' )
		  continue;

		passert_r( gamestate.Realms.size() < MAX_NUMER_REALMS,
				   "You can't use more than " + Clib::decint( MAX_NUMER_REALMS ) + " realms" );

        POLLOG_INFO << "Loading Realm " << realm_name << ".\n";
		Tools::Timer<> timer;
		temprealm = new Plib::Realm( realm_name, Plib::systemstate.config.realm_data_path + realm_name );
        POLLOG_INFO << "Completed in " << timer.ellapsed( ) << " ms.\n";
		gamestate.Realms.push_back( temprealm );
		++realm_counter;

		//To-Fix - Nasty kludge assuming 'britannia' is the default realm
		//May want to make this configurable in later core releases.
		if ( realm_name == "britannia" )
		  gamestate.main_realm = temprealm;
	  }
	  //	main_realm = new DummyRealm();
	  gamestate.baserealm_count = realm_counter;
	  gamestate.shadowrealm_count = 0;
	  if ( realm_counter > 0 )
		return true;
	  else
		return false;
	}

	Plib::Realm* find_realm( const std::string& name )
	{
	  for ( auto &realm : gamestate.Realms )
	  {
		if ( realm->name() == name )
		  return realm;
	  }
	  return NULL;
	}

    bool defined_realm(const std::string& name)
	{
	  for ( const auto &realm : gamestate.Realms )
	  {
		if ( realm->name() == name )
		  return true;
	  }
	  return false;
	}

    void add_realm(const std::string& name, Plib::Realm* base)
	{
	  Plib::Realm* r = new Plib::Realm( name, base );
	  r->shadowid = ++gamestate.shadowrealm_count;
	  gamestate.shadowrealms_by_id[r->shadowid] = r;
	  gamestate.Realms.push_back( r );
	}

    void remove_realm(const std::string& name)
	{
	  std::vector<Plib::Realm*>::iterator itr;
	  for ( itr = gamestate.Realms.begin(); itr != gamestate.Realms.end(); ++itr )
	  {
		if ( ( *itr )->name() == name )
		{
		  gamestate.storage.on_delete_realm( *itr );
		  gamestate.shadowrealms_by_id[( *itr )->shadowid] = NULL; // used inside the decaythread
		  delete *itr;
		  gamestate.Realms.erase( itr );
		  break;
		}
	  }
	}
  }
}

