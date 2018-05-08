#!/usr/bin/env python


def depends(ctx):
    pass


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')


def configure(conf):
    conf.load('compiler_cxx')
    conf.load('gtest')

    conf.env['CXXFLAGS_UNI'] = [ '-std=c++11' ]
    #conf.env['CXXFLAGS_UNI'] += [ '-O2' ]
    conf.env['CXXFLAGS_UNI'] += [ '-ggdb' ]
    conf.env['INCLUDES_UNI'] = [ 'src' ]

    conf.check_cxx(mandatory=True,
                   header_name='cereal/cereal.hpp'
    )


def build(bld):
    bld.program (
        target = 'test-main',
        source = [
            'src/test/test-bytewise.cpp',
            'src/test/test-uni.cpp',
        ],
        features = 'gtest cxx',
        use = [ 'UNI' ],
    )

    bld(
        target = 'uni',
        export_includes = 'src'
    )

    # install headers
    rel = bld.path.find_node('src')
    for header in bld.path.ant_glob('src/uni/*.(h|hpp|ipp)'):
        bld.install_as('${PREFIX}/include/%s' % header.path_from(rel), header)
