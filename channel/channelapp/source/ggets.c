/* File ggets.c  - goodgets is a safe alternative to gets */
/* By C.B. Falconer.  Public domain 2002-06-22            */
/*    attribution appreciated.                            */

/* Revised 2002-06-26  New prototype.
           2002-06-27  Incomplete final lines
           2006-06-14  Simplified, using getc, not fgets
           2006-06-15  Fixed memory leak at EOF
 */

/* fggets and ggets [which is fggets(ln, stdin)] set *ln to
   a buffer filled with the next complete line from the text
   stream f.  The storage has been allocated within fggets,
   and is normally reduced to be an exact fit.  The trailing
   \n has been removed, so the resultant line is ready for
   dumping with puts.  The buffer will be as large as is
   required to hold the complete line.

   Note: this means a final file line without a \n terminator
   has an effective \n appended, as EOF occurs within the read.

   If no error occurs fggets returns 0.  If an EOF occurs on
   the input file, EOF is returned.  For memory allocation
   errors some positive value is returned.  In this case *ln
   may point to a partial line.  For other errors memory is
   freed and *ln is set to NULL.

   Freeing of assigned storage is the callers responsibility
 */

#include <stdio.h>
#include <stdlib.h>
#include "ggets.h"
#include "panic.h"

#define INITSIZE 112 /* power of 2 minus 16, helps malloc */
#define DELTASIZE (INITSIZE + 16)

enum { OK = 0, NOMEM };

int fggets(char **ln, FILE *f) {
    int cursize, ch, ix;
    char *buffer;

    *ln = NULL; /* default */
    buffer = pmalloc(INITSIZE);
    cursize = INITSIZE;

    ix = 0;
    while ((EOF != (ch = getc(f))) && ('\n' != ch)) {
        if (ix >= (cursize - 1)) { /* extend buffer */
            cursize += DELTASIZE;
            buffer = prealloc(buffer, (size_t)cursize);
        }
        buffer[ix++] = ch;
    }
    if ((EOF == ch) && (0 == ix)) {
        free(buffer);
        return EOF;
    }

    buffer[ix] = '\0';
    *ln = prealloc(buffer, (size_t)ix + 1);
    return OK;
} /* fggets */
/* End of ggets.c */
