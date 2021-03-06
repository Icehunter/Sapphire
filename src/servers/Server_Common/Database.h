#ifndef _DATABASE_H
#define _DATABASE_H

#include <mutex>
#include <stdio.h>

#include <mysql.h>

#include <boost/shared_ptr.hpp>
#include <string.h>

namespace Core {
   namespace Db {

      // CField is used to access db-query resultsets
      class Field
      {
      public:

         // set value
         __inline void setValue( char* value )
         {
            m_pValue = value;
         }

         // return as string
         __inline const char *getString()
         {

            return m_pValue ? m_pValue : "";
         }

         // return as string
         __inline void getBinary( char* dstBuf, uint16_t size )
         {
            if( m_pValue )
            {
               memcpy( dstBuf, m_pValue, size );
            }
            else
            {
               dstBuf = NULL;
            }
         }

         //  return as float
         __inline float getFloat()
         {
            return m_pValue ? static_cast< float >( atof( m_pValue ) ) : 0;
         }

         // return as bool
         __inline bool getBool()
         {
            return m_pValue ? atoi( m_pValue ) > 0 : false;
         }

         // return as unsigned 8 bit integer
         __inline uint8_t getUInt8()
         {
            return m_pValue ? static_cast< uint8_t >( atol( m_pValue ) ) : 0;
         }

         // return as signed 8 bit integer
         __inline int8_t getInt8()
         {
            return m_pValue ? static_cast< int8_t >( atol( m_pValue ) ) : 0;
         }

         // return as unsigned 16 bit integer
         __inline uint16_t getUInt16()
         {
            return m_pValue ? static_cast< uint16_t >( atol( m_pValue ) ) : 0;
         }

         // return as signed 16 bit integer
         __inline int16_t getInt16()
         {
            return m_pValue ? static_cast< int16_t >( atol( m_pValue ) ) : 0;
         }

         // return as unsigned 32 bit integer
         __inline uint32_t getUInt32()
         {
            return m_pValue ? static_cast< uint32_t >( atol( m_pValue ) ) : 0;
         }

         // return as signed 8 bit integer
         __inline int32_t getInt32()
         {
            return m_pValue ? static_cast< int32_t >( atol( m_pValue ) ) : 0;
         }

         // return as unsigned 64 bit integer
         uint64_t getUInt64()
         {
            if( m_pValue )
            {
		#ifdef _WIN32
               uint64_t value;
               sscanf( m_pValue, "%I64d", &value );
               return value;
		#else
	       uint64_t value;
               sscanf( m_pValue, "%Lu", &value );
               return value;

		#endif
            }
            else
               return 0;
         }

      private:
         char *m_pValue;
      };


      class QueryResult
      {
      public:
         QueryResult( MYSQL_RES *res, uint32_t fields, uint32_t rows );
         ~QueryResult();

         bool nextRow();
         void Delete()
         {
            delete this;
         }

         __inline Field* fetch()
         {
            return m_currentRow;
         }
         __inline uint32_t getFieldCount() const
         {
            return m_fieldCount;
         }
         __inline uint32_t getRowCount() const
         {
            return m_rowCount;
         }

      protected:
         uint32_t m_fieldCount;
         uint32_t m_rowCount;
         Field *m_currentRow;
         MYSQL_RES *m_result;
      };

      struct DatabaseConnection
      {
         std::mutex lock;
         MYSQL *conn;
      };

      struct DatabaseParams
      {
         std::string hostname;
         std::string username;
         std::string password;
         std::string databaseName;
         uint16_t port;
         uint32_t bufferSize;
         uint32_t connectionCount;
      };

      class Database
      {
      public:
         Database();
         virtual ~Database();

         /************************************************************************/
         /* Virtual Functions                                                    */
         /************************************************************************/
         bool initialize( const DatabaseParams& params );

         void shutdown();

         boost::shared_ptr<QueryResult> query( const std::string& QueryString );
         QueryResult* queryNA( const char* QueryString );
         QueryResult* fQuery( const char * QueryString, DatabaseConnection *con );
         void fWaitExecute( const char * QueryString, DatabaseConnection *con );
         bool waitExecute( const char* QueryString, ... );//Wait For Request Completion
         bool waitExecuteNA( const char* QueryString );//Wait For Request Completion
         bool execute( const char* QueryString, ... );
         bool execute( const std::string& QueryString );
         bool executeNA( const char* QueryString );

         __inline const std::string& getHostName()
         {
            return m_hostname;
         }

         __inline const std::string& getDatabaseName()
         {
            return m_databaseName;
         }

         std::string escapeString( std::string Escape );
         void escapeLongString( const char * str, uint32_t len, std::stringstream& out );
         std::string escapeString( const char * esc, DatabaseConnection *con );

         void freeQueryResult( QueryResult * p );

         DatabaseConnection *getFreeConnection();

         void cleanupLibs();

         /* database is killed off manually. */
         void onShutdown() {}

         uint64_t getNextUId();

      protected:

         // actual query function
         bool _SendQuery( DatabaseConnection *con, const char* Sql, bool Self );
         QueryResult * _StoreQueryResult( DatabaseConnection * con );
         bool _HandleError( DatabaseConnection *conn, uint32_t ErrorNumber );
         bool _Reconnect( DatabaseConnection *conn );

         DatabaseConnection *m_pConnections;

         uint32_t _counter;
         ///////////////////////////////

         int32_t m_connectionCount;

         // For reconnecting a broken connection
         std::string m_hostname;
         std::string m_username;
         std::string m_password;
         std::string m_databaseName;
         uint32_t m_port;

      };

   }
}
#endif
