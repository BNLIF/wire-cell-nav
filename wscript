#!/usr/bin/env python

APPNAME = 'WireCellNav'

def options(ctx):
    ctx.load('find_package')

def configure(ctx):
    ctx.load('find_package')
#    ctx.env.LIBPATH_WireCell = ctx.options.prefix + '/lib'
#    ctx.env.INCLUDES_WireCell = ctx.options.prefix + '/include'
    ctx.check_cxx(lib='WireCellData', libpath=ctx.options.prefix + '/lib')


def build(bld):
    # main code library
    bld.shared_library(use='WireCell')
    bld.api_headers()
    bld.root_dictionary()

#    for testsrc in bld.path.ant_glob('test/test_*.cxx'):
#        bld.test_program([testsrc])


