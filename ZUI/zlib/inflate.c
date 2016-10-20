#include "zutil.h"
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"


int ZEXPORT inflateResetKeep(strm)
z_streamp strm;
{
    struct inflate_state *state;

    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    strm->total_in = strm->total_out = state->total = 0;
    strm->msg = Z_NULL;
    if (state->wrap)        /* to support ill-conceived Java test suite */
        strm->adler = state->wrap & 1;
    state->mode = HEAD;
    state->last = 0;
    state->havedict = 0;
    state->dmax = 32768U;
    state->hold = 0;
    state->bits = 0;
    state->lencode = state->distcode = state->next = state->codes;
    state->sane = 1;
    state->back = -1;
    return Z_OK;
}

int ZEXPORT inflateReset(strm)
z_streamp strm;
{
    struct inflate_state *state;

    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    state->wsize = 0;
    state->whave = 0;
    state->wnext = 0;
    return inflateResetKeep(strm);
}

int ZEXPORT inflateReset2(strm, windowBits)
z_streamp strm;
int windowBits;
{
    int wrap;
    struct inflate_state *state;

    /* get the state */
    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;

    /* extract wrap request from windowBits parameter */
    if (windowBits < 0) {
        wrap = 0;
        windowBits = -windowBits;
    }
    else {
        wrap = (windowBits >> 4) + 1;
    }

    /* set number of window bits, free window if different */
    if (windowBits && (windowBits < 8 || windowBits > 15))
        return Z_STREAM_ERROR;
    if (state->window != Z_NULL && state->wbits != (unsigned)windowBits) {
        ZFREE(strm, state->window);
        state->window = Z_NULL;
    }

    /* update state and reset the rest of it */
    state->wrap = wrap;
    state->wbits = (unsigned)windowBits;
    return inflateReset(strm);
}

int ZEXPORT inflateInit2_(strm, windowBits, version, stream_size)
z_streamp strm;
int windowBits;
const char *version;
int stream_size;
{
    int ret;
    struct inflate_state *state;

    if (version == Z_NULL || version[0] != ZLIB_VERSION[0] ||
        stream_size != (int)(sizeof(z_stream)))
        return Z_VERSION_ERROR;
    if (strm == Z_NULL) return Z_STREAM_ERROR;
    strm->msg = Z_NULL;                 /* in case we return an error */
    if (strm->zalloc == (alloc_func)0) {
        strm->zalloc = zcalloc;
        strm->opaque = (voidpf)0;
    }
    if (strm->zfree == (free_func)0)
        strm->zfree = zcfree;
    state = (struct inflate_state *)
            ZALLOC(strm, 1, sizeof(struct inflate_state));
    if (state == Z_NULL) return Z_MEM_ERROR;
    strm->state = (struct internal_state *)state;
    state->window = Z_NULL;
    ret = inflateReset2(strm, windowBits);
    if (ret != Z_OK) {
        ZFREE(strm, state);
        strm->state = Z_NULL;
    }
    return ret;
}

local void fixedtables(state)
struct inflate_state *state;
{
#include "inffixed.h"
    state->lencode = lenfix;
    state->lenbits = 9;
    state->distcode = distfix;
    state->distbits = 5;
}

local int updatewindow(strm, end, copy)
z_streamp strm;
const Bytef *end;
unsigned copy;
{
    struct inflate_state *state;
    unsigned dist;

    state = (struct inflate_state *)strm->state;

    /* if it hasn't been done already, allocate space for the window */
    if (state->window == Z_NULL) {
        state->window = (unsigned char *)
                        ZALLOC(strm, 1U << state->wbits,
                               sizeof(unsigned char));
        if (state->window == Z_NULL) return 1;
    }

    /* if window not in use yet, initialize */
    if (state->wsize == 0) {
        state->wsize = 1U << state->wbits;
        state->wnext = 0;
        state->whave = 0;
    }

    /* copy state->wsize or less output bytes into the circular window */
    if (copy >= state->wsize) {
        zmemcpy(state->window, end - state->wsize, state->wsize);
        state->wnext = 0;
        state->whave = state->wsize;
    }
    else {
        dist = state->wsize - state->wnext;
        if (dist > copy) dist = copy;
        zmemcpy(state->window + state->wnext, end - copy, dist);
        copy -= dist;
        if (copy) {
            zmemcpy(state->window, end - copy, copy);
            state->wnext = copy;
            state->whave = state->wsize;
        }
        else {
            state->wnext += dist;
            if (state->wnext == state->wsize) state->wnext = 0;
            if (state->whave < state->wsize) state->whave += dist;
        }
    }
    return 0;
}

