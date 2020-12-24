/*

** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/

#include "simple_highlight.h"
#include <math.h> /* amalgamator: keep */
#include <string.h>
#include <stdio.h>
SQLITE_EXTENSION_INIT3

/*
** Object used to iterate through all "coalesced phrase instances" in
** a single column of the current row. If the phrase instances in the
** column being considered do not overlap, this object simply iterates
** through them. Or, if they do overlap (share one or more tokens in
** common), each set of overlapping instances is treated as a single
** match. See documentation for the highlight() auxiliary function for
** details.
**
** Usage is:
**
**   for(rc = fts5CInstIterNext(pApi, pFts, iCol, &iter);
**      (rc==SQLITE_OK && 0==fts5CInstIterEof(&iter);
**      rc = fts5CInstIterNext(&iter)
**   ){
**     printf("instance starts at %d, ends at %d\n", iter.iStart, iter.iEnd);
**   }
**
*/
typedef struct CInstIter CInstIter;
struct CInstIter {
  const Fts5ExtensionApi *pApi; /* API offered by current FTS version */
  Fts5Context *pFts;            /* First arg to pass to pApi functions */
  int iCol;                     /* Column to search */
  int iInst;                    /* Next phrase instance index */
  int nInst;                    /* Total number of phrase instances */

  /* Output variables */
  int iStart; /* First token in coalesced phrase instance */
  int iEnd;   /* Last token in coalesced phrase instance */
};

/*
** Advance the iterator to the next coalesced phrase instance. Return
** an SQLite error code if an error occurs, or SQLITE_OK otherwise.
*/
static int fts5CInstIterNext(CInstIter *pIter) {
  int rc = SQLITE_OK;
  pIter->iStart = -1;
  pIter->iEnd = -1;

  while (rc == SQLITE_OK && pIter->iInst < pIter->nInst) {
    int ip;
    int ic;
    int io;
    rc = pIter->pApi->xInst(pIter->pFts, pIter->iInst, &ip, &ic, &io);
    if (rc == SQLITE_OK) {
      if (ic == pIter->iCol) {
        int iEnd = io - 1 + pIter->pApi->xPhraseSize(pIter->pFts, ip);
        if (pIter->iStart < 0) {
          pIter->iStart = io;
          pIter->iEnd = iEnd;
        } else if (io <= pIter->iEnd + 1) {  // NOTE: +1 is the only diff with buildin highlight function
          if (iEnd > pIter->iEnd) pIter->iEnd = iEnd;
        } else {
          break;
        }
      }
      pIter->iInst++;
    }
  }

  return rc;
}

/*
** Initialize the iterator object indicated by the final parameter to
** iterate through coalesced phrase instances in column iCol.
*/
static int fts5CInstIterInit(const Fts5ExtensionApi *pApi, Fts5Context *pFts, int iCol, CInstIter *pIter) {
  int rc;

  memset(pIter, 0, sizeof(CInstIter));
  pIter->pApi = pApi;
  pIter->pFts = pFts;
  pIter->iCol = iCol;
  rc = pApi->xInstCount(pFts, &pIter->nInst);

  if (rc == SQLITE_OK) {
    rc = fts5CInstIterNext(pIter);
  }

  return rc;
}

/*************************************************************************
** Start of highlight() implementation.
*/
typedef struct HighlightContext HighlightContext;
struct HighlightContext {
  CInstIter iter;     /* Coalesced Instance Iterator */
  int iPos;           /* Current token offset in zIn[] */
  int iRangeStart;    /* First token to include */
  int iRangeEnd;      /* If non-zero, last token to include */
  const char *zOpen;  /* Opening highlight */
  const char *zClose; /* Closing highlight */
  const char *zIn;    /* Input text */
  int nIn;            /* Size of input text in bytes */
  int iOff;           /* Current offset within zIn[] */
  char *zOut;         /* Output value */
};

