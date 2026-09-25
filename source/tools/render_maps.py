#!/usr/bin/env python3
#
# xrick/source/tools/render_maps.py
#
# Render the four worlds of xrick as map images, from the data of the
# data extractor (Atari ST graphics).
#
# Each world is put together from its submaps: submaps are vertical strips
# of 0x20 tiles, joined left and right by their connections, at the height
# where rick leaves one and enters the next. Entities (enemies, boxes,
# bonuses, visible traps) are drawn at their start position, and rick at
# the start of the world.
#
# The castle (world 3) is a maze: some connections lead back to earlier
# submaps, or to the same one, so it can not be laid out as one area.
# Connections that do not match the layout are written as red arrows with
# the number of the submap they lead to.
#
# Usage, from the root of the source tree (needs Pillow):
#
#   python3 source/tools/render_maps.py [output directory]
#
# The default output directory is assets/docs/maps.
#

import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
SRC = os.path.join(ROOT, 'source', 'data_extractor')
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'assets', 'docs', 'maps')

ZOOM = 2        # size of the images: 2 times the game's pixels
MARGIN = 60     # room for the title, in game pixels
NAMES = ['World 1: South America 1945', 'World 2: Egypt',
         'World 3: Europe, the castle', 'World 4: Europe, the missile base']


#
# Reading the data of the data extractor
#

def number(v):
    """C number: hex (0x..) or octal (0...)"""
    return int(v, 16) if v.lower().startswith('0x') else int(v, 8)


def array(text, name):
    """All numbers of the C array initializer that follows name"""
    x = text[text.index(name):]
    x = x[x.index('{') + 1:]
    depth, i = 1, 0
    while depth:
        if x[i] == '{':
            depth += 1
        elif x[i] == '}':
            depth -= 1
        i += 1
    x = x[:i - 1]
    x = re.sub(r'/\*.*?\*/', '', x, flags=re.S)             # comments
    x = re.sub(r'#ifdef ENABLE_SOUND.*?#endif', '', x, flags=re.S)
    x = re.sub(r'#\w+.*', '', x)                             # other directives
    return [number(v) for v in re.findall(r'0[xX][0-9a-fA-F]+|\d+', x)]


def chunks(values, n):
    return [values[i:i + n] for i in range(0, len(values), n)]


def read(name):
    with open(os.path.join(SRC, name)) as f:
        return f.read()


maps_c = read('dat_maps.c')
ents_c = read('dat_ents.c')
tiles_c = read('dat_tilesST.c')
sprites_c = read('dat_spritesST.c')
tiles_c = tiles_c[tiles_c.index('#ifdef GFXST'):]
if '#ifdef GFXST' in sprites_c:
    sprites_c = sprites_c[sprites_c.index('#ifdef GFXST'):]

MAPS = chunks(array(maps_c, 'MAP_MAPS[MAP_NBR_MAPS] ='), 4)          # x, y, row, submap
SUBMAPS = chunks(array(maps_c, 'MAP_SUBMAPS[MAP_NBR_SUBMAPS] ='), 4)  # page, bnum, connect, mark
CONNECT = chunks(array(maps_c, 'MAP_CONNECT[MAP_NBR_CONNECT] ='), 4)  # dir, rowout, submap, rowin
BNUMS = array(maps_c, 'MAP_BNUMS[MAP_NBR_BNUMS] =')
BLOCKS = chunks(array(maps_c, 'MAP_BLOCKS[MAP_NBR_BLOCKS] ='), 16)    # 4x4 tiles
MARKS = chunks(array(maps_c, 'MAP_MARKS[MAP_NBR_MARKS] ='), 5)        # row, ent, flags, xy, lt
ENTDATA = chunks(array(ents_c, 'ENT_ENTDATA[ENT_NBR_ENTDATA] ='), 7)  # w, h, spr, sni, ...
SPRSEQ = array(ents_c, 'ENT_SPRSEQ[ENT_NBR_SPRSEQ] =')
TILES = chunks(array(tiles_c, 'TILES_DATA[TILES_NBR_BANKS][TILES_NBR_TILES] ='), 8)
SPRITES = chunks(array(sprites_c, 'SPRITES_DATA[SPRITES_NBR_SPRITES] ='), 84)
PALETTE = [tuple(map(int, m)) for m in
           re.findall(r'\{(\d+),(\d+),(\d+),\d+\}', read('dat_paletteST.e'))]

RIGHT = 0           # connect_t.dir; LEFT is 1
END = 0xff          # end of the connections or marks of a submap
FLG_STOPRICK = 0x02  # ENT_FLG_STOPRICK: entity on slot 0, not moved down


#
# Layout
#

def submap_rows(s):
    """Height of a submap in tile rows, without empty block rows at its end"""
    b = SUBMAPS[s][1]
    after = [SUBMAPS[t][1] for t in range(len(SUBMAPS)) if SUBMAPS[t][1] > b]
    rows = ((min(after) if after else len(BNUMS)) - b) // 8
    while rows > 0 and not any(BNUMS[b + (rows - 1) * 8:b + rows * 8]):
        rows -= 1
    return rows * 4


def connections(s):
    """(dir, rowout, submap, rowin) of a submap"""
    c, out = SUBMAPS[s][2], []
    while CONNECT[c][0] != END:
        out.append(tuple(CONNECT[c]))
        c += 1
    return out


def layout(start):
    """Position (x in pixels, top in tile rows) of the submaps of a world"""
    pos, todo = {start: (0, 0)}, [start]
    while todo:
        s = todo.pop(0)
        x, top = pos[s]
        for d, rowout, t, rowin in connections(s):
            if t != END and t not in pos:
                pos[t] = (x + (256 if d == RIGHT else -256), top + rowout - rowin)
                todo.append(t)
    return pos


