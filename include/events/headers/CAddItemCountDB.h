#ifndef CADDITEMCOUNTDB_H
#define CADDITEMCOUNTDB_H

#include <common/defs.h>

// CAddItemCountDB declarations
EventClassify::CAddItemCountDB *__cdecl EventClassify::CAddItemCountDB::set_event_data(EventClassify::CAddItemCountDB *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CAddItemCountDB::is_event_item(EventClassify::CAddItemCountDB *this, int id);
int __cdecl EventClassify::CAddItemCountDB::is_event_quest(EventClassify::CAddItemCountDB *this, int id);
int __cdecl EventClassify::CAddItemCountDB::get_db_table_name(EventClassify::CAddItemCountDB *this);
EventClassify::CAddItemCountDB *__cdecl EventClassify::CAddItemCountDB::CAddItemCountDB(EventClassify::CAddItemCountDB *this);

#endif // CADDITEMCOUNTDB_H
