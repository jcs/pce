/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/arch/ibmpc/hook.c                                        *
 * Created:     2003-09-02 by Hampa Hug <hampa@hampa.ch>                     *
 * Copyright:   (C) 2003-2022 Hampa Hug <hampa@hampa.ch>                     *
 *****************************************************************************/

/*****************************************************************************
 * This program is free software. You can redistribute it and / or modify it *
 * under the terms of the GNU General Public License version 2 as  published *
 * by the Free Software Foundation.                                          *
 *                                                                           *
 * This program is distributed in the hope  that  it  will  be  useful,  but *
 * WITHOUT  ANY   WARRANTY,   without   even   the   implied   warranty   of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  General *
 * Public License for more details.                                          *
 *****************************************************************************/


#include "main.h"
#include "ems.h"
#include "hook.h"
#include "ibmpc.h"
#include "int13.h"
#include "msg.h"
#include "xms.h"

#include <string.h>
#include <time.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <cpu/e8086/e8086.h>

#include <devices/memory.h>

#include <lib/log.h>


static
int pc_int_15 (ibmpc_t *pc)
{
	unsigned       ah;
	unsigned short seg, ofs;
	unsigned long  size, src, dst;

	ah = e86_get_ah (pc->cpu);

	if (ah == 0x87) {
		seg = e86_get_es (pc->cpu);
		ofs = e86_get_si (pc->cpu);
		size = e86_get_cx (pc->cpu) * 2;
		src = e86_get_mem16 (pc->cpu, seg, ofs + 0x12);
		src |= (e86_get_mem8 (pc->cpu, seg, ofs + 0x14) & 0xff) << 16;
		dst = e86_get_mem16 (pc->cpu, seg, ofs + 0x1a);
		dst |= (e86_get_mem8 (pc->cpu, seg, ofs + 0x1c) & 0xff) << 16;
		pce_log (MSG_DEB, "int15: copy %08x -> %08x, %04x\n",
			src, dst, size
		);
		e86_set_cf (pc->cpu, 1);
		e86_set_ah (pc->cpu, 0x86);
		return (0);
	}

	if (ah == 0x88) {
		/* No extended memory. Only XMS. */
		e86_set_cf (pc->cpu, 0);
		e86_set_ax (pc->cpu, 0x0000);
		e86_set_cf (pc->cpu, 1);
		return (0);
	}

	if (ah == 0x90) {
		e86_set_ah (pc->cpu, 0);
		e86_set_cf (pc->cpu, 0);
		if (e86_get_al (pc->cpu) == 0xfd) {
			/* diskette motor start */
			e86_set_cf (pc->cpu, 1);
		}
		return (0);
	}

	if (ah == 0x91) {
		e86_set_ah (pc->cpu, 0);
		e86_set_cf (pc->cpu, 0);
		return (0);
	}

	return (1);
}

static
unsigned get_bcd_8 (unsigned n)
{
	return ((n % 10) + 16 * ((n / 10) % 10));
}

static
int pc_int_1a (ibmpc_t *pc)
{
	unsigned  ah;
	time_t    tm;
	struct tm *tval;

	ah = e86_get_ah (pc->cpu);

	if (pc->support_rtc == 0) {
		return (1);
	}

	if (ah == 0x02) {
		tm = time (NULL);
		tval = localtime (&tm);
		e86_set_ch (pc->cpu, get_bcd_8 (tval->tm_hour));
		e86_set_cl (pc->cpu, get_bcd_8 (tval->tm_min));
		e86_set_dh (pc->cpu, get_bcd_8 (tval->tm_sec));
		e86_set_cf (pc->cpu, 0);

		return (0);
	}

	if (ah == 0x03) {
		e86_set_cf (pc->cpu, 0);
		return (0);
	}

	if (ah == 0x04) {
		tm = time (NULL);
		tval = localtime (&tm);
		e86_set_ch (pc->cpu, get_bcd_8 ((1900 + tval->tm_year) / 100));
		e86_set_cl (pc->cpu, get_bcd_8 (1900 + tval->tm_year));
		e86_set_dh (pc->cpu, get_bcd_8 (tval->tm_mon + 1));
		e86_set_dl (pc->cpu, get_bcd_8 (tval->tm_mday));
		e86_set_cf (pc->cpu, 0);

		return (0);
	}
	else if (ah == 0x05) {
		e86_set_cf (pc->cpu, 0);
		return (0);
	}

	return (1);
}

