#include "Airframe.h"
#include <algorithm>
Skyhawk::Airframe::Airframe(Input& controls, Engine& engine):
	m_controls(controls),
	m_engine(engine)
{
	m_integrityElement = new float[(int)Damage::COUNT];
}

Skyhawk::Airframe::~Airframe()
{
	delete[] m_integrityElement;
}

void Skyhawk::Airframe::printDamageState()
{
	printf( "===========================================\n" );

	printf( "            ||\n" );
	printf( "            ||\n" );
	printf( "%.1lf %.1lf %.1lf || %.1lf %.1lf %.1lf\n", 
		DMG_ELEM(Damage::WING_L_OUT), 
		DMG_ELEM(Damage::WING_L_CENTER), 
		DMG_ELEM(Damage::WING_L_IN),
		DMG_ELEM( Damage::WING_R_IN ),
		DMG_ELEM( Damage::WING_R_CENTER ),
		DMG_ELEM( Damage::WING_R_OUT ) );

	printf( "        %.1f || %.1f\n", DMG_ELEM(Damage::AILERON_L), DMG_ELEM(Damage::AILERON_R) );
	printf( "            ||\n" );
	printf( "        %.1f || %.1f\n", DMG_ELEM( Damage::STABILIZATOR_L ), DMG_ELEM( Damage::STABILIZATOR_R ) );
	printf( "        %.1f || %.1f\n", DMG_ELEM( Damage::ELEVATOR_L ), DMG_ELEM( Damage::ELEVATOR_R ) );
	printf( "            %.1f\n", getVertStabDamage() );
	printf( "            %.1f\n", getRudderDamage() );
}

void Skyhawk::Airframe::resetDamage()
{
	for ( int i = 0; i < (int)Damage::COUNT; i++ )
	{
		m_integrityElement[i] = 1.0f;
	}
}

void Skyhawk::Airframe::zeroInit()
{
	m_gearPosition = 0.0;
	m_flapsPosition = 0.0;
	m_spoilerPosition = 0.0;
	m_speedBrakePosition = 0.0;
	m_hookPosition = 0.0;
	m_slatLPosition = 0.0;
	m_slatRPosition = 0.0;
	m_flapsPosition = 0.0;
	m_catapultState = CatapultState::OFF_CAT;
	m_mass = 1.0;
	//m_fuel = 0.0;
	//m_fuelPrevious = 0.0;

	resetDamage();
}

void Skyhawk::Airframe::coldInit()
{
	zeroInit();
	m_controls.gear() = Input::GearPos::DOWN;
	m_gearPosition = 1.0;
}

void Skyhawk::Airframe::hotInit()
{
	zeroInit();
	m_controls.gear() = Input::GearPos::DOWN;
	m_gearPosition = 1.0;
}

void Skyhawk::Airframe::airborneInit()
{
	zeroInit();
	m_controls.gear() = Input::GearPos::UP;
}

void Skyhawk::Airframe::airframeUpdate(double dt)
{
	//m_slatLPosition = m_controls.slatL();
	//m_slatRPosition = m_controls.slatR();

	/*if (m_controls.slatL() >= m_slatLPosition)
	{
		m_slatLPosition += dt / m_slatsExtendTime;
		m_slatLPosition = std::min(m_slatLPosition, m_controls.slatL());
	}
	else
	{
		m_slatLPosition -= dt / m_slatsExtendTime;
		m_slatLPosition = std::max(m_slatLPosition, m_controls.slatL());
	}

	if (m_controls.slatR() >= m_slatRPosition)
	{
		m_slatRPosition += dt / m_slatsExtendTime;
		m_slatRPosition = std::min(m_slatRPosition, m_controls.slatR());
	}
	else
	{
		m_slatRPosition -= dt / m_slatsExtendTime;
		m_slatRPosition = std::max(m_slatRPosition, m_controls.slatR());
	}*/

	/*if (m_controls.airbrake() >= m_speedBrakePosition)
	{
		m_speedBrakePosition += dt / m_airbrakesExtendTime;
		m_speedBrakePosition = std::min(m_speedBrakePosition, m_controls.airbrake());
	}
	else
	{
		m_speedBrakePosition -= dt / m_airbrakesExtendTime;
		m_speedBrakePosition = std::max(m_speedBrakePosition, m_controls.airbrake());
	}*/

	if (m_controls.hook())
	{
		m_hookPosition += dt / m_hookExtendTime;
		m_hookPosition = std::min(m_hookPosition, 1.0);
	}
	else
	{
		m_hookPosition -= dt / m_hookExtendTime;
		m_hookPosition = std::max(m_hookPosition, 0.0);
	}

	double dm = m_engine.getFuelFlow()*dt;

	int totalExt = 0;
	for (int i = Tank::INTERNAL; i < Tank::DONT_TOUCH; i++)
	{
		m_fuelPrev[i] = m_fuel[i];
		if (m_fuel[i] > 10.0 && i > Tank::INTERNAL)
		{
			totalExt++;
		}
	}

	double sub_dm = totalExt > 0 ? dm / (double)totalExt : dm;
	for (int i = Tank::INTERNAL; i < Tank::DONT_TOUCH; i++)
	{
		if (i == Tank::INTERNAL)
		{
			if (!totalExt)
			{
				m_fuel[i] -= dm;
				break;
			}
		}
		else if (m_fuel[i] > 10.0)
		{
			m_fuel[i] -= sub_dm;
		}
	}

	//printf("LEFT: %lf, CENTRE: %lf, RIGHT: %lf, INTERNAL: %lf\n", m_fuel[Tank::LEFT_EXT], m_fuel[Tank::CENTRE_EXT], m_fuel[Tank::RIGHT_EXT], m_fuel[Tank::INTERNAL]);

	m_engine.setHasFuel(m_fuel[Tank::INTERNAL] > 50.0);
	
	m_elevator = m_controls.pitch() + m_controls.pitchTrim();
	m_aileronLeft = m_controls.roll() + m_controls.rollTrim();
	m_aileronRight = -m_aileronLeft;
	m_rudder = m_controls.yaw() + m_controls.yawDamper() + m_controls.yawTrim();


	if (m_catapultState == ON_CAT_NOT_READY && m_engine.getRPMNorm() > 0.9)
	{
		m_catapultState = ON_CAT_READY;
	}

	if (m_catapultState != OFF_CAT)
	{
		m_catMoment = pow((c_catAngle - m_angle)*60.0, 3.0) * c_catConstrainingForce;
		m_catMoment = m_catMoment < 0.0 ? m_catMoment : 0.0;
	}
	else
	{
		m_catMoment = 0.0;
	}
	//printf("Cat Moment: %lf\n", m_catMoment);

	m_catStateSent = false;

	//printDamageState();
}