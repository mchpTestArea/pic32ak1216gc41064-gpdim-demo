/*******************************************************************************
  Capture/Compare/PWM/Timer Module SCCP1 Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sccp1.c

  Summary:
    SCCP1 Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
#include "plib_sccp1.h"
#include "interrupts.h"
// *****************************************************************************

// *****************************************************************************
// Section: SCCP1 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

static volatile CCP_TIMER_OBJECT sccp1TimerObj;
void SCCP1_CompareInitialize (void)
{
    /* Disable Timer */
    CCP1CON1 &= ~_CCP1CON1_ON_MASK;

    CCP1CON1 = 0x5;

    CCP1CON2 = 0x1000000;

    CCP1CON3 = 0x0;

    CCP1PR = 4000;
    CCP1RA = 0;
    CCP1RB = 4095;

    IEC1 |= _IEC1_CCT1IE_MASK;
}

void SCCP1_CompareStart (void)
{
    CCP1CON1 |= _CCP1CON1_ON_MASK;
}

void SCCP1_CompareStop (void)
{
    CCP1CON1 &= ~_CCP1CON1_ON_MASK;
}

void SCCP1_CompareAutoShutdownClear (void)
{
    CCP1STAT &= ~_CCP1STAT_ASEVT_MASK;
}

void SCCP1_CompareAutoShutdownSet (void)
{
    CCP1CON2 |= _CCP1CON2_SSDG_MASK;
}


  

void SCCP1_Compare16bitPeriodValueSet (uint16_t value)
{
    CCP1PR = value;
}

uint16_t SCCP1_Compare16bitPeriodValueGet (void)
{
    return (uint16_t)CCP1PR;
}


void SCCP1_Compare16bitRAValueSet (uint16_t value)
{
    CCP1RA = value;
}

uint16_t SCCP1_Compare16bitRAValueGet (void)
{
    return (uint16_t)CCP1RA;
}

void SCCP1_Compare16bitRBValueSet (uint16_t value)
{
    CCP1RB = value;
}

uint16_t SCCP1_Compare16bitRBValueGet (void)
{
    return (uint16_t)CCP1RB;
}


void SCCP1_TimerCallbackRegister(CCP_TIMER_CALLBACK callback, uintptr_t context)
{
    sccp1TimerObj.callback_fn = callback;

    sccp1TimerObj.context = context;
}

void __attribute__((used)) CCT1_InterruptHandler (void)
{
    /* Additional local variable to prevent MISRA C violations (Rule 13.x) */
    uintptr_t context = sccp1TimerObj.context;
    uint32_t status = IFS1bits.CCT1IF;
    IFS1 &= ~_IFS1_CCT1IF_MASK;    //Clear IRQ flag

    if( (sccp1TimerObj.callback_fn != NULL))
    {
        sccp1TimerObj.callback_fn(status, context);
    }
}