static
void pc_hook_log (ibmpc_t *pc)
{
	pce_log (MSG_DEB,
		"pce: hook %04X:%04X AX=%04X BX=%04X CX=%04X DX=%04X DS=%04X ES=%04X\n",
		e86_get_cs (pc->cpu), e86_get_ip (pc->cpu),
		e86_get_ax (pc->cpu), e86_get_bx (pc->cpu),
		e86_get_cx (pc->cpu), e86_get_dx (pc->cpu),
		e86_get_ds (pc->cpu), e86_get_es (pc->cpu)
	);
}

void pc_hook_set_result (ibmpc_t *pc, unsigned err)
{
	if ((err & 0xffff) != 0) {
		e86_set_ax (pc->cpu, err);
		e86_set_cf (pc->cpu, 1);
	}
	else {
		e86_set_cf (pc->cpu, 0);
	}
}

static
int pc_hook_get_string (ibmpc_t *pc, unsigned char *dst, unsigned max, unsigned seg, unsigned ofs)
{
	unsigned i;

	for (i = 0; i < max; i++) {
		if ((dst[i] = e86_get_mem8 (pc->cpu, seg, ofs + i)) == 0) {
			return (0);
		}
	}

	return (1);
}

static
void pc_hook_unknown (ibmpc_t *pc)
{
	pc_hook_log (pc);
	pc_hook_set_result (pc, 0xffff);
}

static
void pc_hook_check (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, 0x0fce);
	e86_set_cf (pc->cpu, 0);
}

static
void pc_hook_get_version (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, (PCE_VERSION_MIC << 8) | 0);
	e86_set_dx (pc->cpu, (PCE_VERSION_MAJ << 8) | PCE_VERSION_MIN);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_version_str (ibmpc_t *pc)
{
	unsigned       i, n;
	unsigned short seg, ofs, max;
	const char     *str;

	seg = e86_get_es (pc->cpu);
	ofs = e86_get_di (pc->cpu);
	max = e86_get_cx (pc->cpu);
	str = PCE_VERSION_STR;

	n = strlen (str) + 1;

	if (n > max) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

	for (i = 0; i < n; i++) {
		e86_set_mem8 (pc->cpu, seg, ofs + i, str[i]);
	}

	pc_hook_set_result (pc, 0);
}