/* Macros for inflate(): */

/* check function to use adler32() for zlib or crc32() for gzip */
#  define UPDATE(check, buf, len) adler32(check, buf, len)


/* Load registers with state in inflate() for speed */
#define LOAD() \
    do { \
        put = strm->next_out; \
        left = strm->avail_out; \
        next = strm->next_in; \
        have = strm->avail_in; \
        hold = state->hold; \
        bits = state->bits; \
    } while (0)

/* Restore state from registers in inflate() */
#define RESTORE() \
    do { \
        strm->next_out = put; \
        strm->avail_out = left; \
        strm->next_in = next; \
        strm->avail_in = have; \
        state->hold = hold; \
        state->bits = bits; \
    } while (0)

/* Clear the input bit accumulator */
#define INITBITS() \
    do { \
        hold = 0; \
        bits = 0; \
    } while (0)

/* Get a byte of input into the bit accumulator, or return from inflate()
   if there is no input available. */
#define PULLBYTE() \
    do { \
        if (have == 0) goto inf_leave; \
        have--; \
        hold += (unsigned long)(*next++) << bits; \
        bits += 8; \
    } while (0)

/* Assure that there are at least n bits in the bit accumulator.  If there is
   not enough available input to do that, then return from inflate(). */
#define NEEDBITS(n) \
    do { \
        while (bits < (unsigned)(n)) \
            PULLBYTE(); \
    } while (0)

/* Return the low n bits of the bit accumulator (n < 16) */
#define BITS(n) \
    ((unsigned)hold & ((1U << (n)) - 1))

/* Remove n bits from the bit accumulator */
#define DROPBITS(n) \
    do { \
        hold >>= (n); \
        bits -= (unsigned)(n); \
    } while (0)

/* Remove zero to seven bits as needed to go to a byte boundary */
#define BYTEBITS() \
    do { \
        hold >>= bits & 7; \
        bits -= bits & 7; \
    } while (0)


