#!/usr/bin/env python3
import ninja_syntax
import os
import shlex
import subprocess

def get_pkg_config(*args):
    out = subprocess.check_output(['pkg-config'] + list(args))
    return shlex.split(out.strip().decode())

def main():
    os.chdir(os.path.dirname(__file__))
    os.makedirs('build', exist_ok=True)

    glfw_cxxflags = get_pkg_config('--cflags', 'glfw3')
    glfw_linkflags = get_pkg_config('--libs', 'glfw3')

    try:
        with open('build.ninja', 'w') as f:
            ninja = ninja_syntax.Writer(f)
            ninja.rule('cxx', ['clang++', '$cxxflags', '-c', '$in', '-MD', '-MF', '$out.d', '-o', '$out'], depfile='$out.d')
            ninja.rule('link', ['clang++', '$linkflags', '$in', '-o', '$out'])

            ninja.variable('cxxflags', [
                '-g',
                '-O0',
                '-fsanitize=address',
                '-fno-omit-frame-pointer',
                '-I.',
                '-std=c++11',
                '-fcolor-diagnostics',
                '-Wno-constant-conversion',
                '-Wno-empty-body',
                '-Wno-tautological-constant-out-of-range-compare',
                '-Wno-deprecated-declarations',
                '-DWIPEOUTPC',
                '-include stdint.h',
            ] + glfw_cxxflags)

            ninja.variable('linkflags', [
                '-g',
                '-O0',
                '-framework', 'OpenGL',
                '-fsanitize=address',
                '-fno-omit-frame-pointer',
            ] + glfw_linkflags)

            objects = []

            for dirpath, dirnames, filenames in os.walk('src'):
                for filename in filenames:
                    if filename.endswith('.cpp') and not filename.endswith('_old.cpp'):
                        cpp_file = os.path.join(dirpath, filename)
                        obj_file = os.path.join('build', dirpath, filename.removesuffix('.cpp') + '.o')
                        ninja.build(obj_file, 'cxx', cpp_file)
                        objects.append(obj_file)

            ninja.build('wipeout-port', 'link', objects)
            ninja.default('wipeout-port')

        with open('build/compile_commands.json', 'wb') as f:
            subprocess.check_call(['ninja', '-t', 'compdb'], stdout=f)

        subprocess.check_call(['ninja'])
    finally:
        os.unlink('build.ninja')

if __name__ == '__main__':
    main()
