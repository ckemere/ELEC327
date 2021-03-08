
# This is intended to be used after gerbmerge in order to get the gerbmerge layout out to use
# with ThisIsNotRocketScience gerbertools panelizer.

# (Use the https://github.com/ckemere/gerbmerge fork to have the extra columns of the placement
# file created!)

# In order to run gerbmerge, you should run the CompileAndGeneratePanelizationFiles.sh script!


import pandas as pd


panelizer_dir = ''
base_path = 'color_sensors'

import os
panelizer_file = os.path.join(panelizer_dir, 'placement.panlized.twt')

df = pd.read_csv('placement.panelized.twt', header=None, delimiter=' ')
df.columns = ['unparsed','x','y','w','h', 'minx', 'miny', 'orig_minx', 'orig_miny']

student_names = [nm.split('*')[0] for nm in df.iloc[:,0]]

is_rotated = [True if len(nm.split('*')) > 1 else False for nm in df.iloc[:,0]]

#df['student_name'] = student_names
df['is_rotated'] = is_rotated


df['xshift'] = df['orig_minx']
df['yshift'] = df['orig_miny']
df.xshift[df.xshift == 9999999.0] = df.minx[df.xshift == 9999999.0]
df.yshift[df.yshift == 9999999.0] = df.miny[df.yshift == 9999999.0]

df['new_x'] = df['x'] + is_rotated*df['w']  # For 90 degree rotations the bottom left corner becomes the bottom right, so we need to shift the effective origin over by the width

print(df)


import xml.etree.ElementTree as ET

paneldata = ET.Element('GerberLayoutSet')
loaded_outlines = ET.SubElement(paneldata, 'LoadedOutlines')
for name in set(student_names):
  outline_string = ET.SubElement(loaded_outlines,'string')
  outline_string.text = '\\'.join(['Z:',base_path, name+'.zip'])

instances = ET.SubElement(paneldata, 'Instances')

gerber_instance_list = []

for idx, name in enumerate(student_names):
   gerber_instance = ET.SubElement(instances, 'GerberInstance')
   gerber_instance_list.append(gerber_instance)
   center = ET.SubElement(gerber_instance, 'Center')
   if not is_rotated[idx]:
     ET.SubElement(center, 'X').text = '{}'.format((df.new_x.iloc[idx] - df.xshift.iloc[idx]/100000)*25.4)
     ET.SubElement(center, 'Y').text = '{}'.format((df.y.iloc[idx] - df.yshift.iloc[idx]/100000)*25.4)
   else:
     # ET.SubElement(center, 'X').text = '{}'.format((df.new_x.iloc[idx])*25.4)
     # ET.SubElement(center, 'Y').text = '{}'.format((df.y.iloc[idx])*25.4)
     ET.SubElement(center, 'X').text = '{}'.format((df.new_x.iloc[idx] + df.yshift.iloc[idx]/100000)*25.4)
     ET.SubElement(center, 'Y').text = '{}'.format((df.y.iloc[idx] - df.xshift.iloc[idx]/100000)*25.4)

   angle = ET.SubElement(gerber_instance, 'Angle')
   if is_rotated[idx]:
      angle.text = '90'
   else:
      angle.text = '0'
   gerber_path = ET.SubElement(gerber_instance, 'GerberPath')
   gerber_path.text = '\\'.join(['Z:',base_path, name+'.zip'])
   generated = ET.SubElement(gerber_instance, 'Generated')
   generated.text = 'false'
      
 
ET.SubElement(paneldata, 'Width').text = '200'
ET.SubElement(paneldata, 'Height').text = '200'
ET.SubElement(paneldata, 'MarginBetweenBoards').text = '{}'.format(0.125*25.4)

ET.SubElement(paneldata, 'ConstructNegativePolygon').text = 'false'
ET.SubElement(paneldata, 'FillOffset').text = '3'
ET.SubElement(paneldata, 'Smoothing').text = '1'
ET.SubElement(paneldata, 'ExtraTabDrillDistance').text = '0'
ET.SubElement(paneldata, 'ClipToOutlines').text = 'true'
ET.SubElement(paneldata, 'LastExportFolder').text = '.'
ET.SubElement(paneldata, 'DoNotGenerateMouseBites').text = 'false'
ET.SubElement(paneldata, 'MergeFileTypes').text = 'false'

from xml.dom import minidom
with open('test.xml', 'w') as xf: 
    xf.write(minidom.parseString(ET.tostring(paneldata)).toprettyxml(indent='   ')) 



