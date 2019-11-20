#!/usr/bin/env python

import ROOT
#from ROOT.units import mm,meter
mm=1.0                          # something is broken
meter=1000.0*mm                 # with the dictionaries

from ROOT.WCP import Point
import math

def centered():
    """
    Return parameters for wire planes centered on the origin
    """
    # sizes
    dx=10*mm
    dy=4*meter
    dz=10*meter
    bmin = Point(-0.5*dx, -0.5*dy, -0.5*dz)
    bmax = Point(+0.5*dx, +0.5*dy, +0.5*dz)

    oY = Point(-0.4*dx, 0, 0)
    oU = Point(      0, 0, 0)
    oV = Point(+0.4*dx, 0, 0)

    pY = Point(0, 0, 3*mm)
    angle = 30             # fixme, is this the proper U/V convention?
    pU = Point(0, math.cos(+angle*math.pi/180), math.sin(+angle*math.pi/180))
    pV = Point(0, math.cos(-angle*math.pi/180), math.sin(-angle*math.pi/180))
    
    return (bmin,bmax, oU,pU, oV,pV, oY,pY)

