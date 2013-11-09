
#include "Utility/Packets/PacketDefinition.hpp"
#include "Utility/Packets/get.hpp"

#include <boost/fusion/adapted/struct.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/test/unit_test.hpp>

SARRY_PACKET(AlignedPacket,
  (boost::uint32_t,     a, Sarry::Utility::big_endian)
  (boost::uint16_t,     b, Sarry::Utility::little_endian)
  (boost::uint16_t,     c, Sarry::Utility::big_endian)
  (double,              d, Sarry::Utility::big_endian)
  (boost::int64_t,      e, Sarry::Utility::big_endian)
  (float,               f, Sarry::Utility::big_endian)
  (float,               g, Sarry::Utility::little_endian));

SARRY_PACKET(UnalignedPacket,
  (boost::uint32_t,     a, Sarry::Utility::big_endian)
  (boost::uint16_t,     b, Sarry::Utility::little_endian)
  (double,              c, Sarry::Utility::big_endian)
  (boost::int64_t,      d, Sarry::Utility::big_endian)
  (float,               e, Sarry::Utility::big_endian));
using namespace Sarry;

BOOST_AUTO_TEST_CASE( Sarry_AlignedPacketTest )
{
  const unsigned char alignStr[]
    = "\xff\xff\xff\xfe"//max - 1 big 32
      "\x02\x00"// 2 little
      "\x00\x02"// 2 big 
      "\x3f\xf3\xc0\xc1\xfc\x8f\x32\x38" // 1.23456 big double
      "\x80\x00\x00\x00\x00\x00\x00\x00" //min big
      "\x3f\x9e\x06\x10" //1.23456 big float
      "\x10\x06\x9e\x3f"; //1.23456 little float

  BOOST_AUTO(packet, Packets::get<Packets::AlignedPacket>(alignStr));
  BOOST_CHECK_EQUAL(packet.a, std::numeric_limits<boost::uint32_t>::max() - 1);
  BOOST_CHECK_EQUAL(packet.b, 2);
  BOOST_CHECK_EQUAL(packet.c, 2);
  BOOST_CHECK_CLOSE(packet.d, 1.23456, .0000001);
  BOOST_CHECK_EQUAL(packet.e, std::numeric_limits<boost::int64_t>::min());
  BOOST_CHECK_CLOSE(packet.f, 1.23456f, .0000001);
  BOOST_CHECK_CLOSE(packet.g, 1.23456f, .0000001);
}

BOOST_AUTO_TEST_CASE( Sarry_UnalignedPacketTest )
{
  const unsigned char unalignStr[]
    = "\xff\xff\xff\xfe"//max - 1 big 32
      "\x02\x00"// 2 little
      "\x3f\xf3\xc0\xc1\xfc\x8f\x32\x38" // 1.23456 big double
      "\x80\x00\x00\x00\x00\x00\x00\x00" //min big
      "\x3f\x9e\x06\x10"; //1.23456 big float

  BOOST_AUTO(packet, Packets::get<Packets::UnalignedPacket>(unalignStr));
  BOOST_CHECK_EQUAL(packet.a, std::numeric_limits<boost::uint32_t>::max() - 1);
  BOOST_CHECK_EQUAL(packet.b, 2);
  BOOST_CHECK_CLOSE(packet.c, 1.23456, .0000001);
  BOOST_CHECK_EQUAL(packet.d, std::numeric_limits<boost::int64_t>::min());
  BOOST_CHECK_CLOSE(packet.e, 1.23456f, .0000001);
}

