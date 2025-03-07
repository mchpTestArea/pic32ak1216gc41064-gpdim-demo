/*******************************************************************************
  Capture/Compare/PWM/Timer Module SCCP3 Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sccp3.c

  Summary:
    SCCP3 Source File

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
#include "plib_sccp3.h"
#include "interrupts.h"
// *****************************************************************************

// *****************************************************************************
// Section: SCCP3 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

static volatile CCP_TIMER_OBJECT sccp3TimerObj;
void SCCP3_CompareInitialize (void)
{
    /* Disable Timer */
    CCP3CON1 &= ~_CCP3CON1_ON_MASK;

    CCP3CON1 = 0x5;

    CCP3CON2 = 0x1000000;

    CCP3CON3 = 0x0;

    CCP3PR = 4000;
    CCP3RA = 0;
    CCP3RB = 4095;

    IEC1 |= _IEC1_CCT3IE_MASK;
}

void SCCP3_CompareStart (void)
{
    CCP3CON1 |= _CCP3CON1_ON_MASK;
}

void SCCP3_CompareStop (void)
{
    CCP3CON1 &= ~_CCP3CON1_ON_MASK;
}

void SCCP3_CompareAutoShutdownClear (void)
{
    CCP3STAT &= ~_CCP3STAT_ASEVT_MASK;
}

void SCCP3_CompareAutoShutdownSet (void)
{
    CCP3CON2 |= _CCP3CON2_SSDG_MASK;
}


  

void SCCP3_Compare16bitPeriodValueSet (uint16_t value)
{
    CCP3PR = value;
}

uint16_t SCCP3_Compare16bitPeriodValueGet (void)
{
    return (uint16_t)CCP3PR;
}


void SCCP3_Compare16bitRAValueSet (uint16_t value)
{
    CCP3RA = value;
}

uint16_t SCCP3_Compare16bitRAValueGet (void)
{
    return (uint16_t)CCP3RA;
}

void SCCP3_Compare16bitRBValueSet (uint16_t value)
{
    CCP3RB = value;
}

uint16_t SCCP3_Compare16bitRBValueGet (void)
{
    return (uint16_t)CCP3RB;
}


void SCCP3_TimerCallbackRegister(CCP_TIMER_CALLBACK callback, uintptr_t context)
{
    sccp3TimerObj.callback_fn = callback;

    sccp3TimerObj.context = context;
}

void __attribute__((used)) CCT3_InterruptHandler (void)
{
    /* Additional local variable to prevent MISRA C violations (Rule 13.x) */
    uintptr_t context = sccp3TimerObj.context;
    uint32_t status = IFS1bits.CCT3IF;
    IFS1 &= ~_IFS1_CCT3IF_MASK;    //Clear IRQ flag

    if( (sccp3TimerObj.callback_fn != NULL))
    {
        sccp3TimerObj.callback_fn(status, context);
    }
}

