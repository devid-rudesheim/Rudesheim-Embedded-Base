#include "Rudesheim-Embedded-Base.hpp"

namespace Rudesheim::Embedded
{
  auto Duration::Wait() const -> void
  {
    board.Wait( *this );
  }

  namespace Option
  {
    auto Steady::Apply( Pin::Output const &pin, double ) const -> void
    {
      pin.Write( Level() );
    }

    auto PwmOn::Name() const -> char const*
    {
      return "PWM_ON";
    }

    auto PwmOn::Apply( Pin::Output const &pin, double scale ) const -> void
    {
      auto const
        period = Period().AsMicroseconds();

      auto const
        onTime = static_cast< unsigned long >( period * level * scale + 0.5 ),
        cycles = duration.AsMicroseconds() / period;

      for( auto cycle = 0U; cycle < cycles; ++cycle )
      {
        board.SteadyOn().Apply( pin, scale );
        Microseconds( onTime ).Wait();

        board.SteadyOff().Apply( pin, scale );
        Microseconds( period - onTime ).Wait();
      }
    }

    auto PowerStateFrom( bool value ) -> PowerState const &
    {
      return value ? board.SteadyOn() : board.SteadyOff();
    }
  }

  namespace Pin
  {
    Input::Input( Location const &location, Option::Signal const &signal, Option::Mode const &mode ):
      location( location ),
      signal( signal ),
      mode( mode )
    {
      mode.ConfigureInput( location );
    }

    auto Input::Read() const -> float
    {
      return signal.Read( location );
    }

    Output::Output( Location const &location, Option::Signal const &signal, Option::Mode const &mode ):
      location( location ),
      signal( signal ),
      mode( mode )
    {
      mode.ConfigureOutput( location );
    }

    auto Output::Write( float value ) const -> void
    {
      signal.Write( location, value );
    }
  }
}
