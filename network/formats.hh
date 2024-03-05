#pragma once

#include <cstdint>
#include <optional>

#include "base64.hh"
#include "crypto.hh"
#include "parser.hh"

struct VideoChunk
{
  uint32_t frame_index {}; /* index of this chunk (not video frame) */
  bool end_of_nal {};

  uint32_t nal_index {};

  using Buffer = StackBuffer<0, uint16_t, 512>;
  Buffer data {};

  uint16_t serialized_length() const;
  void serialize( Serializer& s ) const;
  void parse( Parser& p );

  static constexpr uint8_t frames_per_packet = 2;
};

template<typename T>
struct NetInteger
{
  T value {};

  static constexpr uint8_t serialized_length() { return sizeof( T ); }
  void serialize( Serializer& s ) const { s.integer( value ); }
  void parse( Parser& p ) { p.integer( value ); }

  NetInteger() {}
  NetInteger( const T other ) : value( other ) {}
  operator T() const { return value; }
  NetInteger& operator=( const T other )
  {
    value = other;
    return *this;
  }
};

template<typename T, uint8_t capacity_>
struct NetArray
{
  std::array<T, capacity_> elements {};
  uint8_t length {};

  static constexpr size_t capacity = capacity_;

  uint32_t serialized_length() const
  {
    if ( length > capacity ) {
      throw std::runtime_error( "invalid NetArray" );
    }

    uint32_t ret = sizeof( length );
    for ( uint8_t i = 0; i < length; i++ ) {
      ret += elements[i].serialized_length();
    }

    return ret;
  }

  void serialize( Serializer& s ) const
  {
    s.integer( length );
    for ( uint8_t i = 0; i < length; i++ ) {
      s.object( elements[i] );
    }
  }

  void parse( Parser& p )
  {
    p.integer( length );
    if ( length > capacity ) {
      p.set_error();
      return;
    }
    for ( uint8_t i = 0; i < length; i++ ) {
      p.object( elements[i] );
    }
  }

  std::span<const T> as_span_view() const { return std::span<const T> { elements.data(), length }; }
  operator std::span<const T>() const { return as_span_view(); }

  const T* begin() const { return as_span_view().begin(); }
  const T* end() const { return as_span_view().end(); }

  void push_back( const T& element )
  {
    if ( length >= capacity ) {
      throw std::out_of_range( "no room for NetArray::push_back" );
    }

    elements[length] = element;
    length++;
  }
};

class NetString : public StackBuffer<0, uint8_t, 255>
{
public:
  NetString() {}
  NetString( const std::string_view s )
  {
    resize( s.length() );
    std::copy( s.begin(), s.end(), mutable_buffer().begin() );
  }
};

template<class FrameType>
struct Packet
{
  struct Record
  {
    uint32_t sequence_number {};
    NetArray<NetInteger<uint32_t>, FrameType::frames_per_packet> frames {};
  };

  struct SenderSection
  {
    uint32_t sequence_number {};
    NetArray<FrameType, FrameType::frames_per_packet> frames {};

    Record to_record() const;
  } sender_section {};

  struct ReceiverSection
  {
    uint32_t next_frame_needed {};
    NetArray<NetInteger<uint32_t>, 32> packets_received {};
  } receiver_section {};

  NetString unreliable_data_ {};

  uint32_t serialized_length() const;
  void serialize( Serializer& s ) const;
  void parse( Parser& p );

  Packet() {}
  Packet( Parser& p ) { parse( p ); }
};

struct KeyMessage
{
  static constexpr char keyreq_id = uint8_t( 254 );
  static constexpr char keyreq_server_id = uint8_t( 255 );

  NetInteger<uint8_t> id {};
  KeyPair key_pair {};

  constexpr uint32_t serialized_length() const { return id.serialized_length() + key_pair.serialized_length(); }
  void serialize( Serializer& s ) const;
  void parse( Parser& p );
};
