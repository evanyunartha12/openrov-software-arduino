#include "SysConfig.h"
#if(HAS_MPL3115A2)

#include <CI2C.h>
#include "CMPL3115A2.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
    CTimer mpl3115a2_report_timer;
    CTimer mpl3115a2_sample_timer;
}



CMPL3115A2::CMPL3115A2( CI2C *i2cInterfaceIn )
    : m_mpl( i2cInterfaceIn )
{

}

void CMPL3115A2::Initialize()
{
    Serial.println( "MPL3115A2.Status:INIT;" );

    //init process
    mpl3115a2_sample_timer.Reset();
    mpl3115a2_report_timer.Reset();


    auto ret = m_mpl.Initialize();
    if( ret != mpl3115a2::ERetCode::SUCCESS )
    {
        Serial.println( "MPL3115A2.Status:INIT_FAILED" );
    }

    Serial.println( "MPL3115A2.Status:POST_INIT; ");
}

void CMPL3115A2::InitializeSensor()
{
    //Attempt to init the sensor
    auto ret = m_mpl.Initialize();
    if( ret != mpl3115a2::ERetCode::SUCCESS )
    {
        Serial.println( "MPL3115A2_INIT_STATUS:FAILED;" );
    }
    else
    {
        Serial.println( "MPL3115A2_INIT_STATUS:SUCCESS;" );

        //Set the mode
        m_mpl.SetMode( mpl3115a2::EMode::BAROMETER );

        //Set the over sample rate_div
        m_mpl.SetOversampleRatio( mpl3115a2::EOversampleRatio::OSR_128 );

        //Enable event flags
        m_mpl.EnableEventFlags();


    }
}

void CMPL3115A2::Update( CCommand &commandIn )
{
    //1 Hz
    if( mpl3115a2_sample_timer.HasElapsed( 1000 ) )
    {
        if( !m_mpl.IsInitialized() )
        {
            //Attempt to init every 5 seconds
            if( mpl3115a2_report_timer.HasElapsed( 5000 ) )
            {
                Serial.println( "MPL3115A2.Status:TRYING;" );
                InitializeSensor();
            }
            return;
        }
        else
        {
            Serial.println( "MPL3115A2.Status:RUNNING;" );
            
            //Pressure read
            float pressure;
            auto pressureRet = m_mpl.ReadPressure(pressure);

            if( pressureRet != mpl3115a2::ERetCode::SUCCESS )
            {
                Serial.println( "MPL3115A2.Status:FAILED_READ;" );
                Serial.print( "REASON: ");
                Serial.print( pressureRet );
                Serial.println( ";" );
            }

            Serial.print( "MPL3115A2.Value.Pressure: " );
            Serial.print( pressure );
            Serial.println( ";" );

        }
    }
    
}

#endif