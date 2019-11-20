#!/usr/bin/env python

import ROOT

# I shouldn't need this! 
# something is broken with rootmaps
ROOT.gSystem.Load("libWCPNavDict")


def test_make():
    gds = ROOT.WCP.make_example_gds()
    assert gds

if '__main__' == __name__:
    test_make()