/*
** Append text to the HighlightContext output string - p->zOut. Argument
** z points to a buffer containing n bytes of text to append. If n is
** negative, everything up until the first '\0' is appended to the output.
**
** If *pRc is set to any value other than SQLITE_OK when this function is
** called, it is a no-op. If an error (i.e. an OOM condition) is encountered,
** *pRc is set to an error code before returning.
*/
static void fts5HighlightAppend(int *pRc, HighlightContext *p, const char *z, int n) {
  if (*pRc == SQLITE_OK && z) {
    if (n < 0) n = (int)strlen(z);
    p->zOut = sqlite3_mprintf("%z%.*s", p->zOut, n, z);
    if (p->zOut == 0) *pRc = SQLITE_NOMEM;
  }
}

/*
** Tokenizer callback used by implementation of highlight() function.
*/
static int fts5HighlightCb(void *pContext,     /* Pointer to HighlightContext object */
                           int tflags,         /* Mask of FTS5_TOKEN_* flags */
                           const char *pToken, /* Buffer containing token */
                           int nToken,         /* Size of token in bytes */
                           int iStartOff,      /* Start offset of token */
                           int iEndOff         /* End offset of token */
) {
  HighlightContext *p = (HighlightContext *)pContext;
  int rc = SQLITE_OK;
  int iPos;

  if (tflags & FTS5_TOKEN_COLOCATED) return SQLITE_OK;
  iPos = p->iPos++;

  if (p->iRangeEnd > 0) {
    if (iPos < p->iRangeStart || iPos > p->iRangeEnd) return SQLITE_OK;
    if (p->iRangeStart && iPos == p->iRangeStart) p->iOff = iStartOff;
  }

  if (iPos == p->iter.iStart) {
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iStartOff - p->iOff);
    fts5HighlightAppend(&rc, p, p->zOpen, -1);
    p->iOff = iStartOff;
  }

  if (iPos == p->iter.iEnd) {
    if (p->iRangeEnd && p->iter.iStart < p->iRangeStart) {
      fts5HighlightAppend(&rc, p, p->zOpen, -1);
    }
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iEndOff - p->iOff);
    fts5HighlightAppend(&rc, p, p->zClose, -1);
    p->iOff = iEndOff;
    if (rc == SQLITE_OK) {
      rc = fts5CInstIterNext(&p->iter);
    }
  }

  if (p->iRangeEnd > 0 && iPos == p->iRangeEnd) {
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iEndOff - p->iOff);
    p->iOff = iEndOff;
    if (iPos >= p->iter.iStart && iPos < p->iter.iEnd) {
      fts5HighlightAppend(&rc, p, p->zClose, -1);
    }
  }

  return rc;
}

/*
** Implementation of simple_highlight() function.
*/
void simple_highlight(const Fts5ExtensionApi *pApi, /* API offered by current FTS version */
                      Fts5Context *pFts,            /* First arg to pass to pApi functions */
                      sqlite3_context *pCtx,        /* Context for returning result/error */
                      int nVal,                     /* Number of values in apVal[] array */
                      sqlite3_value **apVal         /* Array of trailing arguments */
) {
  HighlightContext ctx;
  int rc;
  int iCol;

  if (nVal != 3) {
    const char *zErr = "wrong number of arguments to function highlight()";
    sqlite3_result_error(pCtx, zErr, -1);
    return;
  }

  iCol = sqlite3_value_int(apVal[0]);
  memset(&ctx, 0, sizeof(HighlightContext));
  ctx.zOpen = (const char *)sqlite3_value_text(apVal[1]);
  ctx.zClose = (const char *)sqlite3_value_text(apVal[2]);
  rc = pApi->xColumnText(pFts, iCol, &ctx.zIn, &ctx.nIn);

  if (ctx.zIn) {
    if (rc == SQLITE_OK) {
      rc = fts5CInstIterInit(pApi, pFts, iCol, &ctx.iter);
    }

    if (rc == SQLITE_OK) {
      rc = pApi->xTokenize(pFts, ctx.zIn, ctx.nIn, (void *)&ctx, fts5HighlightCb);
    }
    fts5HighlightAppend(&rc, &ctx, &ctx.zIn[ctx.iOff], ctx.nIn - ctx.iOff);

    if (rc == SQLITE_OK) {
      sqlite3_result_text(pCtx, (const char *)ctx.zOut, -1, SQLITE_TRANSIENT);
    }
    sqlite3_free(ctx.zOut);
  }
  if (rc != SQLITE_OK) {
    sqlite3_result_error_code(pCtx, rc);
  }
}
/*
** End of highlight() implementation.
**************************************************************************/

