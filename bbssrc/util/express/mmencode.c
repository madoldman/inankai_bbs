/*

a lite version of mmencode, this version ONLY can be base64 
encodeing/decodeing

Copyright (c) 1999 BBS Express Project.

*/
/*
CVS: $Id: mmencode.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/
/*
Copyright (c) 1991 Bell Communications Research, Inc. (Bellcore)

Permission to use, copy, modify, and distribute this material 
for any purpose and without fee is hereby granted, provided 
that the above copyright notice and this permission notice 
appear in all copies, and that the name of Bellcore not be 
used in advertising or publicity pertaining to this 
material without the specific, prior written permission 
of an authorized representative of Bellcore.  BELLCORE 
MAKES NO REPRESENTATIONS ABOUT THE ACCURACY OR SUITABILITY 
OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED "AS IS", 
WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES.
*/
#define MMCODE
#include <stdio.h>
#include <ctype.h>
#define	NEWLINE_CHAR 10

char mmencode_c[] = 
   "$Id: mmencode.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $";
extern char *index();
static char basis_64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char index_64[128] = {
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
    52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
    -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
    -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1
};

#define char64(c)  (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])

/* the following gets a character, but fakes it properly into two chars if there's a newline character */
static int InNewline=0;

int nextcharin(infile, PortableNewlines)
FILE *infile;
int PortableNewlines;
{
    int c;

    if (!PortableNewlines) return(getc(infile));
    if (InNewline) {
        InNewline = 0;
        return(10); /* LF */
    }
    c = getc(infile);
    if (c == NEWLINE_CHAR) {
        InNewline = 1;
        return(13); /* CR */
    }
    return(c);
}

to64(infile, outfile, PortableNewlines) 
FILE *infile, *outfile;
int PortableNewlines;
{
    int c1, c2, c3, ct=0;
    InNewline = 0; /* always reset it */
    while ((c1 = nextcharin(infile, PortableNewlines)) != EOF) {
        c2 = nextcharin(infile, PortableNewlines);
        if (c2 == EOF) {
            output64chunk(c1, 0, 0, 2, outfile);
        } else {
            c3 = nextcharin(infile, PortableNewlines);
            if (c3 == EOF) {
                output64chunk(c1, c2, 0, 1, outfile);
            } else {
                output64chunk(c1, c2, c3, 0, outfile);
            }
        }
        ct += 4;
        if (ct > 71) {
            putc('\n', outfile);
            ct = 0;
        }
    }
    if (ct) putc('\n', outfile);
    fflush(outfile);
}

output64chunk(c1, c2, c3, pads, outfile)
FILE *outfile;
{
    putc(basis_64[c1>>2], outfile);
    putc(basis_64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)], outfile);
    if (pads == 2) {
        putc('=', outfile);
        putc('=', outfile);
    } else if (pads) {
        putc(basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)], outfile);
        putc('=', outfile);
    } else {
        putc(basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)], outfile);
        putc(basis_64[c3 & 0x3F], outfile);
    }
}

PendingBoundary(s, Boundaries, BoundaryCt)
char *s;
char **Boundaries;
int *BoundaryCt;
{
    int i;

    for (i=0; i < *BoundaryCt; ++i) {
        if (!strncmp(s, Boundaries[i], strlen(Boundaries[i]))) {
            char Buf[2000];
            strcpy(Buf, Boundaries[i]);
            strcat(Buf, "--\n");
            if (!strcmp(Buf, s)) *BoundaryCt = i;
            return(1);
        }
    }
    return(0);
}

/* If we're in portable newline mode, we have to convert CRLF to the 
    local newline convention on output */

static int CRpending = 0;

almostputc(c, outfile, PortableNewlines)
int c;
FILE *outfile;
int PortableNewlines;
{
    if (CRpending) {
        if (c == 10) {
            putc(NEWLINE_CHAR, outfile);
        } else {
            putc(13, outfile);
            putc(c, outfile);
        }
        CRpending = 0;
    } else {
        if (PortableNewlines && c == 13) {
            CRpending = 1;
        } else {
            putc(c, outfile);
        }
    }
}

from64(infile, outfile, boundaries, boundaryct, PortableNewlines) 
FILE *infile, *outfile;
char **boundaries;
int *boundaryct;
int PortableNewlines;
{
    int c1, c2, c3, c4;
    int newline = 1, DataDone = 0;

    /* always reinitialize */
    CRpending = 0;
    while ((c1 = getc(infile)) != EOF) {
        if (isspace(c1)) {
            if (c1 == '\n') {
                newline = 1;
            } else {
                newline = 0;
            }
            continue;
        }
        if (newline && boundaries && c1 == '-') {
            char Buf[200];
            /* a dash is NOT base 64, so all bets are off if NOT a boundary */
            ungetc(c1, infile);
            fgets(Buf, sizeof(Buf), infile);
            if (boundaries
                 && (Buf[0] == '-')
                 && (Buf[1] == '-')
                 && PendingBoundary(Buf, boundaries, boundaryct)) {
                return;
            }
            fprintf(stderr, "Ignoring unrecognized boundary line: %s\n", Buf);
            continue;
        }
        if (DataDone) continue;
        newline = 0;
        do {
            c2 = getc(infile);
        } while (c2 != EOF && isspace(c2));
        do {
            c3 = getc(infile);
        } while (c3 != EOF && isspace(c3));
        do {
            c4 = getc(infile);
        } while (c4 != EOF && isspace(c4));
        if (c2 == EOF || c3 == EOF || c4 == EOF) {
            fprintf(stderr, "Premature EOF!\n");
            return;
        }
        if (c1 == '=' || c2 == '=') {
            DataDone=1;
            continue;
        }
        c1 = char64(c1);
        c2 = char64(c2);
        almostputc(((c1<<2) | ((c2&0x30)>>4)), outfile, PortableNewlines);
        if (c3 == '=') {
            DataDone = 1;
        } else {
            c3 = char64(c3);
            almostputc((((c2&0XF) << 4) | ((c3&0x3C) >> 2)), outfile, PortableNewlines);
            if (c4 == '=') {
                DataDone = 1;
            } else {
                c4 = char64(c4);
                almostputc((((c3&0x03) <<6) | c4), outfile, PortableNewlines);
            }
        }
    }
    if (CRpending) putc(13, stdout);
}

#define hexchar(c)  (((c) < 0 || (c) > 127) ? -1 : index_hex[(c)])

int
mm(int encode, char *fin, char *fout)
{
    int portablenewlines = 0;
    FILE *in, *out;
    
    if ( fin != NULL )
       in = fopen(fin, "r");
    else
       in = stdin;
    
    if ( fout != NULL )
       out = fopen(fout, "w");
    else
       out = stdout;
       
    if (encode) 
      to64(in, out, portablenewlines); 
    else 
      from64(in, out, NULL, 0, portablenewlines);
      
    if ( fout != NULL )
       fclose(out);
       
    if ( fin != NULL )
       fclose(in);
       
    return(0);
}

