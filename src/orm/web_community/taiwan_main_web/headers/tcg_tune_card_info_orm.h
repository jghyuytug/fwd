#ifndef TCG_TUNE_CARD_INFO_ORM_H
#define TCG_TUNE_CARD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int seqidx;
    int no;
    int card_uninum;
    int count;
    int reg_date;
    char act_info[11];
} TcgTuneCardInfo;

/* CRUD Operations */
int TcgTuneCardInfo_Add(DBConnectionManager* manager, const TcgTuneCardInfo* record);
int TcgTuneCardInfo_Get(DBConnectionManager* manager, int seqidx, TcgTuneCardInfo* record);
int TcgTuneCardInfo_Update(DBConnectionManager* manager, const TcgTuneCardInfo* record);
int TcgTuneCardInfo_Delete(DBConnectionManager* manager, int seqidx);
int TcgTuneCardInfo_Exists(DBConnectionManager* manager, int seqidx);
int TcgTuneCardInfo_GetAll(DBConnectionManager* manager, TcgTuneCardInfo* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_CARD_INFO_ORM_H */