#pragma once

#include <cstdint>
#include <stdexcept>

namespace Rudesheim::Embedded
{
  class Undeletable
  {
  protected:
    virtual ~Undeletable() = default;
  };

  template< class Target > class Class:
    virtual Undeletable
  {
  protected:
    Class() = default;

  public:
    static auto SoleObject() -> Target const &
    {
      static Target
        object;

      return object;
    }
  };

  struct Deletable
  {
    virtual ~Deletable() = default;
  };

  class SiValue
  {
    uint64_t
      micro;

    constexpr explicit SiValue( uint64_t micro ):
      micro( micro )
    {
    }

    friend constexpr auto Micro( uint64_t value ) -> SiValue;
    friend constexpr auto Milli( uint64_t value ) -> SiValue;
    friend constexpr auto Base( uint64_t value ) -> SiValue;
    friend constexpr auto Kilo( uint64_t value ) -> SiValue;
    friend constexpr auto Mega( uint64_t value ) -> SiValue;

  public:
    constexpr auto AsMicro() const -> uint64_t
    {
      return micro;
    }

    constexpr auto AsMilli() const -> uint64_t
    {
      return micro / 1000ULL;
    }

    constexpr auto AsBase() const -> uint64_t
    {
      return micro / 1000000ULL;
    }

    constexpr auto AsKilo() const -> uint64_t
    {
      return micro / 1000000000ULL;
    }

    constexpr auto AsMega() const -> uint64_t
    {
      return micro / 1000000000000ULL;
    }
  };

  constexpr auto Micro( uint64_t value ) -> SiValue
  {
    return SiValue( value );
  }

  constexpr auto Milli( uint64_t value ) -> SiValue
  {
    return SiValue( value * 1000ULL );
  }

  constexpr auto Base( uint64_t value ) -> SiValue
  {
    return SiValue( value * 1000000ULL );
  }

  constexpr auto Kilo( uint64_t value ) -> SiValue
  {
    return SiValue( value * 1000000000ULL );
  }

  constexpr auto Mega( uint64_t value ) -> SiValue
  {
    return SiValue( value * 1000000000000ULL );
  }

  class Duration
  {
    SiValue
      second;

    constexpr explicit Duration( SiValue const &second ):
      second( second )
    {
    }

    friend constexpr auto Second( SiValue const &value ) -> Duration;

  public:
    constexpr auto AsSecond() const -> SiValue const &
    {
      return second;
    }
  };

  constexpr auto Second( SiValue const &value ) -> Duration
  {
    return Duration( value );
  }

  class Device
  {
  public:
    virtual auto BaudRate() const -> unsigned long
    {
      return 115200;
    }

    virtual auto Step() -> void
    {
    }

    virtual ~Device() = default;
  };

  auto EntryPoint() -> Device &;

  class Location
  {
    uint8_t
      number,
      controller;

  public:
    constexpr explicit Location( uint8_t number, uint8_t controller = 0 ):
      number( number ),
      controller( controller )
    {
    }

    constexpr auto Number() const -> uint8_t
    {
      return number;
    }

    constexpr auto Controller() const -> uint8_t
    {
      return controller;
    }
  };

  class Credentials
  {
    char const
      *id,
      *password;

  public:
    constexpr Credentials( char const *id, char const *password ):
      id( id ),
      password( password )
    {
    }

    constexpr auto Id() const -> char const*
    {
      return id;
    }

    constexpr auto Password() const -> char const*
    {
      return password;
    }
  };

  class IpAddress
  {
    uint8_t
      a, b, c, d;

  public:
    constexpr IpAddress( uint8_t a, uint8_t b, uint8_t c, uint8_t d ):
      a( a ), b( b ), c( c ), d( d )
    {
    }
  };

  class Error
  {
  protected:
    Error() = default;
    virtual ~Error() = default;
  };

  class WifiConnectionError:
    public Error,
    public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  namespace Option
  {
    class Signal:
      virtual public Undeletable
    {
    protected:
      Signal() = default;
    public:
      virtual auto Write( Location const &location, float value ) const -> void = 0;
      virtual auto Read( Location const &location ) const -> float = 0;
      virtual auto Resolution() const -> uint8_t = 0;

      auto MaxValue() const -> unsigned long
      {
        return ( 1UL << Resolution() ) - 1;
      }
    };

    class Mode:
      virtual public Undeletable
    {
    protected:
      Mode() = default;
    public:
      virtual auto ConfigureInput( Location const &location ) const -> void = 0;
      virtual auto ConfigureOutput( Location const &location ) const -> void = 0;
    };

    class PowerState;
    class Steady;
  }

  class Board:
    virtual public Undeletable
  {
  protected:
    Board() = default;

  public:
    virtual auto DigitalSignal() const -> Option::Signal const & = 0;
    virtual auto Analog8BitSignal() const -> Option::Signal const & = 0;
    virtual auto Analog12BitSignal() const -> Option::Signal const & = 0;

    virtual auto DefaultMode() const -> Option::Mode const & = 0;
    virtual auto PullUpMode() const -> Option::Mode const & = 0;

    virtual auto SteadyOn() const -> Option::Steady const & = 0;
    virtual auto SteadyOff() const -> Option::Steady const & = 0;

    virtual auto Wait( Duration const &duration ) const -> void = 0;

    virtual auto ConnectWifi( Credentials const &credentials ) const -> IpAddress = 0;
    virtual auto DisconnectWifi() const -> void = 0;

    virtual auto BeginCommunication( unsigned long baudRate ) const -> void
    {
    }

    virtual auto WaitSetupSerialPort() const -> void
    {
    }
  };

  Board extern const
    &board;

  namespace Pin
  {
    class Input:
      public Deletable
    {
      Location
        location;
      Option::Signal const
        &signal;
      Option::Mode const
        &mode;

    public:
      Input( Location const &location, Option::Signal const &signal = board.DigitalSignal(), Option::Mode const &mode = board.DefaultMode() );

      auto Read() const -> float;
    };

    class Output:
      public Deletable
    {
      Location
        location;
      Option::Signal const
        &signal;
      Option::Mode const
        &mode;

    public:
      Output( Location const &location, Option::Signal const &signal = board.DigitalSignal(), Option::Mode const &mode = board.DefaultMode() );

      auto Write( float value ) const -> void;
    };
  }

  namespace Option
  {
    class PowerState:
      virtual public Undeletable
    {
    protected:
      PowerState() = default;
    public:
      virtual auto Name() const -> char const* = 0;
      virtual auto Apply( Pin::Output const &pin, double scale = 1.0 ) const -> void = 0;
    };

    class Steady:
      virtual public PowerState
    {
    protected:
      Steady() = default;
    public:
      virtual auto Level() const -> float = 0;
      virtual auto Apply( Pin::Output const &pin, double scale = 1.0 ) const -> void override;
    };

    class PwmOn:
      virtual public PowerState
    {
      float
        level;
      Duration
        duration;

    public:
      PwmOn( float level, Duration duration ):
        level( level ),
        duration( duration )
      {
      }

      virtual auto Period() const -> Duration
      {
        return Second( Micro( 1000UL ) );
      }

      virtual auto Name() const -> char const* override;
      virtual auto Apply( Pin::Output const &pin, double scale = 1.0 ) const -> void override;
    };

    auto PowerStateFrom( bool value ) -> PowerState const &;
  }
}
