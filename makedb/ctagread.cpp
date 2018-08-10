
/*
 * CodeQuery
 * Copyright (C) 2013-2017 ruben2020 https://github.com/ruben2020/
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#include "ctagread.h"
#include "small_lib.h"
#include <iostream>
#include <fstream>

ctagread::ctagread()
:f_tags(NULL)
,m_insertmember_stmt(NULL)
,m_insertinherit_stmt(NULL)
,m_readsymclass_stmt(NULL)
,m_readsym_stmt(NULL)
,m_readsymf_stmt(NULL)
,m_setsymname_stmt(NULL)
,m_setsymtype_stmt(NULL)
,m_readfile_stmt(NULL)
,m_readmaxfileid_stmt(NULL)
{
}

ctagread::~ctagread()
{
	close_files();
}

ctagread::enResult ctagread::open_files(const char* sqldb, const char* tagsfn)
{
	int rc;
	f_tags = fopen(tagsfn, "r");
	if (f_tags == NULL) return resFILE_ACCESS_ERR;
	
	rc = sqlite3_open_v2(sqldb, &m_db, SQLITE_OPEN_READWRITE, NULL);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(m_db);
		return resFILE_ACCESS_ERR;
	}
	rc=sqlite3_exec(m_db, "PRAGMA synchronous = OFF;"
		"PRAGMA journal_mode = OFF;"
		"PRAGMA locking_mode = EXCLUSIVE;"
		"PRAGMA automatic_index = FALSE;"
		"PRAGMA cache_size = 20000;", NULL, 0, NULL);
	if (rc != SQLITE_OK) return resSQLError;
	return resOK;
}

void ctagread::close_files(void)
{
	sqlite3_reset(m_insertmember_stmt);
	sqlite3_reset(m_insertinherit_stmt);
	sqlite3_reset(m_readsymclass_stmt);
	sqlite3_reset(m_readsym_stmt);
	sqlite3_reset(m_readsymf_stmt);
	sqlite3_reset(m_setsymname_stmt);
	sqlite3_reset(m_setsymtype_stmt);
	sqlite3_reset(m_readfile_stmt);
	sqlite3_reset(m_readmaxfileid_stmt);
	sqlite3_reset(m_insertfile_stmt);
	sqlite3_reset(m_readmaxlineid_stmt);
	sqlite3_reset(m_insertline_stmt);
	sqlite3_reset(m_insertsymbol_stmt);
	sqlite3_reset(m_readmaxsymbolid_stmt);
	
	sqlite3_finalize(m_insertmember_stmt);
	sqlite3_finalize(m_insertinherit_stmt);
	sqlite3_finalize(m_readsymclass_stmt);
	sqlite3_finalize(m_readsym_stmt);
	sqlite3_finalize(m_readsymf_stmt);
	sqlite3_finalize(m_setsymname_stmt);
	sqlite3_finalize(m_setsymtype_stmt);
	sqlite3_finalize(m_readfile_stmt);
	sqlite3_finalize(m_readmaxfileid_stmt);
	sqlite3_finalize(m_insertfile_stmt);
	sqlite3_finalize(m_readmaxlineid_stmt);
	sqlite3_finalize(m_insertline_stmt);
	sqlite3_finalize(m_insertsymbol_stmt);
	sqlite3_finalize(m_readmaxsymbolid_stmt);
	
	m_insertmember_stmt = NULL;
	m_insertinherit_stmt = NULL;
	m_readsymclass_stmt = NULL;
	m_readsym_stmt = NULL;
	m_readsymf_stmt = NULL;
	m_setsymname_stmt = NULL;
	m_setsymtype_stmt = NULL;
	m_readfile_stmt = NULL;
	m_readmaxfileid_stmt = NULL;
	m_insertfile_stmt = NULL;
	m_readmaxlineid_stmt = NULL;
	m_insertline_stmt = NULL;
	m_insertsymbol_stmt = NULL;
	m_readmaxsymbolid_stmt = NULL;

	fclose(f_tags);
	f_tags = NULL;
	sqlite3_close(m_db);
	m_db = 0;
}

ctagread::enResult ctagread::prepare_cqdb(void)
{
	int rc;

	rc = prepare_stmt(&m_insertmember_stmt, "INSERT INTO membertbl VALUES (?,?,?);");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_insertinherit_stmt, "INSERT INTO inherittbl VALUES (?,?);");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_readsymclass_stmt, "SELECT symID FROM symtbl WHERE symName=? AND symType=\"c\";");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_readsym_stmt, "SELECT symtbl.symID FROM symtbl INNER JOIN linestbl ON (symtbl.symName=? AND symtbl.lineID = linestbl.lineID AND linestbl.linenum=?) INNER JOIN filestbl ON (linestbl.fileID = filestbl.fileID AND filePath LIKE ?);");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_readsymf_stmt, "SELECT symtbl.symID FROM symtbl INNER JOIN linestbl ON (symtbl.symName=? AND symtbl.symType=\"$\" AND symtbl.lineID = linestbl.lineID AND linestbl.linenum=?) INNER JOIN filestbl ON (linestbl.fileID = filestbl.fileID AND filePath LIKE ?);");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_setsymname_stmt, "UPDATE symtbl SET symName=? WHERE symID=?;");
	if (rc != SQLITE_OK) return resSQLError;

	rc = prepare_stmt(&m_setsymtype_stmt, "UPDATE symtbl SET symType=? WHERE symID=?;");
	if (rc != SQLITE_OK) return resSQLError;

    rc = prepare_stmt(&m_readfile_stmt, "SELECT fileID,filePath FROM filestbl WHERE filePath=?;");
	if (rc != SQLITE_OK) return resSQLError;

    rc = prepare_stmt(&m_readmaxfileid_stmt, "SELECT MAX(fileID) FROM filestbl;");
	if (rc != SQLITE_OK) return resSQLError;
	
    rc = prepare_stmt(&m_insertfile_stmt, "INSERT INTO filestbl VALUES (?,?);");
	if (rc != SQLITE_OK) return resSQLError;
	
    rc = prepare_stmt(&m_readmaxlineid_stmt, "SELECT MAX(lineID) FROM linestbl;");
	if (rc != SQLITE_OK) return resSQLError;
	
    rc = prepare_stmt(&m_insertline_stmt, "INSERT INTO linestbl VALUES (?,?,?,?);");
	if (rc != SQLITE_OK) return resSQLError;
	
    rc = prepare_stmt(&m_insertsymbol_stmt, "INSERT INTO symtbl VALUES (?,?,?,?);");
	if (rc != SQLITE_OK) return resSQLError;
	
    rc = prepare_stmt(&m_readmaxsymbolid_stmt, "SELECT MAX(symID) FROM symtbl;");
	if (rc != SQLITE_OK) return resSQLError;
	
	rc=sqlite3_exec(m_db,   "BEGIN EXCLUSIVE;"
				"DROP INDEX IF EXISTS memberIDIdx;"
				"DROP INDEX IF EXISTS groupIDIdx;"
				"DROP INDEX IF EXISTS parentIDIdx;"
				"DROP INDEX IF EXISTS childIDIdx;"
				"DELETE FROM membertbl;"
				"DELETE FROM inherittbl;"
				"COMMIT;", NULL, 0, NULL);

	if (rc != SQLITE_OK)
	{
		if (m_debug) printf("SQLErr13: %d, %s\n", rc, sqlite3_errmsg(m_db));
		return resSQLError;
	}

	return resOK;
}

ctagread::enResult ctagread::process_ctags(void)
{
	tempbuf sym(65501), fil(65501), classname(65501), classname2(65501);
	tempbuf numtxt(500), linetxt(65501), fil2(65501), nmspace(65501);
	long int num;
	long int numOfLines=0;
	long int numOfLines2=0;
	long int numOfSumbolsAdded=0;
	char* retval;
	int scanretval = 0;
	int rc;
	char c;
	char smallstr[2];
	char *cp;
	strctagIDList classIDs, symIDs, parentClassIDs, parentClassIDs_temp;
	enResult res;
	std::vector<stClsID> listClsHist;
	bool result;

	*(fil.get()) = '%'; // for SQL LIKE pattern recognition
	smallstr[1] = 0;
	res = prepare_cqdb();
	if (res != resOK) return res;
	
	do{
		retval = fgets(linetxt.get(), linetxt.size() - 1, f_tags);
		if (retval != NULL)
		{
            chomp(linetxt.get());
            if (m_debug) printf("line: %s\n", linetxt.get());
            scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c\tclass:%s\t", sym.get(), fil2.get(),
                                &num, &c, classname.get());
            if (m_debug) printf("scanretval: %d\n", scanretval);
        }
        else
		{
			if (m_debug) printf("retval==null\n");
		} 

		if ((retval != NULL)&&(scanretval == 5))
		{
			strcpy(fil.get(), "%");
			strcat(fil.get(), extract_filename(fil2.get()));
            res = getHListOfClassIDs(&classIDs, get_last_part(get_last_part(classname.get(), ':'), '.'),
                                     &listClsHist);

			if (res != resOK) return res;
			if (classIDs.empty()) continue;

			cp = sym.get();
			if (*(sym.get()) == '~')
			{
				cp = (sym.get()) + 1;   // include destructors
							// which cscope missed out
			}

			sprintf(numtxt.get(), "%ld", num);
			if (c == 'f') res = getListOfSymIDs(m_readsymf_stmt, &symIDs, cp, numtxt.get(), fil.get());
			else res = getListOfSymIDs(m_readsym_stmt, &symIDs, cp, numtxt.get(), fil.get());

			if (res != resOK) return res;

			if ((symIDs.empty() == true)&&(c == 'f'))
			{
				res = getListOfSymIDs(m_readsym_stmt, &symIDs, cp, numtxt.get(), fil.get());
				if (res != resOK) return res;

				if (symIDs.empty() == false)
				{
					for (unsigned long i = 0; i < symIDs.size(); i++)
					{
						rc = execstmt(m_setsymtype_stmt, "$", symIDs[i].c_str());
						if (rc != 0) return resSQLError;
					}
				}
			}

			if (symIDs.empty() == false)
			{
                for (unsigned long i=0; i < symIDs.size(); i++)
				{
					smallstr[0] = c;
					rc = execstmt(m_insertmember_stmt, classIDs[0].c_str(), symIDs[i].c_str(), smallstr);
					if (rc != 0) return resSQLError;

					if (*(sym.get()) == '~')
					{
						rc = execstmt(m_setsymname_stmt, sym.get(), symIDs[i].c_str());
						if (rc != 0) return resSQLError;
					}
					numOfLines++;
				}
			}
            else {if (m_debug) {printf("no match found for symbol: %s\n",sym.get());}}
        }

		if (retval != NULL)
		{
			scanretval = sscanf(linetxt.get(),
					"%s\t%s\t%ld;\"\t%c\tinherits:%s", sym.get(), fil2.get(), &num, &c, classname.get());

			result = ((scanretval == 5)&&(c == 'c'));

			if (!result)
			{
				scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c\tfile:\tinherits:%s", sym.get(), 
									fil2.get(), &num, &c, classname.get());

				result = ((scanretval == 5)&&(c == 'c'));
			}

			if (!result)
			{
				scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c\tclass:%s\tinherits:%s", sym.get(), 
									fil2.get(), &num, &c, classname2.get(), classname.get());

				result = ((scanretval == 6)&&(c == 'c'));
			}

			if (!result)
			{
				scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c\tnamespace:%s\tinherits:%s", sym.get(), 
									fil2.get(), &num, &c, nmspace.get(), classname.get());

				result = ((scanretval == 6)&&(c == 'c'));
			}

			if (!result)
			{
				scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c\tnamespace:%s\tfile:\tinherits:%s", 
									sym.get(), fil2.get(), &num, &c, nmspace.get(), classname.get());

				result = ((scanretval == 6)&&(c == 'c'));
			}

			if (result)
			{
				res = getHListOfClassIDs(&classIDs, sym.get(), &listClsHist);
				if (res != resOK) return res;
				if (classIDs.empty()) continue;

				parentClassIDs.clear();
				parentClassIDs_temp.clear();
				std::vector<std::string> vecstr = splitstr(classname.get(), ',');

                for (unsigned long i=0; i<vecstr.size(); i++)
				{
					res = getHListOfClassIDs(&parentClassIDs_temp, 
							get_last_part(get_last_part((char*)vecstr[i].c_str(), ':'), '.'), &listClsHist);
					if (res != resOK) return res;

					while (parentClassIDs_temp.empty() == false)
					{
						parentClassIDs.push_back(parentClassIDs_temp.back());
						parentClassIDs_temp.pop_back();
					}
				}
                for (unsigned long i=0; i<parentClassIDs.size(); i++)
				{
					rc=execstmt(m_insertinherit_stmt, parentClassIDs[i].c_str(), classIDs[0].c_str());
					if (rc!=0) return resSQLError;
					numOfLines2++;
				}
            }
            else
            {	// no class, inheritance, members etc found. Try to add/update symbol to db.
				c = '@';
				scanretval = sscanf(linetxt.get(), "%s\t%s\t%ld;\"\t%c", sym.get(), fil2.get(), &num, &c);

				result = (scanretval >= 4 && c != '@');
                if (result)
                {   // probably tags file for html/js/c# tags

                    if (m_debug) printf("%s sym=%s f=%s num=%ld c=%c\n", __func__, sym.get(), fil2.get(), num, c);
					
					// check whether symbol already exist
					symIDs.clear();
					sprintf(numtxt.get(), "%ld", num);
					res = getListOfSymIDs(m_readsym_stmt, &symIDs, sym.get(), numtxt.get(), fil2.get());

					if (res != resOK) continue;
                    if (!symIDs.empty()) continue; // symbol already exist in for name, file, place in file

                    rc = addSymbolFromTags(sym.get(), c, fil2.get(), num);

					if (rc != 0) continue;
					numOfSumbolsAdded++;
				}
			}
		}
	} while (retval != NULL);
    
    if (m_debug) printf("%s: Total membertbl records possible = %ld\n", __func__, numOfLines);
    if (m_debug) printf("%s: Total inherittbl records possible = %ld\n", __func__, numOfLines2);
    if (m_debug) printf("%s: Total #sybols added = %ld\n", __func__, numOfSumbolsAdded);
	return resOK;
}

ctagread::enResult ctagread::getHListOfClassIDs(strctagIDList* idlist, const char* v1, 
                                                std::vector<stClsID> *listClsHist)
{
	enResult res = resOK;
	idlist->clear();
    for (unsigned long i=0; i<listClsHist->size(); i++)
	{
		if ((*listClsHist)[i].cls.compare(v1) == 0)
		{idlist->push_back((*listClsHist)[i].id); break;}
	}
	if (idlist->empty())
	{
		res = getListOfClassIDs(idlist, v1);
		if (res != resOK) {return res;}
		if (idlist->empty() == false)
		{
			stClsID tempClsID;
			tempClsID.cls = v1;
			tempClsID.id = (*idlist)[0];
			(*listClsHist).insert((*listClsHist).begin(), tempClsID);
			if ((*listClsHist).size() > 50) (*listClsHist).pop_back();
		}
	}
	return resOK;
}

ctagread::enResult ctagread::getListOfClassIDs(strctagIDList* idlist, const char* v1)
{
	int rc;
	std::string s;

	idlist->clear();
	rc = execstmt(m_readsymclass_stmt, v1);
	while ((rc == SQLITE_ROW) || (rc == SQLITE_BUSY))
	{
		if (rc == SQLITE_ROW)
		{
			s = (const char *)sqlite3_column_text(m_readsymclass_stmt, 0);
			idlist->push_back(s);
		}
		rc = sqlite3_step(m_readsymclass_stmt);
	}
	return resOK;
}

ctagread::enResult ctagread::getListOfSymIDs(sqlite3_stmt* pstmt, strctagIDList* idlist, 
											 const char* v1, const char* v2, const char* v3)
{
	int rc;
	std::string s;

	idlist->clear();
	rc = execstmt(pstmt, v1, v2, v3);
	while ((rc == SQLITE_ROW) || (rc == SQLITE_BUSY))
	{
		if (rc == SQLITE_ROW)
		{
			s = (const char *)sqlite3_column_text(pstmt, 0);
			idlist->push_back(s);
		}
		rc = sqlite3_step(pstmt);
	}
	return resOK;
}

ctagread::enResult ctagread::finalize(void)
{
	int rc;
	std::string s;
	s  = "BEGIN EXCLUSIVE;";
	s += "CREATE INDEX groupIDIdx ON membertbl (groupID);";
	s += "CREATE INDEX memberIDIdx ON membertbl (memberID);";
	s += "CREATE INDEX parentIDIdx ON inherittbl (parentID);";
	s += "CREATE INDEX childIDIdx ON inherittbl (childID);";
	s += "REINDEX symNameIdx;";
	s += "REINDEX symName2Idx;";
	s += "COMMIT;";
	rc=sqlite3_exec(m_db, s.c_str(), NULL, 0, NULL);
	if (rc != SQLITE_OK)
	{
		if (m_debug) printf("SQLErr15: %d, %s\n", rc, sqlite3_errmsg(m_db));
		return resSQLError;
	}
	return resOK;
}

int ctagread::getMaxId(sqlite3_stmt* stmt)
{
	int maxId=0;
	int rc=0;

	rc = execstmt(stmt);
    while ((rc == SQLITE_ROW) || (rc == SQLITE_BUSY))
	{
		if (rc == SQLITE_ROW)
		{
            int tempId = (int)sqlite3_column_int(stmt, 0);
            maxId = (tempId>maxId) ? tempId : maxId;
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) break;
        }
	}
	return maxId;
}

int ctagread::filePathExist(const char* fileName)
{
	int fileId=0;
	int rc=0;
	std::string s;

	rc = execstmt(m_readfile_stmt, fileName);
	while ((rc == SQLITE_ROW) || (rc == SQLITE_BUSY))
	{
		if (rc == SQLITE_ROW)
		{
			fileId = sqlite3_column_int(m_readfile_stmt,0);
			s = (const char *)sqlite3_column_text(m_readfile_stmt, 1);
		}
		rc = sqlite3_step(m_readfile_stmt);
	}
	return fileId;
}

int ctagread::addSymbolFromTags(const char* symbol, char type, const char* file, const long int num)
{
    tempbuf fileIdTxt(0xff), lineIdTxt(0xff), numTxt(0xff), symIdTxt(0xff), typeTxt(0xf);
    int rc = 0;

    sprintf(numTxt.get(), "%ld", num);
    // check file exist, if not add, get file id
	int fileId = filePathExist(file);
    sprintf(fileIdTxt.get(), "%d", fileId);
	if (fileId == 0)
	{
		fileId = getMaxId(m_readmaxfileid_stmt)+1;
        sprintf(fileIdTxt.get(), "%d", fileId);
        rc = execstmt(m_insertfile_stmt, fileIdTxt.get(), file);
		if (rc != 0)
		{
            printf("%s: error insert file\n",__func__);
			return rc;
		}
	}

    // get text line info from file
    std::string lineTxt = getFileLineText(file, num);

	// check add lines table (linestbl)
	int lineId = getMaxId(m_readmaxlineid_stmt);
	++lineId;
    sprintf(lineIdTxt.get(), "%d", lineId);
    rc = execstmt(m_insertline_stmt, lineIdTxt.get(), numTxt.get(), fileIdTxt.get(), lineTxt.c_str());
    if (rc != 0)
    {
        printf("%s: error insert line\n",__func__);
        return rc;
    }

	int symId = getMaxId(m_readmaxsymbolid_stmt);
	++symId;
    sprintf(symIdTxt.get(), "%d", symId);
    sprintf(typeTxt.get(), "%c", type);
	
    // insert symbol
    rc = execstmt(m_insertsymbol_stmt, symIdTxt.get(), symbol, typeTxt.get(), lineIdTxt.get());
    if (rc != 0)
    {
        printf("%s: error insert symbol\n",__func__);
        return rc;
    }
    return rc;
}

std::string ctagread::getFileLineText(const char* file, const long int num)
{
    std::ifstream srcfile(file, std::ifstream::in);
	std::string line = "";

    for (int ix = 0; ix < num; ix++)
	{
        std::getline(srcfile,line);
	}
	size_t p = line.find_first_not_of(" \t");
	line.erase(0,p);
	return line;
}