/*************************************************************************
** Start of highlight_pos() implementation.
*/
typedef struct HighlightPosContext HighlightPosContext;
struct HighlightPosContext {
  CInstIter iter;                 /* Coalesced Instance Iterator */
  int iPos;                       /* Current token offset in zIn[] */
  int iRangeStart;                /* First token to include */
  int iRangeEnd;                  /* If non-zero, last token to include */
  const char *zIn;                /* Input text */
  int nIn;                        /* Size of input text in bytes */
  int iOff;                       /* Current offset within zIn[] */
  char *zOut;                     /* Output value */
};

/*
** Append text to the HighlightPosContext output string - p->zOut. Argument
** z points to a buffer containing n bytes of text to append. If n is
** negative, everything up until the first '\0' is appended to the output.
**
** If *pRc is set to any value other than SQLITE_OK when this function is
** called, it is a no-op. If an error (i.e. an OOM condition) is encountered,
** *pRc is set to an error code before returning.
*/
static void fts5HighlightPosAppend(
  int *pRc,
  HighlightPosContext *p,
  const char *z, int n
){
  if( *pRc==SQLITE_OK ){
    if( n<0 ) n = (int)strlen(z);
    p->zOut = sqlite3_mprintf("%z%.*s", p->zOut, n, z);
    if( p->zOut==0 ) *pRc = SQLITE_NOMEM;
  }
}

static void fts5HighlightPosAppendStart(
  int *pRc,
  HighlightPosContext *p,
  int start
) {
    char str[64];
    sprintf(str, "%d", start);
    fts5HighlightPosAppend(pRc, p, str, -1);
    fts5HighlightPosAppend(pRc, p, ",", -1);
}

static void fts5HighlightPosAppendEnd(
  int *pRc,
  HighlightPosContext *p,
  int end
) {
    char str[64];
    sprintf(str, "%d", end);
    fts5HighlightPosAppend(pRc, p, str, -1);
    fts5HighlightPosAppend(pRc, p, ";", -1);
}

/*
** Implementation of simple_highlight_pos() function.
*/
void simple_highlight_pos(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  sqlite3_context *pCtx,          /* Context for returning result/error */
  int nVal,                       /* Number of values in apVal[] array */
  sqlite3_value **apVal           /* Array of trailing arguments */
){
  HighlightPosContext ctx;
  int rc;
  int iCol;

  if( nVal!=1 ){
    const char *zErr = "wrong number of arguments to function highlight_pos()";
    sqlite3_result_error(pCtx, zErr, -1);
    return;
  }

  iCol = sqlite3_value_int(apVal[0]);
  memset(&ctx, 0, sizeof(HighlightPosContext));
  rc = pApi->xColumnText(pFts, iCol, &ctx.zIn, &ctx.nIn);

  if( ctx.zIn ){
    if( rc==SQLITE_OK ){
      rc = fts5CInstIterInit(pApi, pFts, iCol, &ctx.iter);
    }

    while( rc == SQLITE_OK ){
        if (ctx.iter.iStart >= 0 && ctx.iter.iEnd >= 0) {
            fts5HighlightPosAppendStart(&rc, &ctx, ctx.iter.iStart);
            fts5HighlightPosAppendEnd(&rc, &ctx, ctx.iter.iEnd + 1);
            rc = fts5CInstIterNext(&ctx.iter);
        } else {
            break;
        }
    }

    if( rc==SQLITE_OK ){
      sqlite3_result_text(pCtx, (const char*)ctx.zOut, -1, SQLITE_TRANSIENT);
    }
    sqlite3_free(ctx.zOut);
  }
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
  }
}
/*
** End of highlight_pos() implementation.
**************************************************************************/
