#ifndef DNF_SECURITY_TIP_PAGEVIEW_ORM_H
#define DNF_SECURITY_TIP_PAGEVIEW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int magazine_id;
    int pageview;
} DnfSecurityTipPageview;

/* CRUD Operations */
int DnfSecurityTipPageview_Add(DBConnectionManager* manager, const DnfSecurityTipPageview* record);
int DnfSecurityTipPageview_Get(DBConnectionManager* manager, int magazine_id, DnfSecurityTipPageview* record);
int DnfSecurityTipPageview_Update(DBConnectionManager* manager, const DnfSecurityTipPageview* record);
int DnfSecurityTipPageview_Delete(DBConnectionManager* manager, int magazine_id);
int DnfSecurityTipPageview_Exists(DBConnectionManager* manager, int magazine_id);
int DnfSecurityTipPageview_GetAll(DBConnectionManager* manager, DnfSecurityTipPageview* records, int max_count, int* actual_count);

#endif /* DNF_SECURITY_TIP_PAGEVIEW_ORM_H */