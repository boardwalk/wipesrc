#!/usr/bin/env python3
import argparse
import glob
from construct import *
from aligned_before import AlignedBefore

Int16 = AlignedBefore(2, Int16sb)
Int32 = AlignedBefore(4, Int32sb)
Int64 = AlignedBefore(8, Int64sb)

Vector = Struct(
    'vx' / Int32,
    'vy' / Int32,
    'vz' / Int32,
)

def TrackSection(ptr, max_alignment):
    return Aligned(max_alignment, Struct(
        'junction' / ptr,
        'prevSection' / ptr,
        'nextSection' / ptr,
        'centre' / Vector,
        'version' / Int16,
        'pad' / Int16,
        'objects' / ptr,
        'objectCount' / Int16,
        'northViewSection' / Array(3, ptr),
        'southViewSection' / Array(3, ptr),
        'eastViewSection' / Array(3, ptr),
        'westViewSection' / Array(3, ptr),
        'allViewSection' / Array(3, ptr),
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

TrackSectionList32 = GreedyRange(TrackSection(Int32, 4))
TrackSectionList64 = GreedyRange(TrackSection(Int64, 8))

def reformat_trs():
    for file_in in sorted(glob.glob('WIPEOUT/TRACK*/TRACK.TRS')):
        file_out = file_in + '64'

        with open(file_in, 'rb') as f:
            data_in = f.read()

        track_sections = TrackSectionList32.parse(data_in)
        data_out = TrackSectionList64.build(track_sections)

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
