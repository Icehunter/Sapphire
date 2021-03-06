#ifndef _PLAYERSTATE_H
#define _PLAYERSTATE_H

#include <Server_Common/GamePacketNew.h>
#include "Player.h"
#include "Forwards.h"

namespace Core {
namespace Network {
namespace Packets {
namespace Server {

/**
* @brief The Ping response packet.
*/
class PlayerStateFlagsPacket :
   public GamePacketNew< FFXIVIpcPlayerStateFlags >
{
public:
   PlayerStateFlagsPacket( Entity::PlayerPtr pActor ) :
      GamePacketNew< FFXIVIpcPlayerStateFlags >( pActor->getId(), pActor->getId() )
   {
      initialize( pActor->getStateFlags() );
   }

   PlayerStateFlagsPacket( Entity::PlayerPtr pActor, std::vector< Common::PlayerStateFlag > flags ) :
      GamePacketNew< FFXIVIpcPlayerStateFlags >( pActor->getId(), pActor->getId() )
   {
      uint8_t newFlags[7];
      memset( newFlags, 0, 7 );

      for( auto& flag : flags )
      {
         int iFlag = static_cast< uint32_t >( flag );
         uint8_t index = iFlag / 8;
         uint8_t bitIndex = iFlag % 8;

         uint8_t value = 1 << bitIndex;

         newFlags[index] |= value;
      }

      initialize( newFlags );
   }

private:
   void initialize( const uint8_t* flags )
   {
      memcpy( m_data.flags, flags, 7 );
   };
};

}
}
}
}

#endif /*_PLAYERSTATE_H*/