/*	OpenBSD: sha2.h,v 1.6 2004/06/22 01:57:30 jfb Exp 	*/

/*
 * FILE:	sha2.h
 * AUTHOR:	Aaron D. Gifford <me@aarongifford.com>
 *
 * Copyright (c) 2000-2001, Aaron D. Gifford
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
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $From: sha2.h,v 1.1 2001/11/08 00:02:01 adg Exp adg $
 */

/* OPENBSD ORIGINAL: include/sha2.h */

#ifndef _SSHSHA2_H
#define _SSHSHA2_H

#include "includes.h"

#ifdef WITH_OPENSSL
# include <openssl/opensslv.h>
# if !defined(HAVE_EVP_SHA256) && (OPENSSL_VERSION_NUMBER >= 0x00907000L)
#  define _NEED_SHA2 1
# endif
#else
# define _NEED_SHA2 1
#endif

#if defined(_NEED_SHA2) && !defined(HAVE_SHA256_UPDATE)

/*** SHA-256/384/512 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH		64
#define SHA256_DIGEST_LENGTH		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH		128
#define SHA384_DIGEST_LENGTH		48
#define SHA384_DIGEST_STRING_LENGTH	(SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH		128
#define SHA512_DIGEST_LENGTH		64
#define SHA512_DIGEST_STRING_LENGTH	(SHA512_DIGEST_LENGTH * 2 + 1)


/*** SHA-256/384/512 Context Structures *******************************/
typedef struct _SHA256_CTX {
	uint32_t	state[8];
	uint64_t	bitcount;
	uint8_t	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;
typedef struct _SHA512_CTX {
	uint64_t	state[8];
	uint64_t	bitcount[2];
	uint8_t	buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

typedef SHA512_CTX SHA384_CTX;

void SHA256_Init(SHA256_CTX *);
void SHA256_Transform(uint32_t state[8], const uint8_t [SHA256_BLOCK_LENGTH]);
void SHA256_Update(SHA256_CTX *, const uint8_t *, size_t);
void SHA256_Pad(SHA256_CTX *);
void SHA256_Final(uint8_t[SHA256_DIGEST_LENGTH], SHA256_CTX *);

void SHA384_Init(SHA384_CTX *);
void SHA384_Transform(uint64_t state[8], const uint8_t [SHA384_BLOCK_LENGTH]);
void SHA384_Update(SHA384_CTX *, const uint8_t *, size_t);
void SHA384_Pad(SHA384_CTX *);
void SHA384_Final(uint8_t[SHA384_DIGEST_LENGTH], SHA384_CTX *);

void SHA512_Init(SHA512_CTX *);
void SHA512_Transform(uint64_t state[8], const uint8_t [SHA512_BLOCK_LENGTH]);
void SHA512_Update(SHA512_CTX *, const uint8_t *, size_t);
void SHA512_Pad(SHA512_CTX *);
void SHA512_Final(uint8_t[SHA512_DIGEST_LENGTH], SHA512_CTX *);

#endif /* defined(_NEED_SHA2) && !defined(HAVE_SHA256_UPDATE) */

#endif /* _SSHSHA2_H */