#
# Drawing
#

tile_cache = {}


def tile(bank, n):
    """8x8 tile: one U32 per row, 8 pixels of 4 bits"""
    if (bank, n) not in tile_cache:
        im = Image.new('RGB', (8, 8))
        for y, row in enumerate(TILES[bank * 256 + n]):
            for x in range(8):
                im.putpixel((x, y), PALETTE[(row >> (4 * (7 - x))) & 15])
        tile_cache[(bank, n)] = im
    return tile_cache[(bank, n)]


def sprite(n):
    """32x21 sprite: 4 U32 per row, color 0 is transparent"""
    im = Image.new('RGBA', (32, 21), (0, 0, 0, 0))
    for r in range(21):
        for c in range(4):
            v = SPRITES[n][r * 4 + c]
            for x in range(8):
                p = (v >> (4 * (7 - x))) & 15
                if p:
                    im.putpixel((c * 8 + x, r), PALETTE[p] + (255,))
    return im


def entity_sprite(ent, flags):
    """Sprite an entity shows when created (see ent_actvis), 0 if none"""
    spr, sni = ENTDATA[ent][2], ENTDATA[ent][3]
    if ent >= 0x18:                         # type 3: sprite sequence
        return SPRSEQ[spr]
    if ent < 0x10 and (flags & 0xf0) == 0xf0:
        return sni & 0xff
    return spr


def font(size):
    for name in ('DejaVuSans-Bold.ttf', '/usr/share/fonts/TTF/DejaVuSans-Bold.ttf',
                 '/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf'):
        try:
            return ImageFont.truetype(name, size)
        except OSError:
            pass
    return ImageFont.load_default(size=size)


LABEL = font(30)
TITLE = font(64)


def text(draw, xy, s, f, fill):
    """Text with a black outline, readable on any background"""
    x, y = xy
    for dx in (-2, 0, 2):
        for dy in (-2, 0, 2):
            draw.text((x + dx, y + dy), s, font=f, fill=(0, 0, 0))
    draw.text(xy, s, font=f, fill=fill)


def render(world):
    start_x, start_y, start_row, start_sub = MAPS[world]
    pos = layout(start_sub)
    minx = min(x for x, _ in pos.values())
    width = max(x for x, _ in pos.values()) + 256 - minx
    mintop = min(top for _, top in pos.values())
    height = (max(top + submap_rows(s) for s, (_, top) in pos.items()) - mintop) * 8 + MARGIN

    def origin(s):
        """Top left of a submap in the image, in game pixels"""
        x, top = pos[s]
        return x - minx, (top - mintop) * 8 + MARGIN

    img = Image.new('RGB', (width, height), (25, 25, 25))
    for s in pos:
        ox, oy = origin(s)
        page, bnum = SUBMAPS[s][0], SUBMAPS[s][1]
        for r in range(submap_rows(s) // 4):        # rows of 8 blocks
            for c in range(8):
                block = BLOCKS[BNUMS[bnum + r * 8 + c]]
                for i in range(16):
                    img.paste(tile(1 + page, block[i]),
                              (ox + c * 32 + (i % 4) * 8, oy + r * 32 + (i // 4) * 8))
        m = SUBMAPS[s][3]
        while MARKS[m][0] != END:
            row, ent, flags, xy, _ = MARKS[m]
            m += 1
            n = entity_sprite(ent, flags)
            tile_row = (row & 0xf8) + (xy & 7)
            if n == 0 or n >= len(SPRITES) or tile_row >= submap_rows(s):
                continue    # invisible, or in rows the game never shows
            im = sprite(n)
            y = tile_row * 8 + (0 if flags & FLG_STOPRICK else 3)
            img.paste(im, (ox + (xy & 0xf8), oy + y), im)
    ox, oy = origin(start_sub)
    rick = sprite(1)
    img.paste(rick, (ox + start_x, oy + start_row * 8 + start_y), rick)

    img = img.resize((width * ZOOM, height * ZOOM), Image.NEAREST)
    draw = ImageDraw.Draw(img)
    for s, (x, top) in pos.items():
        ox, oy = (v * ZOOM for v in origin(s))
        draw.rectangle([ox, oy, ox + 256 * ZOOM - 1, oy + submap_rows(s) * 8 * ZOOM - 1],
                       outline=(90, 90, 90), width=2)
        text(draw, (ox + 12, oy + 8), 'Submap %d' % (s + 1), LABEL, (255, 255, 0))
        for d, rowout, t, rowin in connections(s):
            y = oy + rowout * 8 * ZOOM
            if t == END:
                if d == RIGHT:
                    text(draw, (ox + 256 * ZOOM - 180, y), 'Exit →', LABEL, (120, 255, 120))
                continue
            if pos.get(t) != (x + (256 if d == RIGHT else -256), top + rowout - rowin):
                if d == RIGHT:
                    text(draw, (ox + 256 * ZOOM - 80, y), '→ %d' % (t + 1), LABEL, (255, 140, 140))
                else:
                    text(draw, (ox + 10, y), '← %d' % (t + 1), LABEL, (255, 140, 140))
    ox, oy = origin(start_sub)
    text(draw, ((ox + start_x) * ZOOM - 10, (oy + start_row * 8 + start_y) * ZOOM - 40),
         'Start', LABEL, (120, 255, 120))
    text(draw, (20, 20), NAMES[world], TITLE, (255, 255, 255))
    return img


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    for world in range(4):
        name = os.path.join(OUT, 'world%d.png' % (world + 1))
        render(world).save(name, optimize=True)
        print(name)
