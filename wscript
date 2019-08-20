#!/usr/bin/env python


def depends(ctx):
    pass


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')


def configure(conf):
    conf.load('compiler_cxx')
    conf.load('gtest')

    conf.check_cxx(mandatory=True,
                   header_name='cereal/cereal.hpp'
    )

    conf.env['INCLUDES_UNI'] = [ 'src' ]


def build(bld):
    bld.program (
        target = 'uni_v2_test-main',
        source = [
            'src/test/v2/test-bytewise.cpp',
            'src/test/v2/test-uni.cpp',
            'src/test/v2/test-cerealization.cpp',
        ],
        features = 'gtest cxx',
        use = [ 'UNI' ],
    )

    bld.program (
        target = 'uni_v3_test-main',
        source = [
            'src/test/v3/test-bytewise.cpp',
            'src/test/v3/test-uni.cpp',
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
    for header in bld.path.ant_glob('src/uni/**/*.(h|hpp|ipp)'):
        bld.install_as('${PREFIX}/include/%s' % header.path_from(rel), header)
