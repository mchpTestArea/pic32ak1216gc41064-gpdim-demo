/*******************************************************************************
  PWM PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pwm.c

  Summary:
    PWM PLIB Source File

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

// Section: Included Files


#include "plib_pwm.h"
#include "interrupts.h"

//PWM Generator x PCI Termination Polarity options
#define PG1FPCI_PPS_NON_INVERTED          ((uint32_t)(_PG1FPCI_PPS_MASK & ((uint32_t)(0) << _PG1FPCI_PPS_POSITION))) 
#define PG1FPCI_PPS_INVERTED          ((uint32_t)(_PG1FPCI_PPS_MASK & ((uint32_t)(1) << _PG1FPCI_PPS_POSITION))) 

//PGxCON MDCSEL bit
#define PG1CON_MDCSEL_PGxDC          ((uint32_t)(_PG1CON_MDCSEL_MASK & ((uint32_t)(0) << _PG1CON_MDCSEL_POSITION)))
#define PG1CON_MDCSEL_MDC          ((uint32_t)(_PG1CON_MDCSEL_MASK & ((uint32_t)(1) << _PG1CON_MDCSEL_POSITION)))

//PGxCON MPERSEL bit
#define PG1CON_MPERSEL_PGxPER          ((uint32_t)(_PG1CON_MPERSEL_MASK & ((uint32_t)(0) << _PG1CON_MPERSEL_POSITION)))
#define PG1CON_MPERSEL_MPER          ((uint32_t)(_PG1CON_MPERSEL_MASK & ((uint32_t)(1) << _PG1CON_MPERSEL_POSITION)))

//PGxCON MPHSEL bit
#define PG1CON_MPHSEL_PGxPHASE          ((uint32_t)(_PG1CON_MPHSEL_MASK & ((uint32_t)(0) << _PG1CON_MPHSEL_POSITION)))
#define PG1CON_MPHSEL_MPHASE          ((uint32_t)(_PG1CON_MPHSEL_MASK & ((uint32_t)(1) << _PG1CON_MPHSEL_POSITION)))

//PGxCON MSTEN bit
#define PG1CON_MSTEN_DISABLED          ((uint32_t)(_PG1CON_MSTEN_MASK & ((uint32_t)(0) << _PG1CON_MSTEN_POSITION)))
#define PG1CON_MSTEN_ENABLED          ((uint32_t)(_PG1CON_MSTEN_MASK & ((uint32_t)(1) << _PG1CON_MSTEN_POSITION)))

//PWM Generator x Trigger Mode Selection options
#define PG1CON_TRGMOD_SINGLE_TRIGGER_MODE            ((uint32_t)(_PG1CON_TRGMOD_MASK & ((uint32_t)(0) << _PG1CON_TRGMOD_POSITION))) 
#define PG1CON_TRGMOD_RETRIGGRERABLE_MODE            ((uint32_t)(_PG1CON_TRGMOD_MASK & ((uint32_t)(1) << _PG1CON_TRGMOD_POSITION))) 

//PWM Generator x Data Register Update Modes
#define PG1CON_UPDMOD_SOC            ((uint32_t)(_PG1CON_UPDMOD_MASK & ((uint32_t)(0) << _PG1CON_UPDMOD_POSITION))) 
#define PG1CON_UPDMOD_IMMEDIATE            ((uint32_t)(_PG1CON_UPDMOD_MASK & ((uint32_t)(1) << _PG1CON_UPDMOD_POSITION))) 
#define PG1CON_UPDMOD_CLIENT_SOC            ((uint32_t)(_PG1CON_UPDMOD_MASK & ((uint32_t)(2) << _PG1CON_UPDMOD_POSITION))) 
#define PG1CON_UPDMOD_CLIENT_IMMEDIATE            ((uint32_t)(_PG1CON_UPDMOD_MASK & ((uint32_t)(3) << _PG1CON_UPDMOD_POSITION))) 

//PWM Generator x Start-of-Cycle Selection bits
#define PG1CON_SOCS_LOCAL_EOC            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(0) << _PG1CON_SOCS_POSITION)))
#define PG1CON_SOCS_TRIG_OP_BY_PG1_OR_PG5            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(1) << _PG1CON_SOCS_POSITION)))
#define PG1CON_SOCS_TRIG_OP_BY_PG2_OR_PG6            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(2) << _PG1CON_SOCS_POSITION)))
#define PG1CON_SOCS_TRIG_OP_BY_PG3_OR_PG7            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(3) << _PG1CON_SOCS_POSITION)))
#define PG1CON_SOCS_TRIG_OP_BY_PG4_OR_PG8            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(4) << _PG1CON_SOCS_POSITION)))
#define PG1CON_SOCS_TRIG_BIT_OR_PCI_SYNC_FN_ONLY            ((uint32_t)(_PG1CON_SOCS_MASK & ((uint32_t)(15) << _PG1CON_SOCS_POSITION)))

//PWM Generator x CLKSEL bits
#define PG1CON_CLKSEL_NO_CLOCK          ((uint32_t)(_PG1CON_CLKSEL_MASK & ((uint32_t)(0) << _PG1CON_CLKSEL_POSITION)))
#define PG1CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL          ((uint32_t)(_PG1CON_CLKSEL_MASK & ((uint32_t)(1) << _PG1CON_CLKSEL_POSITION)))
#define PG1CON_CLKSEL_MASTER_CLOCK_DIVIDER          ((uint32_t)(_PG1CON_CLKSEL_MASK & ((uint32_t)(2) << _PG1CON_CLKSEL_POSITION)))
#define PG1CON_CLKSEL_MASTER_CLOCK_FREQ_SCALING          ((uint32_t)(_PG1CON_CLKSEL_MASK & ((uint32_t)(3) << _PG1CON_CLKSEL_POSITION)))

#define PG1CON_MODSEL_INDEPENDENT_EDGE          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(0) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_VARIABLE_PHASE          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(1) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_INDEPENDENT_EDGE_DUAL_OUTPUT          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(2) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_CENTER_ALIGNED          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(4) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_DOUBLE_UPDATE_CENTER_ALIGNED          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(5) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_DUALEDGE_CA_ONE_UPDATE_CYCLE          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(6) << _PG1CON_MODSEL_POSITION)))
#define PG1CON_MODSEL_DUALEDGE_CA_TWO_UPDATE_CYCLE          ((uint32_t)(_PG1CON_MODSEL_MASK & ((uint32_t)(7) << _PG1CON_MODSEL_POSITION)))

//PWM Generator x PMOD bits
#define PG1IOCON_PMOD_COMPLEMENTARY_MODE          ((uint32_t)(_PG1IOCON_PMOD_MASK & ((uint32_t)(0) << _PG1IOCON_PMOD_POSITION)))
#define PG1IOCON_PMOD_INDEPENDENT_MODE          ((uint32_t)(_PG1IOCON_PMOD_MASK & ((uint32_t)(1) << _PG1IOCON_PMOD_POSITION)))
#define PG1IOCON_PMOD_PUSH_PULL_MODE          ((uint32_t)(_PG1IOCON_PMOD_MASK & ((uint32_t)(2) << _PG1IOCON_PMOD_POSITION)))
#define PG1IOCON_PMOD_RESERVED          ((uint32_t)(_PG1IOCON_PMOD_MASK & ((uint32_t)(3) << _PG1IOCON_PMOD_POSITION)))

//PWM Generator x POLH bit
#define PG1IOCON_POLH_ACTIVE_HIGH          ((uint32_t)(_PG1IOCON_POLH_MASK & ((uint32_t)(0) << _PG1IOCON_POLH_POSITION)))
#define PG1IOCON_POLH_ACTIVE_LOW          ((uint32_t)(_PG1IOCON_POLH_MASK & ((uint32_t)(1) << _PG1IOCON_POLH_POSITION)))

//PWM Generator x POLL bit
#define PG1IOCON_POLL_ACTIVE_HIGH          ((uint32_t)(_PG1IOCON_POLL_MASK & ((uint32_t)(0) << _PG1IOCON_POLL_POSITION)))
#define PG1IOCON_POLL_ACTIVE_LOW          ((uint32_t)(_PG1IOCON_POLL_MASK & ((uint32_t)(1) << _PG1IOCON_POLL_POSITION)))

//PWM Generator x SWAP bit
#define PG1IOCON_SWAP_NORMAL_MAPPING          ((uint32_t)(_PG1IOCON_SWAP_MASK & ((uint32_t)(0) << _PG1IOCON_SWAP_POSITION)))
#define PG1IOCON_SWAP_SWAPPED          ((uint32_t)(_PG1IOCON_SWAP_MASK & ((uint32_t)(1) << _PG1IOCON_SWAP_POSITION)))

#define PG1IOCON_OVRENH_PWM_GENERATOR          ((uint32_t)(_PG1IOCON_OVRENH_MASK & ((uint32_t)(0) << _PG1IOCON_OVRENH_POSITION)))
#define PG1IOCON_OVRENH_OVRDAT_1          ((uint32_t)(_PG1IOCON_OVRENH_MASK & ((uint32_t)(1) << _PG1IOCON_OVRENH_POSITION)))

//PWM Generator x OVRENL bit
#define PG1IOCON_OVRENL_PWM_GENERATOR          ((uint32_t)(_PG1IOCON_OVRENL_MASK & ((uint32_t)(0) << _PG1IOCON_OVRENL_POSITION)))
#define PG1IOCON_OVRENL_OVRDAT_0          ((uint32_t)(_PG1IOCON_OVRENL_MASK & ((uint32_t)(1) << _PG1IOCON_OVRENL_POSITION)))

//PWM Generator x OSYNC[1:0] bits
#define PG1IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE          ((uint32_t)(_PG1IOCON_OSYNC_MASK & ((uint32_t)(0) << _PG1IOCON_OSYNC_POSITION)))
#define PG1IOCON_OSYNC_SYNC_IMMEDIATE          ((uint32_t)(_PG1IOCON_OSYNC_MASK & ((uint32_t)(1) << _PG1IOCON_OSYNC_POSITION)))
#define PG1IOCON_OSYNC_SYNC_TO_UPDMOD          ((uint32_t)(_PG1IOCON_OSYNC_MASK & ((uint32_t)(2) << _PG1IOCON_OSYNC_POSITION)))
#define PG1IOCON_OSYNC_SYNC_TO_DATA_BUFFER_UPDATE          ((uint32_t)(_PG1IOCON_OSYNC_MASK & ((uint32_t)(3) << _PG1IOCON_OSYNC_POSITION)))

//PGxEVT ADTR2EN3 bit
#define PG1EVT_ADTR2EN3_DISABLED          ((uint32_t)(_PG1EVT_ADTR2EN3_MASK & ((uint32_t)(0) << _PG1EVT_ADTR2EN3_POSITION)))
#define PG1EVT_ADTR2EN3_ENABLED          ((uint32_t)(_PG1EVT_ADTR2EN3_MASK & ((uint32_t)(1) << _PG1EVT_ADTR2EN3_POSITION)))

//PGxEVT ADTR2EN2 bit
#define PG1EVT_ADTR2EN2_DISABLED          ((uint32_t)(_PG1EVT_ADTR2EN2_MASK & ((uint32_t)(0) << _PG1EVT_ADTR2EN2_POSITION)))
#define PG1EVT_ADTR2EN2_ENABLED          ((uint32_t)(_PG1EVT_ADTR2EN2_MASK & ((uint32_t)(1) << _PG1EVT_ADTR2EN2_POSITION)))

//PGxEVT ADTR2EN1 bit
#define PG1EVT_ADTR2EN1_DISABLED          ((uint32_t)(_PG1EVT_ADTR2EN1_MASK & ((uint32_t)(0) << _PG1EVT_ADTR2EN1_POSITION)))
#define PG1EVT_ADTR2EN1_ENABLED          ((uint32_t)(_PG1EVT_ADTR2EN1_MASK & ((uint32_t)(1) << _PG1EVT_ADTR2EN1_POSITION)))

//PGxEVT ADTR1EN3 bit
#define PG1EVT_ADTR1EN3_DISABLED          ((uint32_t)(_PG1EVT_ADTR1EN3_MASK & ((uint32_t)(0) << _PG1EVT_ADTR1EN3_POSITION)))
#define PG1EVT_ADTR1EN3_ENABLED          ((uint32_t)(_PG1EVT_ADTR1EN3_MASK & ((uint32_t)(1) << _PG1EVT_ADTR1EN3_POSITION)))

//PGxEVT ADTR1EN2 bit
#define PG1EVT_ADTR1EN2_DISABLED          ((uint32_t)(_PG1EVT_ADTR1EN2_MASK & ((uint32_t)(0) << _PG1EVT_ADTR1EN2_POSITION)))
#define PG1EVT_ADTR1EN2_ENABLED          ((uint32_t)(_PG1EVT_ADTR1EN2_MASK & ((uint32_t)(1) << _PG1EVT_ADTR1EN2_POSITION)))

//PGxEVT ADTR1EN1 bit
#define PG1EVT_ADTR1EN1_DISABLED          ((uint32_t)(_PG1EVT_ADTR1EN1_MASK & ((uint32_t)(0) << _PG1EVT_ADTR1EN1_POSITION)))
#define PG1EVT_ADTR1EN1_ENABLED          ((uint32_t)(_PG1EVT_ADTR1EN1_MASK & ((uint32_t)(1) << _PG1EVT_ADTR1EN1_POSITION)))

//PGxEVT PWMPCI[2:0] bits
#define PG1EVT_PWMPCI_PWM_GENERATOR_1          ((uint32_t)(_PG1EVT_PWMPCI_MASK & ((uint32_t)(0) << _PG1EVT_PWMPCI_POSITION)))
#define PG1EVT_PWMPCI_PWM_GENERATOR_2          ((uint32_t)(_PG1EVT_PWMPCI_MASK & ((uint32_t)(1) << _PG1EVT_PWMPCI_POSITION)))
#define PG1EVT_PWMPCI_PWM_GENERATOR_3          ((uint32_t)(_PG1EVT_PWMPCI_MASK & ((uint32_t)(2) << _PG1EVT_PWMPCI_POSITION)))
#define PG1EVT_PWMPCI_PWM_GENERATOR_4          ((uint32_t)(_PG1EVT_PWMPCI_MASK & ((uint32_t)(3) << _PG1EVT_PWMPCI_POSITION)))

//PGxEVT UPDTRG[1:0] bits
#define PG1EVT_UPDTRG_MANUAL          ((uint32_t)(_PG1EVT_UPDTRG_MASK & ((uint32_t)(0) << _PG1EVT_UPDTRG_POSITION)))
#define PG1EVT_UPDTRG_PGxDC          ((uint32_t)(_PG1EVT_UPDTRG_MASK & ((uint32_t)(1) << _PG1EVT_UPDTRG_POSITION)))
#define PG1EVT_UPDTRG_PGxPHASE          ((uint32_t)(_PG1EVT_UPDTRG_MASK & ((uint32_t)(2) << _PG1EVT_UPDTRG_POSITION)))
#define PG1EVT_UPDTRG_PGxTRIGA          ((uint32_t)(_PG1EVT_UPDTRG_MASK & ((uint32_t)(3) << _PG1EVT_UPDTRG_POSITION)))

//PGxEVT PGTRGSEL[2:0] bits
#define PG1EVT_PGTRGSEL_EOC          ((uint32_t)(_PG1EVT_PGTRGSEL_MASK & ((uint32_t)(0) << _PG1EVT_PGTRGSEL_POSITION)))
#define PG1EVT_PGTRGSEL_TRIGA_COMPARE          ((uint32_t)(_PG1EVT_PGTRGSEL_MASK & ((uint32_t)(1) << _PG1EVT_PGTRGSEL_POSITION)))
#define PG1EVT_PGTRGSEL_TRIGB_COMPARE          ((uint32_t)(_PG1EVT_PGTRGSEL_MASK & ((uint32_t)(2) << _PG1EVT_PGTRGSEL_POSITION)))
#define PG1EVT_PGTRGSEL_TRIGC_COMPARE          ((uint32_t)(_PG1EVT_PGTRGSEL_MASK & ((uint32_t)(3) << _PG1EVT_PGTRGSEL_POSITION)))
        
//PWM Generator x PCI Termination Polarity options
#define PG2FPCI_PPS_NON_INVERTED          ((uint32_t)(_PG2FPCI_PPS_MASK & ((uint32_t)(0) << _PG2FPCI_PPS_POSITION))) 
#define PG2FPCI_PPS_INVERTED          ((uint32_t)(_PG2FPCI_PPS_MASK & ((uint32_t)(1) << _PG2FPCI_PPS_POSITION))) 

//PGxCON MDCSEL bit
#define PG2CON_MDCSEL_PGxDC          ((uint32_t)(_PG2CON_MDCSEL_MASK & ((uint32_t)(0) << _PG2CON_MDCSEL_POSITION)))
#define PG2CON_MDCSEL_MDC          ((uint32_t)(_PG2CON_MDCSEL_MASK & ((uint32_t)(1) << _PG2CON_MDCSEL_POSITION)))

//PGxCON MPERSEL bit
#define PG2CON_MPERSEL_PGxPER          ((uint32_t)(_PG2CON_MPERSEL_MASK & ((uint32_t)(0) << _PG2CON_MPERSEL_POSITION)))
#define PG2CON_MPERSEL_MPER          ((uint32_t)(_PG2CON_MPERSEL_MASK & ((uint32_t)(1) << _PG2CON_MPERSEL_POSITION)))

//PGxCON MPHSEL bit
#define PG2CON_MPHSEL_PGxPHASE          ((uint32_t)(_PG2CON_MPHSEL_MASK & ((uint32_t)(0) << _PG2CON_MPHSEL_POSITION)))
#define PG2CON_MPHSEL_MPHASE          ((uint32_t)(_PG2CON_MPHSEL_MASK & ((uint32_t)(1) << _PG2CON_MPHSEL_POSITION)))

//PGxCON MSTEN bit
#define PG2CON_MSTEN_DISABLED          ((uint32_t)(_PG2CON_MSTEN_MASK & ((uint32_t)(0) << _PG2CON_MSTEN_POSITION)))
#define PG2CON_MSTEN_ENABLED          ((uint32_t)(_PG2CON_MSTEN_MASK & ((uint32_t)(1) << _PG2CON_MSTEN_POSITION)))

//PWM Generator x Trigger Mode Selection options
#define PG2CON_TRGMOD_SINGLE_TRIGGER_MODE            ((uint32_t)(_PG2CON_TRGMOD_MASK & ((uint32_t)(0) << _PG2CON_TRGMOD_POSITION))) 
#define PG2CON_TRGMOD_RETRIGGRERABLE_MODE            ((uint32_t)(_PG2CON_TRGMOD_MASK & ((uint32_t)(1) << _PG2CON_TRGMOD_POSITION))) 

//PWM Generator x Data Register Update Modes
#define PG2CON_UPDMOD_SOC            ((uint32_t)(_PG2CON_UPDMOD_MASK & ((uint32_t)(0) << _PG2CON_UPDMOD_POSITION))) 
#define PG2CON_UPDMOD_IMMEDIATE            ((uint32_t)(_PG2CON_UPDMOD_MASK & ((uint32_t)(1) << _PG2CON_UPDMOD_POSITION))) 
#define PG2CON_UPDMOD_CLIENT_SOC            ((uint32_t)(_PG2CON_UPDMOD_MASK & ((uint32_t)(2) << _PG2CON_UPDMOD_POSITION))) 
#define PG2CON_UPDMOD_CLIENT_IMMEDIATE            ((uint32_t)(_PG2CON_UPDMOD_MASK & ((uint32_t)(3) << _PG2CON_UPDMOD_POSITION))) 

//PWM Generator x Start-of-Cycle Selection bits
#define PG2CON_SOCS_LOCAL_EOC            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(0) << _PG2CON_SOCS_POSITION)))
#define PG2CON_SOCS_TRIG_OP_BY_PG1_OR_PG5            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(1) << _PG2CON_SOCS_POSITION)))
#define PG2CON_SOCS_TRIG_OP_BY_PG2_OR_PG6            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(2) << _PG2CON_SOCS_POSITION)))
#define PG2CON_SOCS_TRIG_OP_BY_PG3_OR_PG7            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(3) << _PG2CON_SOCS_POSITION)))
#define PG2CON_SOCS_TRIG_OP_BY_PG4_OR_PG8            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(4) << _PG2CON_SOCS_POSITION)))
#define PG2CON_SOCS_TRIG_BIT_OR_PCI_SYNC_FN_ONLY            ((uint32_t)(_PG2CON_SOCS_MASK & ((uint32_t)(15) << _PG2CON_SOCS_POSITION)))

//PWM Generator x CLKSEL bits
#define PG2CON_CLKSEL_NO_CLOCK          ((uint32_t)(_PG2CON_CLKSEL_MASK & ((uint32_t)(0) << _PG2CON_CLKSEL_POSITION)))
#define PG2CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL          ((uint32_t)(_PG2CON_CLKSEL_MASK & ((uint32_t)(1) << _PG2CON_CLKSEL_POSITION)))
#define PG2CON_CLKSEL_MASTER_CLOCK_DIVIDER          ((uint32_t)(_PG2CON_CLKSEL_MASK & ((uint32_t)(2) << _PG2CON_CLKSEL_POSITION)))
#define PG2CON_CLKSEL_MASTER_CLOCK_FREQ_SCALING          ((uint32_t)(_PG2CON_CLKSEL_MASK & ((uint32_t)(3) << _PG2CON_CLKSEL_POSITION)))

#define PG2CON_MODSEL_INDEPENDENT_EDGE          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(0) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_VARIABLE_PHASE          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(1) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_INDEPENDENT_EDGE_DUAL_OUTPUT          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(2) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_CENTER_ALIGNED          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(4) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_DOUBLE_UPDATE_CENTER_ALIGNED          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(5) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_DUALEDGE_CA_ONE_UPDATE_CYCLE          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(6) << _PG2CON_MODSEL_POSITION)))
#define PG2CON_MODSEL_DUALEDGE_CA_TWO_UPDATE_CYCLE          ((uint32_t)(_PG2CON_MODSEL_MASK & ((uint32_t)(7) << _PG2CON_MODSEL_POSITION)))

//PWM Generator x PMOD bits
#define PG2IOCON_PMOD_COMPLEMENTARY_MODE          ((uint32_t)(_PG2IOCON_PMOD_MASK & ((uint32_t)(0) << _PG2IOCON_PMOD_POSITION)))
#define PG2IOCON_PMOD_INDEPENDENT_MODE          ((uint32_t)(_PG2IOCON_PMOD_MASK & ((uint32_t)(1) << _PG2IOCON_PMOD_POSITION)))
#define PG2IOCON_PMOD_PUSH_PULL_MODE          ((uint32_t)(_PG2IOCON_PMOD_MASK & ((uint32_t)(2) << _PG2IOCON_PMOD_POSITION)))
#define PG2IOCON_PMOD_RESERVED          ((uint32_t)(_PG2IOCON_PMOD_MASK & ((uint32_t)(3) << _PG2IOCON_PMOD_POSITION)))

//PWM Generator x POLH bit
#define PG2IOCON_POLH_ACTIVE_HIGH          ((uint32_t)(_PG2IOCON_POLH_MASK & ((uint32_t)(0) << _PG2IOCON_POLH_POSITION)))
#define PG2IOCON_POLH_ACTIVE_LOW          ((uint32_t)(_PG2IOCON_POLH_MASK & ((uint32_t)(1) << _PG2IOCON_POLH_POSITION)))

//PWM Generator x POLL bit
#define PG2IOCON_POLL_ACTIVE_HIGH          ((uint32_t)(_PG2IOCON_POLL_MASK & ((uint32_t)(0) << _PG2IOCON_POLL_POSITION)))
#define PG2IOCON_POLL_ACTIVE_LOW          ((uint32_t)(_PG2IOCON_POLL_MASK & ((uint32_t)(1) << _PG2IOCON_POLL_POSITION)))

//PWM Generator x SWAP bit
#define PG2IOCON_SWAP_NORMAL_MAPPING          ((uint32_t)(_PG2IOCON_SWAP_MASK & ((uint32_t)(0) << _PG2IOCON_SWAP_POSITION)))
#define PG2IOCON_SWAP_SWAPPED          ((uint32_t)(_PG2IOCON_SWAP_MASK & ((uint32_t)(1) << _PG2IOCON_SWAP_POSITION)))

#define PG2IOCON_OVRENH_PWM_GENERATOR          ((uint32_t)(_PG2IOCON_OVRENH_MASK & ((uint32_t)(0) << _PG2IOCON_OVRENH_POSITION)))
#define PG2IOCON_OVRENH_OVRDAT_1          ((uint32_t)(_PG2IOCON_OVRENH_MASK & ((uint32_t)(1) << _PG2IOCON_OVRENH_POSITION)))

//PWM Generator x OVRENL bit
#define PG2IOCON_OVRENL_PWM_GENERATOR          ((uint32_t)(_PG2IOCON_OVRENL_MASK & ((uint32_t)(0) << _PG2IOCON_OVRENL_POSITION)))
#define PG2IOCON_OVRENL_OVRDAT_0          ((uint32_t)(_PG2IOCON_OVRENL_MASK & ((uint32_t)(1) << _PG2IOCON_OVRENL_POSITION)))

//PWM Generator x OSYNC[1:0] bits
#define PG2IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE          ((uint32_t)(_PG2IOCON_OSYNC_MASK & ((uint32_t)(0) << _PG2IOCON_OSYNC_POSITION)))
#define PG2IOCON_OSYNC_SYNC_IMMEDIATE          ((uint32_t)(_PG2IOCON_OSYNC_MASK & ((uint32_t)(1) << _PG2IOCON_OSYNC_POSITION)))
#define PG2IOCON_OSYNC_SYNC_TO_UPDMOD          ((uint32_t)(_PG2IOCON_OSYNC_MASK & ((uint32_t)(2) << _PG2IOCON_OSYNC_POSITION)))
#define PG2IOCON_OSYNC_SYNC_TO_DATA_BUFFER_UPDATE          ((uint32_t)(_PG2IOCON_OSYNC_MASK & ((uint32_t)(3) << _PG2IOCON_OSYNC_POSITION)))

//PGxEVT ADTR2EN3 bit
#define PG2EVT_ADTR2EN3_DISABLED          ((uint32_t)(_PG2EVT_ADTR2EN3_MASK & ((uint32_t)(0) << _PG2EVT_ADTR2EN3_POSITION)))
#define PG2EVT_ADTR2EN3_ENABLED          ((uint32_t)(_PG2EVT_ADTR2EN3_MASK & ((uint32_t)(1) << _PG2EVT_ADTR2EN3_POSITION)))

//PGxEVT ADTR2EN2 bit
#define PG2EVT_ADTR2EN2_DISABLED          ((uint32_t)(_PG2EVT_ADTR2EN2_MASK & ((uint32_t)(0) << _PG2EVT_ADTR2EN2_POSITION)))
#define PG2EVT_ADTR2EN2_ENABLED          ((uint32_t)(_PG2EVT_ADTR2EN2_MASK & ((uint32_t)(1) << _PG2EVT_ADTR2EN2_POSITION)))

//PGxEVT ADTR2EN1 bit
#define PG2EVT_ADTR2EN1_DISABLED          ((uint32_t)(_PG2EVT_ADTR2EN1_MASK & ((uint32_t)(0) << _PG2EVT_ADTR2EN1_POSITION)))
#define PG2EVT_ADTR2EN1_ENABLED          ((uint32_t)(_PG2EVT_ADTR2EN1_MASK & ((uint32_t)(1) << _PG2EVT_ADTR2EN1_POSITION)))

//PGxEVT ADTR1EN3 bit
#define PG2EVT_ADTR1EN3_DISABLED          ((uint32_t)(_PG2EVT_ADTR1EN3_MASK & ((uint32_t)(0) << _PG2EVT_ADTR1EN3_POSITION)))
#define PG2EVT_ADTR1EN3_ENABLED          ((uint32_t)(_PG2EVT_ADTR1EN3_MASK & ((uint32_t)(1) << _PG2EVT_ADTR1EN3_POSITION)))

//PGxEVT ADTR1EN2 bit
#define PG2EVT_ADTR1EN2_DISABLED          ((uint32_t)(_PG2EVT_ADTR1EN2_MASK & ((uint32_t)(0) << _PG2EVT_ADTR1EN2_POSITION)))
#define PG2EVT_ADTR1EN2_ENABLED          ((uint32_t)(_PG2EVT_ADTR1EN2_MASK & ((uint32_t)(1) << _PG2EVT_ADTR1EN2_POSITION)))

//PGxEVT ADTR1EN1 bit
#define PG2EVT_ADTR1EN1_DISABLED          ((uint32_t)(_PG2EVT_ADTR1EN1_MASK & ((uint32_t)(0) << _PG2EVT_ADTR1EN1_POSITION)))
#define PG2EVT_ADTR1EN1_ENABLED          ((uint32_t)(_PG2EVT_ADTR1EN1_MASK & ((uint32_t)(1) << _PG2EVT_ADTR1EN1_POSITION)))

//PGxEVT PWMPCI[2:0] bits
#define PG2EVT_PWMPCI_PWM_GENERATOR_1          ((uint32_t)(_PG2EVT_PWMPCI_MASK & ((uint32_t)(0) << _PG2EVT_PWMPCI_POSITION)))
#define PG2EVT_PWMPCI_PWM_GENERATOR_2          ((uint32_t)(_PG2EVT_PWMPCI_MASK & ((uint32_t)(1) << _PG2EVT_PWMPCI_POSITION)))
#define PG2EVT_PWMPCI_PWM_GENERATOR_3          ((uint32_t)(_PG2EVT_PWMPCI_MASK & ((uint32_t)(2) << _PG2EVT_PWMPCI_POSITION)))
#define PG2EVT_PWMPCI_PWM_GENERATOR_4          ((uint32_t)(_PG2EVT_PWMPCI_MASK & ((uint32_t)(3) << _PG2EVT_PWMPCI_POSITION)))

//PGxEVT UPDTRG[1:0] bits
#define PG2EVT_UPDTRG_MANUAL          ((uint32_t)(_PG2EVT_UPDTRG_MASK & ((uint32_t)(0) << _PG2EVT_UPDTRG_POSITION)))
#define PG2EVT_UPDTRG_PGxDC          ((uint32_t)(_PG2EVT_UPDTRG_MASK & ((uint32_t)(1) << _PG2EVT_UPDTRG_POSITION)))
#define PG2EVT_UPDTRG_PGxPHASE          ((uint32_t)(_PG2EVT_UPDTRG_MASK & ((uint32_t)(2) << _PG2EVT_UPDTRG_POSITION)))
#define PG2EVT_UPDTRG_PGxTRIGA          ((uint32_t)(_PG2EVT_UPDTRG_MASK & ((uint32_t)(3) << _PG2EVT_UPDTRG_POSITION)))

//PGxEVT PGTRGSEL[2:0] bits
#define PG2EVT_PGTRGSEL_EOC          ((uint32_t)(_PG2EVT_PGTRGSEL_MASK & ((uint32_t)(0) << _PG2EVT_PGTRGSEL_POSITION)))
#define PG2EVT_PGTRGSEL_TRIGA_COMPARE          ((uint32_t)(_PG2EVT_PGTRGSEL_MASK & ((uint32_t)(1) << _PG2EVT_PGTRGSEL_POSITION)))
#define PG2EVT_PGTRGSEL_TRIGB_COMPARE          ((uint32_t)(_PG2EVT_PGTRGSEL_MASK & ((uint32_t)(2) << _PG2EVT_PGTRGSEL_POSITION)))
#define PG2EVT_PGTRGSEL_TRIGC_COMPARE          ((uint32_t)(_PG2EVT_PGTRGSEL_MASK & ((uint32_t)(3) << _PG2EVT_PGTRGSEL_POSITION)))
        
//PWM Generator x PCI Termination Polarity options
#define PG3FPCI_PPS_NON_INVERTED          ((uint32_t)(_PG3FPCI_PPS_MASK & ((uint32_t)(0) << _PG3FPCI_PPS_POSITION))) 
#define PG3FPCI_PPS_INVERTED          ((uint32_t)(_PG3FPCI_PPS_MASK & ((uint32_t)(1) << _PG3FPCI_PPS_POSITION))) 

//PGxCON MDCSEL bit
#define PG3CON_MDCSEL_PGxDC          ((uint32_t)(_PG3CON_MDCSEL_MASK & ((uint32_t)(0) << _PG3CON_MDCSEL_POSITION)))
#define PG3CON_MDCSEL_MDC          ((uint32_t)(_PG3CON_MDCSEL_MASK & ((uint32_t)(1) << _PG3CON_MDCSEL_POSITION)))

//PGxCON MPERSEL bit
#define PG3CON_MPERSEL_PGxPER          ((uint32_t)(_PG3CON_MPERSEL_MASK & ((uint32_t)(0) << _PG3CON_MPERSEL_POSITION)))
#define PG3CON_MPERSEL_MPER          ((uint32_t)(_PG3CON_MPERSEL_MASK & ((uint32_t)(1) << _PG3CON_MPERSEL_POSITION)))

//PGxCON MPHSEL bit
#define PG3CON_MPHSEL_PGxPHASE          ((uint32_t)(_PG3CON_MPHSEL_MASK & ((uint32_t)(0) << _PG3CON_MPHSEL_POSITION)))
#define PG3CON_MPHSEL_MPHASE          ((uint32_t)(_PG3CON_MPHSEL_MASK & ((uint32_t)(1) << _PG3CON_MPHSEL_POSITION)))

//PGxCON MSTEN bit
#define PG3CON_MSTEN_DISABLED          ((uint32_t)(_PG3CON_MSTEN_MASK & ((uint32_t)(0) << _PG3CON_MSTEN_POSITION)))
#define PG3CON_MSTEN_ENABLED          ((uint32_t)(_PG3CON_MSTEN_MASK & ((uint32_t)(1) << _PG3CON_MSTEN_POSITION)))

//PWM Generator x Trigger Mode Selection options
#define PG3CON_TRGMOD_SINGLE_TRIGGER_MODE            ((uint32_t)(_PG3CON_TRGMOD_MASK & ((uint32_t)(0) << _PG3CON_TRGMOD_POSITION))) 
#define PG3CON_TRGMOD_RETRIGGRERABLE_MODE            ((uint32_t)(_PG3CON_TRGMOD_MASK & ((uint32_t)(1) << _PG3CON_TRGMOD_POSITION))) 

//PWM Generator x Data Register Update Modes
#define PG3CON_UPDMOD_SOC            ((uint32_t)(_PG3CON_UPDMOD_MASK & ((uint32_t)(0) << _PG3CON_UPDMOD_POSITION))) 
#define PG3CON_UPDMOD_IMMEDIATE            ((uint32_t)(_PG3CON_UPDMOD_MASK & ((uint32_t)(1) << _PG3CON_UPDMOD_POSITION))) 
#define PG3CON_UPDMOD_CLIENT_SOC            ((uint32_t)(_PG3CON_UPDMOD_MASK & ((uint32_t)(2) << _PG3CON_UPDMOD_POSITION))) 
#define PG3CON_UPDMOD_CLIENT_IMMEDIATE            ((uint32_t)(_PG3CON_UPDMOD_MASK & ((uint32_t)(3) << _PG3CON_UPDMOD_POSITION))) 

//PWM Generator x Start-of-Cycle Selection bits
#define PG3CON_SOCS_LOCAL_EOC            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(0) << _PG3CON_SOCS_POSITION)))
#define PG3CON_SOCS_TRIG_OP_BY_PG1_OR_PG5            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(1) << _PG3CON_SOCS_POSITION)))
#define PG3CON_SOCS_TRIG_OP_BY_PG2_OR_PG6            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(2) << _PG3CON_SOCS_POSITION)))
#define PG3CON_SOCS_TRIG_OP_BY_PG3_OR_PG7            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(3) << _PG3CON_SOCS_POSITION)))
#define PG3CON_SOCS_TRIG_OP_BY_PG4_OR_PG8            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(4) << _PG3CON_SOCS_POSITION)))
#define PG3CON_SOCS_TRIG_BIT_OR_PCI_SYNC_FN_ONLY            ((uint32_t)(_PG3CON_SOCS_MASK & ((uint32_t)(15) << _PG3CON_SOCS_POSITION)))

//PWM Generator x CLKSEL bits
#define PG3CON_CLKSEL_NO_CLOCK          ((uint32_t)(_PG3CON_CLKSEL_MASK & ((uint32_t)(0) << _PG3CON_CLKSEL_POSITION)))
#define PG3CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL          ((uint32_t)(_PG3CON_CLKSEL_MASK & ((uint32_t)(1) << _PG3CON_CLKSEL_POSITION)))
#define PG3CON_CLKSEL_MASTER_CLOCK_DIVIDER          ((uint32_t)(_PG3CON_CLKSEL_MASK & ((uint32_t)(2) << _PG3CON_CLKSEL_POSITION)))
#define PG3CON_CLKSEL_MASTER_CLOCK_FREQ_SCALING          ((uint32_t)(_PG3CON_CLKSEL_MASK & ((uint32_t)(3) << _PG3CON_CLKSEL_POSITION)))

#define PG3CON_MODSEL_INDEPENDENT_EDGE          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(0) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_VARIABLE_PHASE          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(1) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_INDEPENDENT_EDGE_DUAL_OUTPUT          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(2) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_CENTER_ALIGNED          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(4) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_DOUBLE_UPDATE_CENTER_ALIGNED          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(5) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_DUALEDGE_CA_ONE_UPDATE_CYCLE          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(6) << _PG3CON_MODSEL_POSITION)))
#define PG3CON_MODSEL_DUALEDGE_CA_TWO_UPDATE_CYCLE          ((uint32_t)(_PG3CON_MODSEL_MASK & ((uint32_t)(7) << _PG3CON_MODSEL_POSITION)))

//PWM Generator x PMOD bits
#define PG3IOCON_PMOD_COMPLEMENTARY_MODE          ((uint32_t)(_PG3IOCON_PMOD_MASK & ((uint32_t)(0) << _PG3IOCON_PMOD_POSITION)))
#define PG3IOCON_PMOD_INDEPENDENT_MODE          ((uint32_t)(_PG3IOCON_PMOD_MASK & ((uint32_t)(1) << _PG3IOCON_PMOD_POSITION)))
#define PG3IOCON_PMOD_PUSH_PULL_MODE          ((uint32_t)(_PG3IOCON_PMOD_MASK & ((uint32_t)(2) << _PG3IOCON_PMOD_POSITION)))
#define PG3IOCON_PMOD_RESERVED          ((uint32_t)(_PG3IOCON_PMOD_MASK & ((uint32_t)(3) << _PG3IOCON_PMOD_POSITION)))

//PWM Generator x POLH bit
#define PG3IOCON_POLH_ACTIVE_HIGH          ((uint32_t)(_PG3IOCON_POLH_MASK & ((uint32_t)(0) << _PG3IOCON_POLH_POSITION)))
#define PG3IOCON_POLH_ACTIVE_LOW          ((uint32_t)(_PG3IOCON_POLH_MASK & ((uint32_t)(1) << _PG3IOCON_POLH_POSITION)))

//PWM Generator x POLL bit
#define PG3IOCON_POLL_ACTIVE_HIGH          ((uint32_t)(_PG3IOCON_POLL_MASK & ((uint32_t)(0) << _PG3IOCON_POLL_POSITION)))
#define PG3IOCON_POLL_ACTIVE_LOW          ((uint32_t)(_PG3IOCON_POLL_MASK & ((uint32_t)(1) << _PG3IOCON_POLL_POSITION)))

//PWM Generator x SWAP bit
#define PG3IOCON_SWAP_NORMAL_MAPPING          ((uint32_t)(_PG3IOCON_SWAP_MASK & ((uint32_t)(0) << _PG3IOCON_SWAP_POSITION)))
#define PG3IOCON_SWAP_SWAPPED          ((uint32_t)(_PG3IOCON_SWAP_MASK & ((uint32_t)(1) << _PG3IOCON_SWAP_POSITION)))

#define PG3IOCON_OVRENH_PWM_GENERATOR          ((uint32_t)(_PG3IOCON_OVRENH_MASK & ((uint32_t)(0) << _PG3IOCON_OVRENH_POSITION)))
#define PG3IOCON_OVRENH_OVRDAT_1          ((uint32_t)(_PG3IOCON_OVRENH_MASK & ((uint32_t)(1) << _PG3IOCON_OVRENH_POSITION)))

//PWM Generator x OVRENL bit
#define PG3IOCON_OVRENL_PWM_GENERATOR          ((uint32_t)(_PG3IOCON_OVRENL_MASK & ((uint32_t)(0) << _PG3IOCON_OVRENL_POSITION)))
#define PG3IOCON_OVRENL_OVRDAT_0          ((uint32_t)(_PG3IOCON_OVRENL_MASK & ((uint32_t)(1) << _PG3IOCON_OVRENL_POSITION)))

//PWM Generator x OSYNC[1:0] bits
#define PG3IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE          ((uint32_t)(_PG3IOCON_OSYNC_MASK & ((uint32_t)(0) << _PG3IOCON_OSYNC_POSITION)))
#define PG3IOCON_OSYNC_SYNC_IMMEDIATE          ((uint32_t)(_PG3IOCON_OSYNC_MASK & ((uint32_t)(1) << _PG3IOCON_OSYNC_POSITION)))
#define PG3IOCON_OSYNC_SYNC_TO_UPDMOD          ((uint32_t)(_PG3IOCON_OSYNC_MASK & ((uint32_t)(2) << _PG3IOCON_OSYNC_POSITION)))
#define PG3IOCON_OSYNC_SYNC_TO_DATA_BUFFER_UPDATE          ((uint32_t)(_PG3IOCON_OSYNC_MASK & ((uint32_t)(3) << _PG3IOCON_OSYNC_POSITION)))

//PGxEVT ADTR2EN3 bit
#define PG3EVT_ADTR2EN3_DISABLED          ((uint32_t)(_PG3EVT_ADTR2EN3_MASK & ((uint32_t)(0) << _PG3EVT_ADTR2EN3_POSITION)))
#define PG3EVT_ADTR2EN3_ENABLED          ((uint32_t)(_PG3EVT_ADTR2EN3_MASK & ((uint32_t)(1) << _PG3EVT_ADTR2EN3_POSITION)))

//PGxEVT ADTR2EN2 bit
#define PG3EVT_ADTR2EN2_DISABLED          ((uint32_t)(_PG3EVT_ADTR2EN2_MASK & ((uint32_t)(0) << _PG3EVT_ADTR2EN2_POSITION)))
#define PG3EVT_ADTR2EN2_ENABLED          ((uint32_t)(_PG3EVT_ADTR2EN2_MASK & ((uint32_t)(1) << _PG3EVT_ADTR2EN2_POSITION)))

//PGxEVT ADTR2EN1 bit
#define PG3EVT_ADTR2EN1_DISABLED          ((uint32_t)(_PG3EVT_ADTR2EN1_MASK & ((uint32_t)(0) << _PG3EVT_ADTR2EN1_POSITION)))
#define PG3EVT_ADTR2EN1_ENABLED          ((uint32_t)(_PG3EVT_ADTR2EN1_MASK & ((uint32_t)(1) << _PG3EVT_ADTR2EN1_POSITION)))

//PGxEVT ADTR1EN3 bit
#define PG3EVT_ADTR1EN3_DISABLED          ((uint32_t)(_PG3EVT_ADTR1EN3_MASK & ((uint32_t)(0) << _PG3EVT_ADTR1EN3_POSITION)))
#define PG3EVT_ADTR1EN3_ENABLED          ((uint32_t)(_PG3EVT_ADTR1EN3_MASK & ((uint32_t)(1) << _PG3EVT_ADTR1EN3_POSITION)))

//PGxEVT ADTR1EN2 bit
#define PG3EVT_ADTR1EN2_DISABLED          ((uint32_t)(_PG3EVT_ADTR1EN2_MASK & ((uint32_t)(0) << _PG3EVT_ADTR1EN2_POSITION)))
#define PG3EVT_ADTR1EN2_ENABLED          ((uint32_t)(_PG3EVT_ADTR1EN2_MASK & ((uint32_t)(1) << _PG3EVT_ADTR1EN2_POSITION)))

//PGxEVT ADTR1EN1 bit
#define PG3EVT_ADTR1EN1_DISABLED          ((uint32_t)(_PG3EVT_ADTR1EN1_MASK & ((uint32_t)(0) << _PG3EVT_ADTR1EN1_POSITION)))
#define PG3EVT_ADTR1EN1_ENABLED          ((uint32_t)(_PG3EVT_ADTR1EN1_MASK & ((uint32_t)(1) << _PG3EVT_ADTR1EN1_POSITION)))

//PGxEVT PWMPCI[2:0] bits
#define PG3EVT_PWMPCI_PWM_GENERATOR_1          ((uint32_t)(_PG3EVT_PWMPCI_MASK & ((uint32_t)(0) << _PG3EVT_PWMPCI_POSITION)))
#define PG3EVT_PWMPCI_PWM_GENERATOR_2          ((uint32_t)(_PG3EVT_PWMPCI_MASK & ((uint32_t)(1) << _PG3EVT_PWMPCI_POSITION)))
#define PG3EVT_PWMPCI_PWM_GENERATOR_3          ((uint32_t)(_PG3EVT_PWMPCI_MASK & ((uint32_t)(2) << _PG3EVT_PWMPCI_POSITION)))
#define PG3EVT_PWMPCI_PWM_GENERATOR_4          ((uint32_t)(_PG3EVT_PWMPCI_MASK & ((uint32_t)(3) << _PG3EVT_PWMPCI_POSITION)))

//PGxEVT UPDTRG[1:0] bits
#define PG3EVT_UPDTRG_MANUAL          ((uint32_t)(_PG3EVT_UPDTRG_MASK & ((uint32_t)(0) << _PG3EVT_UPDTRG_POSITION)))
#define PG3EVT_UPDTRG_PGxDC          ((uint32_t)(_PG3EVT_UPDTRG_MASK & ((uint32_t)(1) << _PG3EVT_UPDTRG_POSITION)))
#define PG3EVT_UPDTRG_PGxPHASE          ((uint32_t)(_PG3EVT_UPDTRG_MASK & ((uint32_t)(2) << _PG3EVT_UPDTRG_POSITION)))
#define PG3EVT_UPDTRG_PGxTRIGA          ((uint32_t)(_PG3EVT_UPDTRG_MASK & ((uint32_t)(3) << _PG3EVT_UPDTRG_POSITION)))

//PGxEVT PGTRGSEL[2:0] bits
#define PG3EVT_PGTRGSEL_EOC          ((uint32_t)(_PG3EVT_PGTRGSEL_MASK & ((uint32_t)(0) << _PG3EVT_PGTRGSEL_POSITION)))
#define PG3EVT_PGTRGSEL_TRIGA_COMPARE          ((uint32_t)(_PG3EVT_PGTRGSEL_MASK & ((uint32_t)(1) << _PG3EVT_PGTRGSEL_POSITION)))
#define PG3EVT_PGTRGSEL_TRIGB_COMPARE          ((uint32_t)(_PG3EVT_PGTRGSEL_MASK & ((uint32_t)(2) << _PG3EVT_PGTRGSEL_POSITION)))
#define PG3EVT_PGTRGSEL_TRIGC_COMPARE          ((uint32_t)(_PG3EVT_PGTRGSEL_MASK & ((uint32_t)(3) << _PG3EVT_PGTRGSEL_POSITION)))
        

//PWM Clock Divider Selection options
#define PCLKCON_DIVSEL_1_2          ((uint32_t)(_PCLKCON_DIVSEL_MASK & ((uint32_t)(0) << _PCLKCON_DIVSEL_POSITION))) 
#define PCLKCON_DIVSEL_1_4          ((uint32_t)(_PCLKCON_DIVSEL_MASK & ((uint32_t)(1) << _PCLKCON_DIVSEL_POSITION))) 
#define PCLKCON_DIVSEL_1_8          ((uint32_t)(_PCLKCON_DIVSEL_MASK & ((uint32_t)(2) << _PCLKCON_DIVSEL_POSITION))) 
#define PCLKCON_DIVSEL_1_16          ((uint32_t)(_PCLKCON_DIVSEL_MASK & ((uint32_t)(3) << _PCLKCON_DIVSEL_POSITION))) 

//PWM Master Clock Selection options
#define PCLKCON_MCLKSEL_UPB_CLOCK           ((uint32_t)(_PCLKCON_MCLKSEL_MASK & ((uint32_t)(0) << _PCLKCON_MCLKSEL_POSITION))) 
#define PCLKCON_MCLKSEL_CLOCK_GEN_5           ((uint32_t)(_PCLKCON_MCLKSEL_MASK & ((uint32_t)(1) << _PCLKCON_MCLKSEL_POSITION))) 

// Section: File specific functions


// Section: PWM Module APIs

void PWM_Initialize (void)
{
    PG1CON = (PG1CON_MDCSEL_PGxDC
            |PG1CON_MPERSEL_PGxPER
            |PG1CON_MPHSEL_PGxPHASE
            |PG1CON_MSTEN_DISABLED
            |PG1CON_UPDMOD_SOC
            |PG1CON_TRGMOD_SINGLE_TRIGGER_MODE
            |PG1CON_SOCS_LOCAL_EOC
            |PG1CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL
            |PG1CON_MODSEL_INDEPENDENT_EDGE);
    
    PG1IOCON = (PG1IOCON_PMOD_COMPLEMENTARY_MODE
            |PG1IOCON_POLH_ACTIVE_HIGH
            |PG1IOCON_POLL_ACTIVE_HIGH
            |PG1IOCON_SWAP_NORMAL_MAPPING
            |PG1IOCON_OVRENH_PWM_GENERATOR
            |PG1IOCON_OVRENL_PWM_GENERATOR
            |(uint32_t)0x0 << _PG1IOCON_OVRDAT_POSITION
            |PG1IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE
            |(uint32_t)0x0 << _PG1IOCON_FLTDAT_POSITION);
            
    PG1EVT = (PG1EVT_ADTR2EN3_DISABLED
            |PG1EVT_ADTR2EN2_DISABLED
            |PG1EVT_ADTR2EN1_DISABLED
            |(uint32_t)0x0 << _PG1EVT_ADTR1OFS_POSITION
            |(uint32_t)0x0 << _PG1EVT_ADTR1PS_POSITION
            |PG1EVT_ADTR1EN3_DISABLED
            |PG1EVT_ADTR1EN2_DISABLED
            |PG1EVT_ADTR1EN1_DISABLED
            |PG1EVT_PWMPCI_PWM_GENERATOR_1
            |PG1EVT_UPDTRG_MANUAL
            |PG1EVT_PGTRGSEL_EOC);
            
    PG1DC = 0x4e200UL;
    
    PG1PER = 0x9c3f0UL;
    
    PG1PHASE = 0x0UL;
    
    PG1DT = 0x0UL;
    
    PG1FPCI = PG1FPCI_PPS_NON_INVERTED | (uint32_t)0x0 << _PG1FPCI_PSS_POSITION;
    
    PG1TRIGA = 0x0UL;
    
    PG1TRIGB = 0x0UL;
    
    PG1TRIGC = 0x0UL;
    
    PG2CON = (PG2CON_MDCSEL_PGxDC
            |PG2CON_MPERSEL_PGxPER
            |PG2CON_MPHSEL_PGxPHASE
            |PG2CON_MSTEN_DISABLED
            |PG2CON_UPDMOD_SOC
            |PG2CON_TRGMOD_SINGLE_TRIGGER_MODE
            |PG2CON_SOCS_LOCAL_EOC
            |PG2CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL
            |PG2CON_MODSEL_INDEPENDENT_EDGE);
    
    PG2IOCON = (PG2IOCON_PMOD_COMPLEMENTARY_MODE
            |PG2IOCON_POLH_ACTIVE_HIGH
            |PG2IOCON_POLL_ACTIVE_HIGH
            |PG2IOCON_SWAP_NORMAL_MAPPING
            |PG2IOCON_OVRENH_PWM_GENERATOR
            |PG2IOCON_OVRENL_PWM_GENERATOR
            |(uint32_t)0x0 << _PG2IOCON_OVRDAT_POSITION
            |PG2IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE
            |(uint32_t)0x0 << _PG2IOCON_FLTDAT_POSITION);
            
    PG2EVT = (PG2EVT_ADTR2EN3_DISABLED
            |PG2EVT_ADTR2EN2_DISABLED
            |PG2EVT_ADTR2EN1_DISABLED
            |(uint32_t)0x0 << _PG2EVT_ADTR1OFS_POSITION
            |(uint32_t)0x0 << _PG2EVT_ADTR1PS_POSITION
            |PG2EVT_ADTR1EN3_DISABLED
            |PG2EVT_ADTR1EN2_DISABLED
            |PG2EVT_ADTR1EN1_DISABLED
            |PG2EVT_PWMPCI_PWM_GENERATOR_1
            |PG2EVT_UPDTRG_MANUAL
            |PG2EVT_PGTRGSEL_EOC);
            
    PG2DC = 0x4e200UL;
    
    PG2PER = 0x9c3f0UL;
    
    PG2PHASE = 0x0UL;
    
    PG2DT = 0x0UL;
    
    PG2FPCI = PG2FPCI_PPS_NON_INVERTED | (uint32_t)0x0 << _PG2FPCI_PSS_POSITION;
    
    PG2TRIGA = 0x0UL;
    
    PG2TRIGB = 0x0UL;
    
    PG2TRIGC = 0x0UL;
    
    PG3CON = (PG3CON_MDCSEL_PGxDC
            |PG3CON_MPERSEL_PGxPER
            |PG3CON_MPHSEL_PGxPHASE
            |PG3CON_MSTEN_DISABLED
            |PG3CON_UPDMOD_SOC
            |PG3CON_TRGMOD_SINGLE_TRIGGER_MODE
            |PG3CON_SOCS_LOCAL_EOC
            |PG3CON_CLKSEL_MASTER_CLOCK_BY_MCLKSEL
            |PG3CON_MODSEL_INDEPENDENT_EDGE);
    
    PG3IOCON = (PG3IOCON_PMOD_COMPLEMENTARY_MODE
            |PG3IOCON_POLH_ACTIVE_HIGH
            |PG3IOCON_POLL_ACTIVE_HIGH
            |PG3IOCON_SWAP_NORMAL_MAPPING
            |PG3IOCON_OVRENH_PWM_GENERATOR
            |PG3IOCON_OVRENL_PWM_GENERATOR
            |(uint32_t)0x0 << _PG3IOCON_OVRDAT_POSITION
            |PG3IOCON_OSYNC_SYNC_TO_LOCAL_PWM_TIMEBASE
            |(uint32_t)0x0 << _PG3IOCON_FLTDAT_POSITION);
            
    PG3EVT = (PG3EVT_ADTR2EN3_DISABLED
            |PG3EVT_ADTR2EN2_DISABLED
            |PG3EVT_ADTR2EN1_DISABLED
            |(uint32_t)0x0 << _PG3EVT_ADTR1OFS_POSITION
            |(uint32_t)0x0 << _PG3EVT_ADTR1PS_POSITION
            |PG3EVT_ADTR1EN3_DISABLED
            |PG3EVT_ADTR1EN2_DISABLED
            |PG3EVT_ADTR1EN1_DISABLED
            |PG3EVT_PWMPCI_PWM_GENERATOR_1
            |PG3EVT_UPDTRG_MANUAL
            |PG3EVT_PGTRGSEL_EOC);
            
    PG3DC = 0x4e200UL;
    
    PG3PER = 0x9c3f0UL;
    
    PG3PHASE = 0x0UL;
    
    PG3DT = 0x0UL;
    
    PG3FPCI = PG3FPCI_PPS_NON_INVERTED | (uint32_t)0x0 << _PG3FPCI_PSS_POSITION;
    
    PG3TRIGA = 0x0UL;
    
    PG3TRIGB = 0x0UL;
    
    PG3TRIGC = 0x0UL;
    
    MDC = 0x0UL;
    
    MPER = 0x9c3f0UL;

    MPHASE = 0x0UL;

    PCLKCON = (PCLKCON_DIVSEL_1_2
            |PCLKCON_MCLKSEL_UPB_CLOCK);
    


    
}

void PWM_Deinitialize (void)
{
    PG1CONbits.ON = 0U;
    PG2CONbits.ON = 0U;
    PG3CONbits.ON = 0U;


    MDC = 0x0UL;

    MPER = 0x0UL;

    MPHASE = 0x0UL;

    PCLKCON = 0x0UL;

    PG1CON = 0x0UL;
    PG1IOCON = 0x0UL;
    PG1STAT = 0x0UL;
    PG1EVT = 0x0UL;
    PG1DC = 0x0UL;
    PG1PER = 0x0UL;
    PG1PHASE = 0x0UL;
    PG1DT = 0x0UL;
    PG1FPCI = 0x0UL;
    PG1TRIGA = 0x0UL;
    PG1TRIGB = 0x0UL;
    PG1TRIGC = 0x0UL;

    PG2CON = 0x0UL;
    PG2IOCON = 0x0UL;
    PG2STAT = 0x0UL;
    PG2EVT = 0x0UL;
    PG2DC = 0x0UL;
    PG2PER = 0x0UL;
    PG2PHASE = 0x0UL;
    PG2DT = 0x0UL;
    PG2FPCI = 0x0UL;
    PG2TRIGA = 0x0UL;
    PG2TRIGB = 0x0UL;
    PG2TRIGC = 0x0UL;

    PG3CON = 0x0UL;
    PG3IOCON = 0x0UL;
    PG3STAT = 0x0UL;
    PG3EVT = 0x0UL;
    PG3DC = 0x0UL;
    PG3PER = 0x0UL;
    PG3PHASE = 0x0UL;
    PG3DT = 0x0UL;
    PG3FPCI = 0x0UL;
    PG3TRIGA = 0x0UL;
    PG3TRIGB = 0x0UL;
    PG3TRIGC = 0x0UL;


    
}


/**
 End of File
*/
