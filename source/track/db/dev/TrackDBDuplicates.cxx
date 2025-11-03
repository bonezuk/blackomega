//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_TRACKDBDUPLICATES_H
#define __OMEGA_TRACK_DB_TRACKDBDUPLICATES_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "track/db/inc/Schema.h"
#include "track/db/inc/SQLiteInsert.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

/*

-------------------------------------------------------------------------------------------

Purpose : The music database, as it stands, is rather limited in its ability in being able
to sort an arbitrary collection of music media files such that they are uniquely organised
by album and track.

Condition for a Track record: A music media file contains meta-data (e.g. ID3 tag or name of
file) that can be used to identify it. This meta-data information is read and placed as a 
record into the Track table.

Condition for an Album record: An Album record is created if for a given track the associated 
(album name, year and directory) tuple data for the given track does not already exist.

Condition for a Subtrack record: The meta-data information in a given media file may indicate
that it can be consequtively subdivided with each division being given an associated name.

Condition for an Album Group: A collection of sub-directories of a common directory contain 
files whose associated (album name, year) pair are all the same. This group of album records
are pulled together to form an album group. This is a solution to an album being spread out
over multiple CDs.

There are a number of issues with the music database as it stands in sorting:
    - The location of a given file defines what album/group it belongs to.
    - The same track maybe represented by more than one file.
    - In forming an album/track based view there are actually four levels
      that have to be contented with group -> album -> track -> subtrack.

-------------------------------------------------------------------------------------------

Goal : To adapt the existing music database from its current state such given an arbitary
collection of media files such that albums are uniquely identified with the available track.
A given track can be stored in any number of known locations regardless of the underlying
file location and data encoding. These changes should be made in such a way that existing
functionality and logic described above does not change.

//-------------------------------------------------------------------------------------------

TODO Dictionary definition of album and track.

The use of the file location and associated meta-data has been used to provide a given
classification. However as already been mentioned this is now insuficient in handling the
requirements for a new music database model.

A new classification algorithm is to be developed that is transparent to file location.

The database schema is required to be extended such that the requirements of having a
album -> track -> location (child/parent) relationship can be created. The classification
algorithm will be used to populate the new database structures. The music model will then
employ this new database schema in order to populate its various views.

//-------------------------------------------------------------------------------------------

Break down of components

1. Extend the existing schema such that:
    - The existing functionality and logic are preserved.
    - Album -> track -> location (parent/child) relationship can be stored.

2. The new classification algorithm will be based on a statistical record linkage model.
In order to test the validity and tune the given classification algorithm a series of
test databases are required.
    - Create test database using existing code and scanning in contents of all music
      files in collection.
    - Create test database composed of the loaded contents of the FreeDB music database.

3. Develop interface(s) to encapsulate classification algortihm as black box such that it
can be used too:
    - Work as a way to populate the new database items when upgrading an existing
      music database.
    - Given a new media file resource is being added to the database then the
      associated new database items are created/updated as approriate.

4. Refactor/modify the existing track model to use the new database.

//-------------------------------------------------------------------------------------------

*/

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT TrackDBDuplicates
{
    public:
    
        class Album;

    public:
        TrackDBDuplicates();
        TrackDBDuplicates(SQLiteDatabase *db);
        virtual ~TrackDBDuplicates();
        
    protected:
    
        SQLiteDatabase *m_db;
};

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT TrackDBDuplicates::Album
{
    public:
        Album();
        Album(const Album& rhs);
        
        const Album& operator = (const Album& rhs);
        
    protected:
    
        QString m_albumName;
        int m_year;
        QVector<QString> m_artists;
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "track/db/inc/TrackDBDuplicates.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

TrackDBDuplicates::TrackDBDuplicates() : m_db(0)
{}

//-------------------------------------------------------------------------------------------

TrackDBDuplicates::TrackDBDuplicates(SQLiteDatabase *db) : m_db(db)
{}

//-------------------------------------------------------------------------------------------

TrackDBDuplicates::~TrackDBDuplicates()
{}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------