static
void pc_hook_stop (ibmpc_t *pc)
{
	pc->brk = 1;
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_abort (ibmpc_t *pc)
{
	pc->brk = 2;
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_set_msg (ibmpc_t *pc)
{
	unsigned char msg[256];
	unsigned char val[256];

	if (pc_hook_get_string (pc, msg, 256, e86_get_ds (pc->cpu), e86_get_si (pc->cpu))) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

	if (pc_hook_get_string (pc, val, 256, e86_get_es (pc->cpu), e86_get_di (pc->cpu))) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

	e86_set_ax (pc->cpu, 0);

	if (pc_set_msg (pc, (char *) msg, (char *) val)) {
		pc_hook_set_result (pc, 0x0001);
	}
	else {
		pc_hook_set_result (pc, 0);
	}
}

static
void pc_hook_get_time_unix (ibmpc_t *pc)
{
	unsigned long long tm;

	if (pc->support_rtc == 0) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

	tm = (unsigned long long) time (NULL);

	e86_set_ax (pc->cpu, tm & 0xffff);
	tm >>= 16;
	e86_set_dx (pc->cpu, tm & 0xffff);
	tm >>= 16;
	e86_set_cx (pc->cpu, tm & 0xffff);
	tm >>= 16;
	e86_set_bx (pc->cpu, tm & 0xffff);

	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_time_bios (ibmpc_t *pc, int local)
{
	unsigned long v;
	time_t        tm;
	struct tm     *tv;

	if (pc->support_rtc == 0) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

	tm = time (NULL);

	if (local) {
		tv = localtime (&tm);
	}
	else {
		tv = gmtime (&tm);
	}

	v = (60UL * ((60U * tv->tm_hour) + tv->tm_min)) + tv->tm_sec;
	v = ((unsigned long long) PCE_IBMPC_CLK2 * v) / 65536;

	e86_set_ax (pc->cpu, v & 0xffff);
	e86_set_dx (pc->cpu, (v >> 16) & 0xffff);

	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_calendar (ibmpc_t *pc, int local)
{
	unsigned  cs;
	time_t    tm;
	struct tm *tv;

	if (pc->support_rtc == 0) {
		pc_hook_set_result (pc, 0xffff);
		return;
	}

#ifdef HAVE_GETTIMEOFDAY
	{
		struct timeval tv;

		if (gettimeofday (&tv, NULL)) {
			tm = time (NULL);
			cs = 0;
		}
		else {
			tm = tv.tv_sec;
			cs = tv.tv_usec / 10000;
		}
	}
#else
	tm = time (NULL);
	cs = 0;
#endif

	if (local) {
		tv = localtime (&tm);
	}
	else {
		tv = gmtime (&tm);
	}

	e86_set_ax (pc->cpu, 1900 + tv->tm_year);
	e86_set_bh (pc->cpu, tv->tm_mon);
	e86_set_bl (pc->cpu, tv->tm_mday - 1);
	e86_set_ch (pc->cpu, tv->tm_hour);
	e86_set_cl (pc->cpu, tv->tm_min);
	e86_set_dh (pc->cpu, tv->tm_sec);
	e86_set_dl (pc->cpu, cs);

	pc_hook_set_result (pc, 0);
}

static
void pc_hook_xms (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, e86_get_bp (pc->cpu));
	xms_handler (pc->xms, pc->cpu);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_xms_info (ibmpc_t *pc)
{
	xms_info (pc->xms, pc->cpu);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_ems (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, e86_get_bp (pc->cpu));
	ems_handler (pc->ems, pc->cpu);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_ems_info (ibmpc_t *pc)
{
	ems_info (pc->ems, pc->cpu);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_int (ibmpc_t *pc, unsigned n)
{
	int      exec;
	unsigned ss, sp, ax;

	ss = e86_get_ss (pc->cpu);
	sp = e86_get_sp (pc->cpu);
	ax = e86_get_mem16 (pc->cpu, ss, sp);
	e86_set_sp (pc->cpu, e86_get_sp (pc->cpu) + 2);
	e86_set_ax (pc->cpu, ax);

	exec = 0;

	if (n == 0x13) {
		if (pc_int13_check (pc)) {
			dsk_int13 (pc->dsk, pc->cpu);
			exec = 1;
		}
	}
	else if (n == 0x15) {
		exec = !pc_int_15 (pc);
	}
	else if (n == 0x1a) {
		exec = !pc_int_1a (pc);
	}
	else {
		pc_hook_log (pc);
	}

	if (exec) {
		e86_set_ip (pc->cpu, e86_get_ip (pc->cpu) + 2);
		e86_pq_init (pc->cpu);
	}
}

static
void pc_hook_get_model (ibmpc_t *pc)
{
	unsigned model;

	model = 0;

	if (pc->model & PCE_IBMPC_5150) {
		model |= 1;
	}

	if (pc->model & PCE_IBMPC_5160) {
		model |= 2;
	}

	e86_set_ax (pc->cpu, model);

	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_com (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, (pc->serport[0] != NULL) ? pc->serport[0]->io : 0);
	e86_set_bx (pc->cpu, (pc->serport[1] != NULL) ? pc->serport[1]->io : 0);
	e86_set_cx (pc->cpu, (pc->serport[2] != NULL) ? pc->serport[2]->io : 0);
	e86_set_dx (pc->cpu, (pc->serport[3] != NULL) ? pc->serport[3]->io : 0);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_lpt (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, (pc->parport[0] != NULL) ? pc->parport[0]->io : 0);
	e86_set_bx (pc->cpu, (pc->parport[1] != NULL) ? pc->parport[1]->io : 0);
	e86_set_cx (pc->cpu, (pc->parport[2] != NULL) ? pc->parport[2]->io : 0);
	e86_set_dx (pc->cpu, (pc->parport[3] != NULL) ? pc->parport[3]->io : 0);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_boot (ibmpc_t *pc)
{
	e86_set_ax (pc->cpu, pc_get_bootdrive (pc) & 0xff);
	pc_hook_set_result (pc, 0);
}

static
void pc_hook_get_drvcnt (ibmpc_t *pc)
{
	e86_set_ah (pc->cpu, pc->hd_cnt);
	e86_set_al (pc->cpu, pc->fd_cnt);
	pc_hook_set_result (pc, 0);
}

int pc_hook (void *ext)
{
	unsigned op;
	ibmpc_t  *pc;

	pc = ext;

	op = e86_get_ax (pc->cpu);

	switch (op & 0xff00) {
	case PCE_HOOK_INT:
		pc_hook_int (pc, op & 0xff);
		return (0);
	}

	switch (op) {
	case PCE_HOOK_CHECK:
		pc_hook_check (pc);
		break;

	case PCE_HOOK_GET_VERSION:
		pc_hook_get_version (pc);
		break;

	case PCE_HOOK_GET_VERSION_STR:
		pc_hook_get_version_str (pc);
		break;

	case PCE_HOOK_STOP:
		pc_hook_stop (pc);
		break;

	case PCE_HOOK_ABORT:
		pc_hook_abort (pc);
		break;

	case PCE_HOOK_SET_MSG:
		pc_hook_set_msg (pc);
		break;

	case PCE_HOOK_GET_TIME_UNIX:
		pc_hook_get_time_unix (pc);
		break;

	case PCE_HOOK_GET_TIME_LOCAL:
		pc_hook_get_calendar (pc, 1);
		break;

	case PCE_HOOK_GET_TIME_UTC:
		pc_hook_get_calendar (pc, 0);
		break;

	case PCE_HOOK_GET_TIME_BIOS:
		pc_hook_get_time_bios (pc, 1);
		break;

	case PCE_HOOK_XMS:
		pc_hook_xms (pc);
		break;

	case PCE_HOOK_XMS_INFO:
		pc_hook_xms_info (pc);
		break;

	case PCE_HOOK_EMS:
		pc_hook_ems (pc);
		break;

	case PCE_HOOK_EMS_INFO:
		pc_hook_ems_info (pc);
		break;

	case PCE_HOOK_GET_MODEL:
		pc_hook_get_model (pc);
		break;

	case PCE_HOOK_GET_COM:
		pc_hook_get_com (pc);
		break;

	case PCE_HOOK_GET_LPT:
		pc_hook_get_lpt (pc);
		break;

	case PCE_HOOK_GET_BOOT:
		pc_hook_get_boot (pc);
		break;

	case PCE_HOOK_GET_DRVCNT:
		pc_hook_get_drvcnt (pc);
		break;

	default:
		pc_hook_unknown (pc);
		break;
	}

	return (0);
}

static
void pc_hook_version (ibmpc_t *pc)
{
	unsigned short es, di;
	const char     *str;

	es = e86_get_es (pc->cpu);
	di = e86_get_di (pc->cpu);
	str = PCE_VERSION_STR;

	e86_set_ax (pc->cpu, (PCE_VERSION_MAJ << 8) | PCE_VERSION_MIN);
	e86_set_dx (pc->cpu, (PCE_VERSION_MIC << 8));

	if ((es == 0) && (di == 0)) {
		return;
	}

	while (*str != 0) {
		e86_set_mem8 (pc->cpu, es, di, *str);
		di = (di + 1) & 0xffff;
		str += 1;
	}

	e86_set_mem8 (pc->cpu, es, di, 0);
}

static
void pc_hook_msg (ibmpc_t *pc)
{
	unsigned       i, p;
	unsigned short ds;
	char           msg[256];
	char           val[256];

	e86_set_cf (pc->cpu, 1);

	ds = e86_get_ds (pc->cpu);

	p = e86_get_si (pc->cpu);
	for (i = 0; i < 256; i++) {
		msg[i] = e86_get_mem16 (pc->cpu, ds, p++);
		if (msg[i] == 0) {
			break;
		}
	}

	if (i >= 256) {
		return;
	}

	p = e86_get_di (pc->cpu);
	for (i = 0; i < 256; i++) {
		val[i] = e86_get_mem16 (pc->cpu, ds, p++);
		if (val[i] == 0) {
			break;
		}
	}

	if (i >= 256) {
		return;
	}

	if (pc_set_msg (pc, msg, val)) {
		e86_set_cf (pc->cpu, 1);
		e86_set_ax (pc->cpu, 0x0001);
	}
	else {
		e86_set_cf (pc->cpu, 0);
		e86_set_ax (pc->cpu, 0x0000);
	}
}

void pc_hook_old (void *ext, unsigned char op1, unsigned char op2)
{
	ibmpc_t       *pc;
	unsigned long msk;

	pc = (ibmpc_t *) ext;

	pc_hook_log (pc);

	switch (op1) {
	case (PCEH_INT & 0xff):
		if (op2 == 0x13) {
			dsk_int13 (pc->dsk, pc->cpu);
			return;
		}
		else if (op2 == 0x1a) {
			pc_int_1a (pc);
			return;
		}
		else if (op2 == 0x15) {
			pc_int_15 (pc);
			return;
		}
		break;

	case (PCEH_CHECK_INT & 0xff):
		e86_set_ax (pc->cpu, 0);

		if (op2 == 0x13) {
			dsk_int_13_check (pc);
		}
		return;
	}

	e86_set_cf (pc->cpu, 0);

	switch ((op2 << 8) | op1) {
	case PCEH_STOP:
		pc->brk = 1;
		break;

	case PCEH_ABORT:
		pc->brk = 2;
		break;

	case PCEH_SET_BOOT:
		pc_set_bootdrive (pc, e86_get_al (pc->cpu));
		break;

	case PCEH_SET_INT28:
		/* not supported anymore */
		break;

	case PCEH_SET_CPU:
		/* not supported anymore */
		break;

	case PCEH_SET_AMSK:
		msk = (e86_get_dx (pc->cpu) << 16) + e86_get_ax (pc->cpu);
		e86_set_addr_mask (pc->cpu, msk);
		break;

	case PCEH_GET_BOOT:
		e86_set_al (pc->cpu, pc_get_bootdrive (pc));
		break;

	case PCEH_GET_COM:
		e86_set_ax (pc->cpu, (pc->serport[0] != NULL) ? pc->serport[0]->io : 0);
		e86_set_bx (pc->cpu, (pc->serport[1] != NULL) ? pc->serport[1]->io : 0);
		e86_set_cx (pc->cpu, (pc->serport[2] != NULL) ? pc->serport[2]->io : 0);
		e86_set_dx (pc->cpu, (pc->serport[3] != NULL) ? pc->serport[3]->io : 0);
		break;

	case PCEH_GET_LPT:
		e86_set_ax (pc->cpu, (pc->parport[0] != NULL) ? pc->parport[0]->io : 0);
		e86_set_bx (pc->cpu, (pc->parport[1] != NULL) ? pc->parport[1]->io : 0);
		e86_set_cx (pc->cpu, (pc->parport[2] != NULL) ? pc->parport[2]->io : 0);
		e86_set_dx (pc->cpu, (pc->parport[3] != NULL) ? pc->parport[3]->io : 0);
		break;

	case PCEH_GET_VIDEO:
		/* not supported anymore */
		e86_set_ax (pc->cpu, 0);
		break;

	case PCEH_GET_INT28:
		/* not supported anymore */
		e86_set_ax (pc->cpu, 0);
		break;

	case PCEH_GET_CPU:
		/* not supported anymore */
		e86_set_ax (pc->cpu, 0);
		break;

	case PCEH_GET_AMSK:
		msk = e86_get_addr_mask (pc->cpu);
		e86_set_ax (pc->cpu, msk & 0xffff);
		e86_set_dx (pc->cpu, (msk >> 16) & 0xffff);
		break;

	case PCEH_GET_FDCNT:
		e86_set_ax (pc->cpu, pc->fd_cnt);
		break;

	case PCEH_GET_HDCNT:
		e86_set_ax (pc->cpu, pc->hd_cnt);
		break;

	case PCEH_GET_VERS:
		pc_hook_version (pc);
		break;

	case PCEH_XMS:
		xms_handler (pc->xms, pc->cpu);
		break;

	case PCEH_XMS_INFO:
		xms_info (pc->xms, pc->cpu);
		break;

	case PCEH_EMS:
		ems_handler (pc->ems, pc->cpu);
		break;

	case PCEH_EMS_INFO:
		ems_info (pc->ems, pc->cpu);
		break;

	case PCEH_MSG:
		pc_hook_msg (pc);
		break;

	default:
		e86_set_cf (pc->cpu, 1);
		pc_hook_log (pc);
		break;
	}
}