int ZEXPORT inflate(strm, flush)
z_streamp strm;
int flush;
{
    struct inflate_state *state;
    z_const unsigned char *next;    /* next input */
    unsigned char *put;     /* next output */
    unsigned have, left;        /* available input and output */
    unsigned long hold;         /* bit buffer */
    unsigned bits;              /* bits in bit buffer */
    unsigned in, out;           /* save starting available input and output */
    unsigned copy;              /* number of stored or match bytes to copy */
    unsigned char *from;    /* where to copy match bytes from */
    code here;                  /* current decoding table entry */
    code last;                  /* parent table entry */
    unsigned len;               /* length to copy for repeats, bits to drop */
    int ret;                    /* return code */
    static const unsigned short order[19] = /* permutation of code lengths */
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    if (strm == Z_NULL || strm->state == Z_NULL || strm->next_out == Z_NULL ||
        (strm->next_in == Z_NULL && strm->avail_in != 0))
        return Z_STREAM_ERROR;

    state = (struct inflate_state *)strm->state;
    if (state->mode == TYPE) state->mode = TYPEDO;      /* skip check */
    LOAD();
    in = have;
    out = left;
    ret = Z_OK;
    for (;;)
        switch (state->mode) {
        case HEAD:
            if (state->wrap == 0) {
                state->mode = TYPEDO;
                break;
            }
            NEEDBITS(16);
            if (
                ((BITS(8) << 8) + (hold >> 8)) % 31) {
                strm->msg = (char *)"incorrect header check";
                state->mode = BAD;
                break;
            }
            if (BITS(4) != Z_DEFLATED) {
                strm->msg = (char *)"unknown compression method";
                state->mode = BAD;
                break;
            }
            DROPBITS(4);
            len = BITS(4) + 8;
            if (state->wbits == 0)
                state->wbits = len;
            else if (len > state->wbits) {
                strm->msg = (char *)"invalid window size";
                state->mode = BAD;
                break;
            }
            state->dmax = 1U << len;
            strm->adler = state->check = adler32(0L, Z_NULL, 0);
            state->mode = hold & 0x200 ? DICTID : TYPE;
            INITBITS();
            break;
        case DICTID:
            NEEDBITS(32);
            strm->adler = state->check = ZSWAP32(hold);
            INITBITS();
            state->mode = DICT;
        case DICT:
            if (state->havedict == 0) {
                RESTORE();
                return Z_NEED_DICT;
            }
            strm->adler = state->check = adler32(0L, Z_NULL, 0);
            state->mode = TYPE;
        case TYPE:
            if (flush == Z_BLOCK || flush == Z_TREES) goto inf_leave;
        case TYPEDO:
            if (state->last) {
                BYTEBITS();
                state->mode = CHECK;
                break;
            }
            NEEDBITS(3);
            state->last = BITS(1);
            DROPBITS(1);
            switch (BITS(2)) {
            case 0:                             /* stored block */
                state->mode = STORED;
                break;
            case 1:                             /* fixed block */
                fixedtables(state);
                state->mode = LEN_;             /* decode codes */
                if (flush == Z_TREES) {
                    DROPBITS(2);
                    goto inf_leave;
                }
                break;
            case 2:                             /* dynamic block */
                state->mode = TABLE;
                break;
            case 3:
                strm->msg = (char *)"invalid block type";
                state->mode = BAD;
            }
            DROPBITS(2);
            break;
        case STORED:
            BYTEBITS();                         /* go to byte boundary */
            NEEDBITS(32);
            if ((hold & 0xffff) != ((hold >> 16) ^ 0xffff)) {
                strm->msg = (char *)"invalid stored block lengths";
                state->mode = BAD;
                break;
            }
            state->length = (unsigned)hold & 0xffff;
            INITBITS();
            state->mode = COPY_;
            if (flush == Z_TREES) goto inf_leave;
        case COPY_:
            state->mode = COPY;
        case COPY:
            copy = state->length;
            if (copy) {
                if (copy > have) copy = have;
                if (copy > left) copy = left;
                if (copy == 0) goto inf_leave;
                zmemcpy(put, next, copy);
                have -= copy;
                next += copy;
                left -= copy;
                put += copy;
                state->length -= copy;
                break;
            }
            state->mode = TYPE;
            break;
        case TABLE:
            NEEDBITS(14);
            state->nlen = BITS(5) + 257;
            DROPBITS(5);
            state->ndist = BITS(5) + 1;
            DROPBITS(5);
            state->ncode = BITS(4) + 4;
            DROPBITS(4);
#ifndef PKZIP_BUG_WORKAROUND
            if (state->nlen > 286 || state->ndist > 30) {
                strm->msg = (char *)"too many length or distance symbols";
                state->mode = BAD;
                break;
            }
#endif
            state->have = 0;
            state->mode = LENLENS;
        case LENLENS:
            while (state->have < state->ncode) {
                NEEDBITS(3);
                state->lens[order[state->have++]] = (unsigned short)BITS(3);
                DROPBITS(3);
            }
            while (state->have < 19)
                state->lens[order[state->have++]] = 0;
            state->next = state->codes;
            state->lencode = (const code *)(state->next);
            state->lenbits = 7;
            ret = inflate_table(CODES, state->lens, 19, &(state->next),
                                &(state->lenbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid code lengths set";
                state->mode = BAD;
                break;
            }
            state->have = 0;
            state->mode = CODELENS;
        case CODELENS:
            while (state->have < state->nlen + state->ndist) {
                for (;;) {
                    here = state->lencode[BITS(state->lenbits)];
                    if ((unsigned)(here.bits) <= bits) break;
                    PULLBYTE();
                }
                if (here.val < 16) {
                    DROPBITS(here.bits);
                    state->lens[state->have++] = here.val;
                }
                else {
                    if (here.val == 16) {
                        NEEDBITS(here.bits + 2);
                        DROPBITS(here.bits);
                        if (state->have == 0) {
                            strm->msg = (char *)"invalid bit length repeat";
                            state->mode = BAD;
                            break;
                        }
                        len = state->lens[state->have - 1];
                        copy = 3 + BITS(2);
                        DROPBITS(2);
                    }
                    else if (here.val == 17) {
                        NEEDBITS(here.bits + 3);
                        DROPBITS(here.bits);
                        len = 0;
                        copy = 3 + BITS(3);
                        DROPBITS(3);
                    }
                    else {
                        NEEDBITS(here.bits + 7);
                        DROPBITS(here.bits);
                        len = 0;
                        copy = 11 + BITS(7);
                        DROPBITS(7);
                    }
                    if (state->have + copy > state->nlen + state->ndist) {
                        strm->msg = (char *)"invalid bit length repeat";
                        state->mode = BAD;
                        break;
                    }
                    while (copy--)
                        state->lens[state->have++] = (unsigned short)len;
                }
            }

            /* handle error breaks in while */
            if (state->mode == BAD) break;

            /* check for end-of-block code (better have one) */
            if (state->lens[256] == 0) {
                strm->msg = (char *)"invalid code -- missing end-of-block";
                state->mode = BAD;
                break;
            }

            state->next = state->codes;
            state->lencode = (const code *)(state->next);
            state->lenbits = 9;
            ret = inflate_table(LENS, state->lens, state->nlen, &(state->next),
                                &(state->lenbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid literal/lengths set";
                state->mode = BAD;
                break;
            }
            state->distcode = (const code *)(state->next);
            state->distbits = 6;
            ret = inflate_table(DISTS, state->lens + state->nlen, state->ndist,
                            &(state->next), &(state->distbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid distances set";
                state->mode = BAD;
                break;
            }
            state->mode = LEN_;
            if (flush == Z_TREES) goto inf_leave;
        case LEN_:
            state->mode = LEN;
        case LEN:
            if (have >= 6 && left >= 258) {
                RESTORE();
                inflate_fast(strm, out);
                LOAD();
                if (state->mode == TYPE)
                    state->back = -1;
                break;
            }
            state->back = 0;
            for (;;) {
                here = state->lencode[BITS(state->lenbits)];
                if ((unsigned)(here.bits) <= bits) break;
                PULLBYTE();
            }
            if (here.op && (here.op & 0xf0) == 0) {
                last = here;
                for (;;) {
                    here = state->lencode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + here.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
                state->back += last.bits;
            }
            DROPBITS(here.bits);
            state->back += here.bits;
            state->length = (unsigned)here.val;
            if ((int)(here.op) == 0) {
                state->mode = LIT;
                break;
            }
            if (here.op & 32) {
                state->back = -1;
                state->mode = TYPE;
                break;
            }
            if (here.op & 64) {
                strm->msg = (char *)"invalid literal/length code";
                state->mode = BAD;
                break;
            }
            state->extra = (unsigned)(here.op) & 15;
            state->mode = LENEXT;
        case LENEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->length += BITS(state->extra);
                DROPBITS(state->extra);
                state->back += state->extra;
            }
            state->was = state->length;
            state->mode = DIST;
        case DIST:
            for (;;) {
                here = state->distcode[BITS(state->distbits)];
                if ((unsigned)(here.bits) <= bits) break;
                PULLBYTE();
            }
            if ((here.op & 0xf0) == 0) {
                last = here;
                for (;;) {
                    here = state->distcode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + here.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
                state->back += last.bits;
            }
            DROPBITS(here.bits);
            state->back += here.bits;
            if (here.op & 64) {
                strm->msg = (char *)"invalid distance code";
                state->mode = BAD;
                break;
            }
            state->offset = (unsigned)here.val;
            state->extra = (unsigned)(here.op) & 15;
            state->mode = DISTEXT;
        case DISTEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->offset += BITS(state->extra);
                DROPBITS(state->extra);
                state->back += state->extra;
            }
            state->mode = MATCH;
        case MATCH:
            if (left == 0) goto inf_leave;
            copy = out - left;
            if (state->offset > copy) {         /* copy from window */
                copy = state->offset - copy;
                if (copy > state->whave) {
                    if (state->sane) {
                        strm->msg = (char *)"invalid distance too far back";
                        state->mode = BAD;
                        break;
                    }
                }
                if (copy > state->wnext) {
                    copy -= state->wnext;
                    from = state->window + (state->wsize - copy);
                }
                else
                    from = state->window + (state->wnext - copy);
                if (copy > state->length) copy = state->length;
            }
            else {                              /* copy from output */
                from = put - state->offset;
                copy = state->length;
            }
            if (copy > left) copy = left;
            left -= copy;
            state->length -= copy;
            do {
                *put++ = *from++;
            } while (--copy);
            if (state->length == 0) state->mode = LEN;
            break;
        case LIT:
            if (left == 0) goto inf_leave;
            *put++ = (unsigned char)(state->length);
            left--;
            state->mode = LEN;
            break;
        case CHECK:
            if (state->wrap) {
                NEEDBITS(32);
                out -= left;
                strm->total_out += out;
                state->total += out;
                if (out)
                    strm->adler = state->check =
                        UPDATE(state->check, put - out, out);
                out = left;
                if ((
                     ZSWAP32(hold)) != state->check) {
                    strm->msg = (char *)"incorrect data check";
                    state->mode = BAD;
                    break;
                }
                INITBITS();
            }
            state->mode = DONE;
        case DONE:
            ret = Z_STREAM_END;
            goto inf_leave;
        case BAD:
            ret = Z_DATA_ERROR;
            goto inf_leave;
        case MEM:
            return Z_MEM_ERROR;
        case SYNC:
        default:
            return Z_STREAM_ERROR;
        }
  inf_leave:
    RESTORE();
    if (state->wsize || (out != strm->avail_out && state->mode < BAD &&
            (state->mode < CHECK || flush != Z_FINISH)))
        if (updatewindow(strm, strm->next_out, out - strm->avail_out)) {
            state->mode = MEM;
            return Z_MEM_ERROR;
        }
    in -= strm->avail_in;
    out -= strm->avail_out;
    strm->total_in += in;
    strm->total_out += out;
    state->total += out;
    if (state->wrap && out)
        strm->adler = state->check =
            UPDATE(state->check, strm->next_out - out, out);
    strm->data_type = state->bits + (state->last ? 64 : 0) +
                      (state->mode == TYPE ? 128 : 0) +
                      (state->mode == LEN_ || state->mode == COPY_ ? 256 : 0);
    if (((in == 0 && out == 0) || flush == Z_FINISH) && ret == Z_OK)
        ret = Z_BUF_ERROR;
    return ret;
}

int ZEXPORT inflateEnd(strm)
z_streamp strm;
{
    struct inflate_state *state;
    if (strm == Z_NULL || strm->state == Z_NULL || strm->zfree == (free_func)0)
        return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    if (state->window != Z_NULL) ZFREE(strm, state->window);
    ZFREE(strm, strm->state);
    strm->state = Z_NULL;
    return Z_OK;
}

