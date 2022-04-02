#!/usr/bin/env python3
import argparse
import glob
from aligned_before import AlignedBefore
from construct import *

def endian_char(endian):
    if endian == 'l':
        return '<'
    elif endian == 'b':
        return '>'
    else:
        raise ValueError('Invalid endian value')

def format_char(size):
    if size == 2:
        return 'h'
    elif size == 4:
        return 'l'
    elif size == 8:
        return 'q'
    else:
        raise ValueError('Invalid size value')

def TrackSection(ptr_size, endian):
    Int16 = AlignedBefore(2, FormatField(endian_char(endian), format_char(2)))
    Int32 = AlignedBefore(4, FormatField(endian_char(endian), format_char(4)))
    IntPtr = AlignedBefore(ptr_size, FormatField(endian_char(endian), format_char(ptr_size)))

    Vector = Struct(
        'vx' / Int32,
        'vy' / Int32,
        'vz' / Int32,
    )

    return Aligned(ptr_size, Struct(
        'junction' / IntPtr,
        'prevSection' / IntPtr,
        'nextSection' / IntPtr,
        'centre' / Vector,
        'version' / Int16,
        'pad' / Int16,
        'objects' / IntPtr,
        'objectCount' / Int16,
        'northViewSection' / Array(3, IntPtr),
        'southViewSection' / Array(3, IntPtr),
        'eastViewSection' / Array(3, IntPtr),
        'westViewSection' / Array(3, IntPtr),
        'allViewSection' / Array(3, IntPtr),
        'northViewCount' / Array(3, Int16),
        'southViewCount' / Array(3, Int16),
        'eastViewCount' / Array(3, Int16),
        'westViewCount' / Array(3, Int16),
        'allViewCount' / Array(3, Int16),
        'high' / Array(4, Int16),
        'med' / Array(4, Int16),
        'faceStart' / Int16,
        'faceCount' / Int16,
        'globalRadius' / Int16,
        'localRadius' / Int16,
        'flags' / Int16,
        'secNo' / Int16,
    ))

TrackSectionListOld = GreedyRange(TrackSection(4, 'b'))
TrackSectionListNew = GreedyRange(TrackSection(8, 'l'))

#    char              name[16];
#    int16_t             vertexCount;      /* Number of Vertices */
#    SmallPtr<SVECTOR>          vertices;         /* Pointer to 3D Points */
#    int16_t             normalCount;      /* Number of Normals */
#    SmallPtr<SVECTOR>          normals;          /* Pointer to 3D Normals */
#    int16_t             primitiveCount;   /* Number of Primitives */
#    SmallPtr<Primitive>        primitives;       /* Pointer to Z Sort Primitives */
#    int32_t              pad;
#    SmallPtr<BspTree>          bspTree;          /* Pointer to BSP Tree Primitives */
#    SmallPtr<Skeleton>         skeleton;         /* Pointer to Local Coordinates */
#    int32_t              extent;           /* Flags for object characteristics */
#    int16_t             flags;            /* Next object in list */
#    SmallPtr<struct Object>    next;             /* Next object in list */

# def Object(ptr, max_alignment):
#     return Aligned(max_alignment, Struct(
#         'name' / PaddedString(16, 'utf8'),
#         'vertexCount' / Int16,
#         'vertices' / ptr,
#         'normalCount' / Int16,
#         'normals' / ptr,
#         'primitiveCount' / Int16,
#         'primitives' / ptr,
#         'pad' / Int32,
#         'bspTree' / ptr,
#         'skeleton' / ptr,
#         'extent' / Int32,
#         'flags' / Int16,
#         'next' / ptr,
#     ))

# Matrix = Struct(
#     'm' / Array(9, Int16),
#     't' / Array(3, Int16),
# )

# def Skeleton(ptr, max_alignment):
#     return Aligned(max_alignment, Struct(
#         'relative' / Matrix,
#         'absolute' / Matrix,
#         'update' / Int16,
#         'super' / ptr,
#         'sub' / ptr,
#         'next' / ptr,
#     ));

# SVector = Struct(
#     'vx' / Int16,
#     'vy' / Int16,
#     'vz' / Int16,
#     'pad' / Int16,
# )

# Primitive = Struct(
#     'type' / Int16,
#     'value' / Switch(this.n,
#         '1'
# )

def reformat_trs():
    for file_in in sorted(glob.glob('WIPEOUT/TRACK*/TRACK.TRS')):
        file_out = file_in + '2'

        with open(file_in, 'rb') as f:
            data_in = f.read()

        track_sections = TrackSectionListOld.parse(data_in)
        data_out = TrackSectionListNew.build(track_sections)

        with open(file_out, 'wb') as f:
            f.write(data_out)

        print(f'{file_in} {file_out} {len(data_in)} {len(data_out)} {len(track_sections)}')

def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    reformat_trs_parser = subparsers.add_parser('reformat_trs')
    reformat_trs_parser.set_defaults(func=reformat_trs)

    args = parser.parse_args()
    args.func()

if __name__ == '__main__':
    main()
