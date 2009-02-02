#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Copyright (C) 2000-2008 Alistair Riddoch

from atlas import *
from random import *
from mind.panlingua import interlinguish
il=interlinguish
from cyphesis import probability
from cyphesis.editor import editor
from Quaternion import Quaternion
from Vector3D import Vector3D
import time
from math import *

from define_world import *

def test_skeleton(mapeditor):
#   general things

    m=editor(mapeditor)

    skeleton = m.make('skeleton', pos=(-38,-25,settlement_height))
    m.learn(skeleton,skeleton_goals)

def test_pig(mapeditor):
#   general things

    m=editor(mapeditor)
    pig = m.make('pig', pos=(3,3,settlement_height))
    m.learn(pig,pig_goals)
    m.make('acorn', pos=(4,4,settlement_height))

def test_browse(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    deer = m.make('deer', pos=(5, 0, settlement_height))
    m.learn(deer, (il.browse,"browse('fir', 0.8)"))
    m.make('fir',pos=(-10,-0,settlement_height))
    m.make('fir',pos=(-0,-10,settlement_height))
    m.make('fir',pos=(0,10,settlement_height))
    m.make('fir',pos=(10,0,settlement_height))
    
def test_forest(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],pos=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

   
def test_coll(mapeditor):

    m=editor(mapeditor)

    sty=m.make('sty',pos=pig_sty_pos,status=1.0,bbox=[5,5,3], orientation=directions[0])

def test_butcher(mapeditor):

    m=editor(mapeditor)

    butcher=m.make('merchant', name='Ulad Bargan',desc='the butcher',
                 pos=(3,3,0),age=probability.fertility_age)
    cleaver=m.make('cleaver', place='market', pos=(3,2.5,settlement_height))
    m.own(butcher,cleaver)
    m.learn(butcher,(il.trade,"trade('pig', 'cleaver', 'cut', 'ham', 'market')"))
    piglet = m.make('pig', pos=(3,2,0))

def test_pig(mapeditor):

    m=editor(mapeditor)

    piglet = m.make('pig', pos=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

def test_settler(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('axe',pos=(0,0,0),parent=settler.id)
    m.own(settler,axe)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"harvest_resource('lumber','oak','forest','axe')"))
    m.make('oak',pos=(32,32,0))

def test_forester(mapeditor):

    m=editor(mapeditor)
    # An NPC forester
    settler=m.make('settler', name='forester',pos=(0, 12, 0))
    trowel=m.make('trowel',pos=(0,0,0),parent=settler.id)
    m.own(settler, trowel)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"plant_seeds('acorn','oak','forest','trowel')"))
    m.make('oak',pos=(32,32,0))

def test_warrior(mapeditor):

    m=editor(mapeditor)
    # An NPC warrior
    warrior=m.make('mercenary',pos=(0, 12, 0))
    m.learn(warrior,(il.trade,"gather(['loaf', 'axe'])"))

def test_chicken(mapeditor):

    m=editor(mapeditor)

    chuck = m.make('chicken', pos=(-3,-1,settlement_height))
    m.learn(chuck,chicken_goals)

def test_path(mapeditor):

    m=editor(mapeditor)
    path_area={'points' : [ [2,0], [22, 40], [132,122], [140,127], [144.5, 146.5], [169, 153], [169,155], [142.5,148.5], [138,129], [130,124], [18,40], [-2,-1] ], 'layer' : 7}
    m.make('path', name='path to village',pos=(10, 20,settlement_height), area=path_area,bbox=[169,154,1])
   
    
lych2_knowledge=[('w1','location',(-140,110,graveyard_height)),
                ('w2','location',(-50,160,graveyard_height)),
                ('w3','location',(-80,40,graveyard_height)),
                ('w4','location',(-180,150,graveyard_height))]

lych2_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]
   
