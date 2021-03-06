/* Original copyright (C) 2010 by Creytiv.com. */

#include <inc/string.h>
#include <inc/sha.h>
#include <inc/hmac.h>

/* SHA-1 blocksize */
#define SHA_BLOCKSIZE 64

/*
 * This function computes the digest.
 */
void
hmac_sha1(const uint8_t *key_ptr, size_t key_len,
	const uint8_t *text_ptr, size_t text_len,
	uint8_t *rnd_ptr)
{
	size_t i;
	uint8_t isha[SHA_DIGEST_LENGTH], osha[SHA_DIGEST_LENGTH];
	uint8_t key[SHA_DIGEST_LENGTH];
	uint8_t buf[SHA_BLOCKSIZE];
	SHA_CTX ictx, octx, tctx;

	if (key_len > SHA_BLOCKSIZE)
	{
		SHA1_Init(&tctx);
		SHA1_Update(&tctx, key_ptr, key_len);
		SHA1_Final(key, &tctx);

		key_ptr = key;
		key_len = SHA_DIGEST_LENGTH;
	}

	/* Inner digest */

	SHA1_Init(&ictx);

	/* Pad the key for inner digest */
	
	for (i = 0; i < key_len; i++)
	{
		buf[i] = key_ptr[i] ^ 0x36;
	}

	for (i = key_len; i < SHA_BLOCKSIZE; i++)
	{
		buf[i] = 0x36;
	}

	SHA1_Update(&ictx, buf, SHA_BLOCKSIZE);
	SHA1_Update(&ictx, text_ptr, text_len);

	SHA1_Final(isha, &ictx);

	/* Outher digest */

	SHA1_Init(&octx);

	/* Pad the key for outer digest */

	for (i = 0; i < key_len; i++)
	{
		buf[i] = key_ptr[i] ^ 0x5C;
	}

	for (i = key_len; i < SHA_BLOCKSIZE; i++)
	{
		buf[i] = 0x5C;
	}

	SHA1_Update(&octx, buf, SHA_BLOCKSIZE);
	SHA1_Update(&octx, isha, SHA_DIGEST_LENGTH);

	SHA1_Final(osha, &octx);

	/* Write the truncated results the results */
	memcpy(rnd_ptr, osha, SHA_DIGEST_LENGTH);
}
