/*
 * xrick/rects.c
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net).
 * Copyright (C) 2008-2014 Pierluigi Vicinanza.
 * All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

/*
 * Rectangles lists, telling the video which parts of the screen changed
 */

#include "xrick/rects.h"
#include "xrick/system/system.h"

/*
 * Free a list of rectangles.
 *
 * r: rectangle list
 */
void
rects_free(rect_t *r)
{
    while (r)
    {
        rect_t * next = r->next;
        sysmem_pop(r);
        r = next;
    }
}


/*
 * Add a rectangle to a list of rectangles
 *
 * x, y, width, height: the rectangle (pixels, screen)
 * next: the list to add it to, NULL for a new list
 * return: the new list, starting with the rectangle; NULL when out of
 *         memory
 */
rect_t *
rects_new(U16 x, U16 y, U16 width, U16 height, rect_t *next)
{
    rect_t *r;

    r = sysmem_push(sizeof(*r));
    if (!r)
    {
        return NULL;
    }
    r->x = x;
    r->y = y;
    r->width = width;
    r->height = height;
    r->next = next;
    return r;
}

/* eof */
