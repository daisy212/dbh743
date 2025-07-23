#ifndef VERSION_H
#define VERSION_H


#define DB48X_VERSION "0.9.10"
#define PROGRAM_NAME    "DB48X"
#define PROGRAM_VERSION DB48X_VERSION


//#define DBu585		(1)
#define DBh743		(1)

#define Db_TEST (1)

#define HELPINDEX_NAME "/help/db48x.idx"
#define HELPFILE_NAME "/help/db48x.md"


#if  DBh743
#define HARD_NAME "DBh743"
#define HARD_VERSION "1.f"

#elif DBu585
#define HARD_NAME "DBu585"
#else
#define HARD_NAME "DBxxxx"
#endif
#define USE_EmFile	(DBh743 | DBu585)




#define BUILD_ID	1


#define HBUILD_ID	1



#endif // VERSION_H

