
/*
 * CodeQuery
 * Copyright (C) 2013-2017 ruben2020 https://github.com/ruben2020/
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef CTAGREAD_H_CQ
#define CTAGREAD_H_CQ

#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "sqlbase.h"

typedef std::vector<std::string> strctagIDList;

typedef struct
{
	std::string cls;
	std::string id;
} stClsID;

/**
 * @brief ctagread, handle ctags symbols. Mainly used for filling in class inheritance and members.
 *      When no class info found, the symbol will still be added/updated into the database.
 *      This for languages which cscope can not handle, eg javascript/html
 */
class ctagread : public sqlbase
{

public:
  enum enResult
  {
    resOK = 0,
    resSQLError,
    resFILE_NOT_FOUND,
    resFILE_ACCESS_ERR,
    resUNRECOG_FORMAT,
    resINCORRECT_VER,
    resUNSUPPORTED_PARAM,
    resOTHER_ERR,
    resUNKNOWN_ERR,
    resFILE_NOT_OPEN
  };

  ctagread();
  ~ctagread();
  enResult open_files(const char *sqldb, const char *tagsfn);
  void close_files(void);
  enResult process_ctags(void);
  enResult finalize(void);

private:
  FILE *f_tags;

  sqlite3_stmt *m_insertmember_stmt;
  sqlite3_stmt *m_insertinherit_stmt;
  sqlite3_stmt *m_readsymclass_stmt;
  sqlite3_stmt *m_readsym_stmt;
  sqlite3_stmt *m_readsymf_stmt;
  sqlite3_stmt *m_setsymname_stmt;
  sqlite3_stmt *m_setsymtype_stmt;
  sqlite3_stmt *m_readfile_stmt;
  sqlite3_stmt *m_readmaxfileid_stmt;
  sqlite3_stmt *m_insertfile_stmt;
  sqlite3_stmt *m_readmaxlineid_stmt;
  sqlite3_stmt *m_insertline_stmt;
  sqlite3_stmt *m_insertsymbol_stmt;
  sqlite3_stmt *m_readmaxsymbolid_stmt;

  enResult getListOfClassIDs(strctagIDList *idlist, const char *v1);
  enResult getHListOfClassIDs(strctagIDList *idlist, const char *v1, std::vector<stClsID> *listClsHist);
  enResult getListOfSymIDs(sqlite3_stmt *pstmt, strctagIDList *idlist, const char *v1, const char *v2, const char *v3);
  enResult prepare_cqdb(void);

  int getMaxId(sqlite3_stmt* stmt);
  
  /**
   * @return, fileId, 0 if not exist
   */
  int filePathExist(const char* fileName);
  int addSymbolFromTags(const char *symbol, char type, const char *file, const long int num);
  std::string getFileLineText(const char *file, const long int num);
};

#endif

