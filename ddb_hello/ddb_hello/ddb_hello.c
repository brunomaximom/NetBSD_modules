/*	$NetBSD$	*/

/*-
 * Copyright (c) 2020 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/module.h>

#include <ddb/ddb.h>

static void db_hello(db_expr_t, bool, db_expr_t, const char*);

/*
 * Defines a list of commands to be added.
 * The last one needs to be a NULL command.
 */

static struct db_command db_hello_command_table[] = {
	{ DDB_ADD_CMD("hello", db_hello, 0, "hello from kernel module", NULL, NULL) },
	{ DDB_ADD_CMD(NULL, NULL, 0, NULL, NULL, NULL) }
};

/*
 * Defines what the command will do.
 */

static void 
db_hello(db_expr_t addr __unused, bool have_addr __unused,
	 db_expr_t count __unused, const char *modif __unused)
{

	printf("hello from kernel module\n");
}

/*
 * Last parameter of MODULE macro is a list of names (as string; names are
 * separated by commas) of dependencies.  If module has no dependencies,
 * then NULL should be passed.
 */

MODULE(MODULE_CLASS_MISC, ddb_hello, NULL);

static int
ddb_hello_modcmd(modcmd_t cmd, void *arg __unused)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		/*
		 * Register the command in ddb.
		 * First parameter is the class of the command.
		 * Second parameter is the command table structure itself.
		 */
		db_register_tbl(DDB_BASE_CMD, db_hello_command_table);
		break;

	case MODULE_CMD_FINI:
		/*
		 * Unregister the registered command.
		 * The same parameter of the previous function are needed.
		 */
		db_unregister_tbl(DDB_BASE_CMD, db_hello_command_table);
		break;

	default:
		return ENOTTY;
	}

	return 0;
}