def test_graveyard(mapeditor):
    m = editor(mapeditor)
    
    m.make('mausoleum', name='mausoleum of harrington', pos=mausoleum_pos, orientation=directions[0])
    m.make('wall', name='steps',pos=(-160,100,graveyard_height),bbox=[4,4,2])
    
    graveyard_area={'points': [[-10, -8], [15, -11], [13,23], [-8, 8]], 'layer':7 }
    m.make('path', name='graveyard', pos=(-150, 110, graveyard_height), area=graveyard_area, bbox=[-10, -11,0, 15, 23, 1])
    
    path_area={'points' : [ [2, 0], [20, 20], [45, 60], [65, 70], [83, 40], [106, -15], [105, -12], [82, 36], [66, 67], [44, 57], [18, 16], [0, -4]], 'layer' : 7}
    m.make('path', name='path to graveyard',pos=(-150, 110, graveyard_height), area=path_area,bbox=[100,8,1])
    gravestone_styles = ['simple', 'rounded', 'stylish']
        

    m.make('gravestone', name='gravestone1', pos=(-145, 107, graveyard_height), orientation=directions[1], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone2', pos=(-147, 104, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone3', pos=(-148, 109, graveyard_height), orientation=directions[3], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone4', pos=(-150, 117, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone5', pos=(-143, 111, graveyard_height), orientation=directions[4], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone6', pos=(-149, 110, graveyard_height), orientation=directions[3], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone7', pos=(-147, 116, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    
    lych=m.make('lych', pos=(-140, 130, graveyard_height))
    m.learn(lych,lych2_goals)
    m.know(lych,lych2_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

def test_fire(mapeditor):
    m = editor(mapeditor)

    cfire=m.make('campfire',pos=(3,9,settlement_height))
    # m.make('fire',pos=(0,0,0),parent=cfire.id)

def test_own(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('axe',pos=(0,0,0),parent=settler.id)
    m.own(settler,axe)


def test_goblins(mapeditor):

    m=editor(mapeditor)
    goblin_guards=[]
    goblin=m.make('goblin', pos=(102, -33, settlement_height))
    goblin_guards.append(goblin)
    goblin=m.make('goblin', pos=(98, -33, settlement_height))
    goblin_guards.append(goblin)

    m.learn(goblin_guards,(il.defend,"defend('settler', 10)"))

def test_deer(mapeditor):

    m=editor(mapeditor)

    d=m.make('deer', pos=(5, 0, settlement_height))
    m.learn(d,deer_goals)
    # m.make('settler', pos=(0, 0, settlement_height))

def kill_world(mapeditor):

    m=editor(mapeditor)

    world=m.look()
    m.set(world.id, status=-1)

def kill_tree(mapeditor):

    m=editor(mapeditor)

    tree=m.make('oak')
    m.set(tree.id, status=-1)

def test_oak(mapeditor):

    m=editor(mapeditor)

    tree=m.make('oak')

def test_attachment(mapeditor):

    m=editor(mapeditor)

    m.make('axe', attachment=23)

def make_shirt(mapeditor):

    m=editor(mapeditor)

    m.make('shirt', worn='chest', attachment=23, mass=2)
    m.make('acorn', worn='chest', pos=(1,1,1), attachment=23, mass=2)

def test_decays(mapeditor):

    m=editor(mapeditor)

    m.make('axe', decays='oak')

def test_device(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('door',pos=(0,0,0))
    m.learn(settler,(il.trade,"activate_device('door')"))

    world=m.look()

def test_tmods(mapeditor):
#   general things
    m=editor(mapeditor)
    
    world=m.look()
    points = { }
    for i in range(-8, 7):
        for j in range(-6, 7):
            if i>=5 or j>=5:
                points['%ix%i'%(i,j)] = [i, j, uniform(100, 150)]
            elif i<=-5 or j <= -5:
                points['%ix%i'%(i,j)] = [i, j, uniform(-30, -10)]
            elif (i==2 or i==3) and (j==2 or j==3):
                points['%ix%i'%(i,j)] = [i, j, uniform(20, 25)]
            elif i==4 or j==4:
                points['%ix%i'%(i,j)] = [i, j, uniform(30, 80)]
            elif i==-4 or j==-4:
                points['%ix%i'%(i,j)] = [i, j, uniform(-5, 5)]
            else:
                points['%ix%i'%(i,j)] = [i, j, 1+uniform(3, 11)*(abs(i)+abs(j))]

    points['-4x-1'] = [-4, -1, 12.4]
    points['-4x-2'] = [-4, -2, -8.3]
    points['-3x-2'] = [-3, -2, -6.2]
    points['-3x-1'] = [-3, -1, -5.3]
    points['-2x-1'] = [-2, -1, -4.1]
    points['-1x-1'] = [-1, -1, -16.8]
    points['0x-1'] = [0, -1, -3.8]
    points['-1x0'] = [-1, 0, -2.8]
    points['-1x1'] = [-1, 1, -1.8]
    points['-1x2'] = [-1, 2, -1.7]
    points['0x2'] = [0, 2, -1.6]
    points['1x2'] = [1, 2, -1.3]
    points['1x3'] = [1, 3, -1.1]
    points['1x4'] = [1, 4, -0.6]
    points['1x-1'] = [1, -1, 15.8]
    points['0x0'] = [0, 0, 12.8]
    points['1x0'] = [1, 0, 23.1]
    points['0x1'] = [0, 1, 14.2]
    points['1x1'] = [1, 1, 19.7]

    minx=0
    miny=0
    minz=0
    maxx=0
    maxy=0
    maxz=0
    for i in points.values():
        x = i[0]
        y = i[1]
        z = i[2]
        if not minx or x < minx:
            minx = x
        if not miny or y < miny:
            miny = y
        if not minz or z < minz:
            minz = z
        if not maxx or x > maxx:
            maxx = x
        if not maxy or y > maxy:
            maxy = y
        if not maxz or z > maxz:
            maxz = z
   
    terrainMod1 = {'type' : 'levelmod', 'pos' : [40,40,15], 'shape' :
        {'type' : 'ball', 'dim' : 2, 'radius' : 10 },
        'height' : 40
    }
    
    terrainMod2 = {'type' : 'cratermod', 'pos' : [0,0,0], 'shape' :
        {'type' : 'ball', 'dim' : 3, 'radius' : 10 },
    }
    
    myterrain = {'points' : points}
    m.set(world.id, terrain=myterrain, name="terrainMod_testLand", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])
    
    m.make('tower',pos=(60,60,5),terrainmod=terrainMod1)
    m.make('tower',pos=(20,20,5), terrainmod=terrainMod2)