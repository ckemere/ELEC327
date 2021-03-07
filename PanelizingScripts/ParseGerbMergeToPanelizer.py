

import pandas as pd


panelizer_dir = ''

import os
panelizer_file = os.path.join(panelizer_dir, 'placement.panlized.twt')

df = pd.read_csv('placement.panelized.twt', header=None, delimiter=' ')
df.columns = ['unparsed','x','y']

student_names = [nm.split('*')[0] for nm in df.iloc[:,0]]

is_rotated = [True if len(nm.split('*')) > 1 else False for nm in df.iloc[:,0]]

df['student_name'] = student_names
df['is_rotated'] = is_rotated

print(df)


import xml.etree.ElementTree as ET

paneldata = ET.Element('GerberLayoutSet')
