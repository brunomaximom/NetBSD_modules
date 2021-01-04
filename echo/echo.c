/*	$NetBSD: hello.c,v 1.1 2015/05/13 07:07:36 pgoyette Exp $	*/

/*-
 * Copyright (c) 2015 The NetBSD Foundation, Inc.
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
__KERNEL_RCSID(0, "$NetBSD: hello.c,v 1.1 2015/05/13 07:07:36 pgoyette Exp $");

#include <sys/param.h>
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/cdefs.h>

static void
echo(prop_dictionary_t props)
{
	int i, argc = 2;
	const char *msg = NULL;
	prop_string_t str;

	if (props != NULL) {
		str = prop_dictionary_get(props, "str");
 		if (str != NULL){
                	msg = prop_string_cstring_nocopy(str);
			msg = __UNCONST(msg);	//tira o modificador const
			argc--;
		 
			if((msg[0] == '-') && (msg[1] == 'n')){
                        	msg = &msg[3];
				for(i=1; i<=argc; i++)
                                	printf("%s%c", msg, i==argc? ' ': ' ');
			} else {
				for(i=1; i<=argc; i++)
					printf("%s%c", msg, i==argc? '\n': ' ');
			}
                } else {
			for(i=1; i<=argc; i++)
				 printf("%s%c", "", i==argc? '\n': ' ');
		}
	}
}

/*
 * Last parameter of MODULE macro is a list of names (as string; names are
 * separated by commas) of dependencies.  If module has no dependencies,
 * then NULL should be passed.
 */

MODULE(MODULE_CLASS_MISC, echo, NULL);

static int
echo_modcmd(modcmd_t cmd, void *arg)
{
	switch (cmd) {
	case MODULE_CMD_INIT:
		echo(arg);
		break;

	case MODULE_CMD_FINI:
		printf("Example module unloaded.\n");
		break;

	case MODULE_CMD_STAT:
		printf("Example module status queried.\n");
		break;

	default:
		return ENOTTY;
	}

	return 0;
}
