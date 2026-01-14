#ifndef CEVENTENTITY_H
#define CEVENTENTITY_H

#include <common/defs.h>

// CEventEntity declarations
char *__cdecl EventClassify::CEventEntity::CEventEntity(EventClassify::CEventEntity *this);
int __cdecl EventClassify::CEventEntity::is_eventing(EventClassify::CEventEntity *this);
EventClassify::CEventEntity *__cdecl EventClassify::CEventEntity::set_event_flag(EventClassify::CEventEntity *this, bool a2);
EventClassify::CEventEntity *__cdecl EventClassify::CEventEntity::set_event_data(EventClassify::CEventEntity *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CEventEntity::get_event_record_type(EventClassify::CEventEntity *this);
EventClassify::CEventEntity *__cdecl EventClassify::CEventEntity::set_event_record_type(EventClassify::CEventEntity *this, int id);
char *__cdecl EventClassify::CEventEntity::get_db_table_name(EventClassify::CEventEntity *this);
EventClassify::CEventEntity *__cdecl EventClassify::CEventEntity::set_event_start_time(EventClassify::CEventEntity *this, int id);
EventClassify::CEventEntity *__cdecl EventClassify::CEventEntity::set_event_end_time(EventClassify::CEventEntity *this, int id);
int __cdecl EventClassify::CEventEntity::get_event_start_time(EventClassify::CEventEntity *this);
int __cdecl EventClassify::CEventEntity::get_event_end_time(EventClassify::CEventEntity *this);
int EventClassify::CEventEntity::send_init_data();
int `global constructor keyed to'EventClassify::CEventEntity::CEventEntity();

#endif // CEVENTENTITY_H
