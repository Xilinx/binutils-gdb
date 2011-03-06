/* Blackfin Universal Asynchronous Receiver/Transmitter (UART) model.
   For "old style" UARTs on BF53x/etc... parts.

   Copyright (C) 2010-2011 Free Software Foundation, Inc.
   Contributed by Analog Devices, Inc.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef DV_BFIN_UART_H
#define DV_BFIN_UART_H

/* XXX: This should be pushed into the model data.  */
#define BFIN_MMR_UART_SIZE	0x30

struct bfin_uart;
bu16 bfin_uart_get_next_byte (struct hw *, bu16, bool *fresh);
bu16 bfin_uart_write_byte (struct hw *, bu16);
bu16 bfin_uart_get_status (struct hw *);
unsigned bfin_uart_write_buffer (struct hw *, const unsigned char *, unsigned);
unsigned bfin_uart_read_buffer (struct hw *, unsigned char *, unsigned);
void bfin_uart_reschedule (struct hw *);

/* UART_LCR */
#define DLAB (1 << 7)

/* UART_LSR */
#define TEMT (1 << 6)
#define THRE (1 << 5)
#define DR (1 << 0)

/* UART_IER */
#define ERBFI (1 << 0)
#define ETBEI (1 << 1)
#define ELSI  (1 << 2)

#endif
