/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/* **** Includes **** */
#include <stddef.h>
#include "mxc_device.h"
#include "mxc_assert.h"
#include "gpio.h"
#include "gpio_reva.h"
#include "gpio_common.h"
#include "mxc_sys.h"

/* **** Definitions **** */
#define MXC_GPIO_HART_UART (MXC_GPIO_PIN_14 | MXC_GPIO_PIN_15 | MXC_GPIO_PIN_16 | MXC_GPIO_PIN_17)

/* **** Functions **** */

int MXC_GPIO_Init(uint32_t portmask)
{
    if (portmask & MXC_GPIO_PORT_0) {
        MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO0);
    }

    if (portmask & MXC_GPIO_PORT_1) {
        MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO1);
    }

    return MXC_GPIO_Common_Init(portmask);
}

int MXC_GPIO_Shutdown(uint32_t portmask)
{
    if (portmask & MXC_GPIO_PORT_0) {
        MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_GPIO0);
    }

    if (portmask & MXC_GPIO_PORT_1) {
        MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_GPIO1);
    }

    return E_NO_ERROR;
}

int MXC_GPIO_Reset(uint32_t portmask)
{
    if (portmask & MXC_GPIO_PORT_0) {
        MXC_SYS_Reset_Periph(MXC_SYS_RESET0_GPIO0);
    }

    if (portmask & MXC_GPIO_PORT_1) {
        MXC_SYS_Reset_Periph(MXC_SYS_RESET0_GPIO1);
    }

    return E_NO_ERROR;
}

int MXC_GPIO_Config(const mxc_gpio_cfg_t *cfg)
{
    int error;
    mxc_gpio_regs_t *gpio = cfg->port;

    // Don't use inaccessible pins
    if ((gpio == MXC_GPIO0) && (cfg->mask & MXC_GPIO_INACCESSIBLE)) {
        return E_NOT_SUPPORTED;
    }

    // Make sure UART2 pins aren't used by the HART Modem
    if ((gpio == MXC_GPIO0) && (cfg->mask & MXC_GPIO_HART_UART) &&
        (cfg->func == MXC_GPIO_FUNC_ALT2)) {
        if ((gpio->en0 & MXC_GPIO_HART_UART) == 0 && (gpio->en1 & MXC_GPIO_HART_UART) &&
            (gpio->en2 & MXC_GPIO_HART_UART) == 0) {
            return E_BAD_STATE;
        }
    }

    // Configure alternate function
    error = MXC_GPIO_RevA_SetAF((mxc_gpio_reva_regs_t *)gpio, cfg->func, cfg->mask);
    if (error != E_NO_ERROR) {
        return error;
    }

    // Configure the pad
    switch (cfg->pad) {
    case MXC_GPIO_PAD_NONE:
        gpio->padctrl0 &= ~cfg->mask;
        break;

    case MXC_GPIO_PAD_PULL_UP:
        gpio->padctrl0 |= cfg->mask;
        gpio->ps |= cfg->mask;
        break;

    case MXC_GPIO_PAD_PULL_DOWN:
        gpio->padctrl0 |= cfg->mask;
        gpio->ps &= ~cfg->mask;
        break;

    default:
        return E_BAD_PARAM;
    }

    return E_NO_ERROR;
}

uint32_t MXC_GPIO_InGet(mxc_gpio_regs_t *port, uint32_t mask)
{
    return MXC_GPIO_RevA_InGet((mxc_gpio_reva_regs_t *)port, mask);
}

void MXC_GPIO_OutSet(mxc_gpio_regs_t *port, uint32_t mask)
{
    MXC_GPIO_RevA_OutSet((mxc_gpio_reva_regs_t *)port, mask);
}

void MXC_GPIO_OutClr(mxc_gpio_regs_t *port, uint32_t mask)
{
    MXC_GPIO_RevA_OutClr((mxc_gpio_reva_regs_t *)port, mask);
}

uint32_t MXC_GPIO_OutGet(mxc_gpio_regs_t *port, uint32_t mask)
{
    return MXC_GPIO_RevA_OutGet((mxc_gpio_reva_regs_t *)port, mask);
}

void MXC_GPIO_OutPut(mxc_gpio_regs_t *port, uint32_t mask, uint32_t val)
{
    MXC_GPIO_RevA_OutPut((mxc_gpio_reva_regs_t *)port, mask, val);
}

void MXC_GPIO_OutToggle(mxc_gpio_regs_t *port, uint32_t mask)
{
    MXC_GPIO_RevA_OutToggle((mxc_gpio_reva_regs_t *)port, mask);
}

int MXC_GPIO_IntConfig(const mxc_gpio_cfg_t *cfg, mxc_gpio_int_pol_t pol)
{
    return MXC_GPIO_RevA_IntConfig(cfg, pol);
}

void MXC_GPIO_EnableInt(mxc_gpio_regs_t *port, uint32_t mask)
{
    MXC_GPIO_RevA_EnableInt((mxc_gpio_reva_regs_t *)port, mask);
}

void MXC_GPIO_DisableInt(mxc_gpio_regs_t *port, uint32_t mask)
{
    MXC_GPIO_RevA_DisableInt((mxc_gpio_reva_regs_t *)port, mask);
}

void MXC_GPIO_RegisterCallback(const mxc_gpio_cfg_t *cfg, mxc_gpio_callback_fn func, void *cbdata)
{
    MXC_GPIO_Common_RegisterCallback(cfg, func, cbdata);
}

void MXC_GPIO_Handler(unsigned int port)
{
    MXC_GPIO_Common_Handler(port);
}

void MXC_GPIO_ClearFlags(mxc_gpio_regs_t *port, uint32_t flags)
{
    MXC_GPIO_RevA_ClearFlags((mxc_gpio_reva_regs_t *)port, flags);
}

uint32_t MXC_GPIO_GetFlags(mxc_gpio_regs_t *port)
{
    return MXC_GPIO_RevA_GetFlags((mxc_gpio_reva_regs_t *)port);
}

int MXC_GPIO_SetVSSEL(mxc_gpio_regs_t *port, mxc_gpio_vssel_t vssel, uint32_t mask)
{
    return E_NOT_SUPPORTED;
}
