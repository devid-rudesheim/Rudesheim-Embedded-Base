#include "Rudesheim-Embedded-Base.hpp"

namespace Rudesheim::Embedded
{
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
