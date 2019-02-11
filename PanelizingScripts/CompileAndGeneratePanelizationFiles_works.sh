#!/bin/bash
PATH=$PATH:/Applications/EAGLE-7.7.0/EAGLE.app/Contents/MacOS

counter=1;
SubmissionCounter=0;


### Following is the preabmble for the gerbmerge layout file
read -d '' LayoutPreamble << "EOS"
# GerbMerge Panelization for Advanced Circuits Barebones
[DEFAULT]
projdir = .
MergeOut = panelized
[Options]
# Optional indication of the number of decimal places in input Excellon drill
# files. The default is 4. Note that EAGLE 7 is now using 5 by default
#ExcellonDecimals = 4

# Which layers to draw cut lines on. Omit this option or set to 'None' for no
# cut lines. Cut lines are borders around each job that serve as guides for
# cutting the panel into individual jobs. Option 'CutLineWidth' sets the
# thickness of these cut lines.
#
# NOTE: Layer names are ALL LOWERCASE, even if you define them with uppercase
# letters below.
CutLineLayers = *topsilkscreen,*bottomsilkscreen
# Optional additional Gerber layer on which to draw a rectangle defining the
# extents of the entire panelized job. This will create a Gerber file (with
# name specified by this option) that simply contains a rectangle defining the
# outline of the final panel. This outline file is useful for circuit board
# milling to indicate a path for the router tool. There's no harm in creating
# this file...you can ignore it if you don't need it.
OutlineLayerFile = %(mergeout)s.oln
# Set the maximum dimensions of the final panel, if known. The dimensions are
# specified in inches.
PanelWidth = 10
PanelHeight = 10
# Set the amount of extra space to leave around the edges of the panel to
# simplify tooling and handling. These margins are specified in inches
LeftMargin   = 0.1
RightMargin  = 0.1
TopMargin    = 0.1
BottomMargin = 0.1
# Set the inter-job spacing (inches) in both the X-dimension (width) and
# Y-dimension (height). 
XSpacing = 0.125
YSpacing = 0.125
# Width of cut lines, in inches. The default value is 0.01". These are drawn on
# the layers specified by CutLineLayers.
CutLineWidth = 0.01

[MergeOutputFiles]
Prefix = %(mergeout)s
*TopLayer=%(prefix)s.GTL
*BottomLayer=%(prefix)s.GBL
Drills=%(prefix)s.TXT
BoardOutline=%(prefix)s.GML
ToolList = toollist.%(prefix)s.drl
Placement = placement.%(prefix)s.twt
EOS

echo "$LayoutPreamble" > layout_gen.cfg
echo "#Layout file by alphabetical order" > layout_gen.def;
echo "Row {" >> layout_gen.def;
echo "  Col {" >> layout_gen.def;


find . -name "*.brd" | while read i
do
  prefix=$(echo $i | sed -e "s/\(.*\).brd/\1/");
  student=$(echo $i | sed -e "s/\.\/\([^_]*\)_.*.brd/\1/");
  echo $student;
  echo "[$student]" >> layout_gen.cfg;

  EAGLE -X -d GERBER_RS274X -o "$student.GTL" "$prefix.brd" Top Pads Vias Dimension > /dev/null
  EAGLE -X -d GERBER_RS274X -o "$student.GBL" "$prefix.brd" Bottom Pads Vias > /dev/null
  EAGLE -X -d GERBER_RS274X -o "$student.GML" "$prefix.brd" Dimension Milling > /dev/null
  EAGLE -X -d EXCELLON_24 -o "$student.TXT" "$prefix.brd" Drills Holes > /dev/null

  echo "*TopLayer=$student.GTL" >> layout_gen.cfg;
  echo "*BottomLayer=$student.GBL" >> layout_gen.cfg;
  echo "BoardOutline=$student.GML" >> layout_gen.cfg;
  echo "Drills=$student.TXT" >> layout_gen.cfg;

  counter=$[counter + 1];
  let "SubmissionCounter++"
  if [ $counter -gt "5" ]
  then
    echo "  }" >> layout_gen.def;
    echo "  Col {" >> layout_gen.def;
    counter=0;
  fi
  echo "    $student" >> layout_gen.def;
  echo $SubmissionCounter
done
echo "  }" >> layout_gen.def;
echo "}" >> layout_gen.def;

 
