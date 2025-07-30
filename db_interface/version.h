#ifndef VERSION_H
#define VERSION_H


#define DB48X_VERSION "0.9.11"
#define PROGRAM_NAME    "DB48X"
#define PROGRAM_VERSION DB48X_VERSION

// for tests.cc compilation
#define DBH_TSTS (1)

//#define DBu585     (1)
#define DBh743    (1)
#define USE_RNDIS (0)


#define Db_TEST (0)
//#define SIMULATOR (1)



#define HELPINDEX_NAME "/help/db48x.idx"
#define HELPFILE_NAME "/help/db48x.md"


#if  DBh743
#define HARD_NAME "DBh743"
#define HARD_VERSION "2.b"

#elif DBu585
#define HARD_NAME "DBu585"
#define HARD_VERSION "1.a"
#else
#define HARD_NAME "DBxxxx"
#endif
#define USE_EmFile   (DBh743 | DBu585)




#define BUILD_ID  1


#define HBUILD_ID 1



#endif // VERSION_H

