#!/usr/bin/env python3
import json
import re
from typing import List, Tuple
from svgpathtools import svg2paths2
from xml.etree.ElementTree import Element, SubElement, tostring, ElementTree, parse

def parse_drc_violations(json_file: str) -> List[Tuple[float, float]]:
    """
    Parse JSON file containing DRC violations and extract positions
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        
    Returns:
        List[Tuple]: List of (x, y) coordinate tuples for each violation
    """
    # Load DRC violations from JSON file
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    # Extract violation positions
    positions = []
    for violation in data.get("violations", []):
        for item in violation.get("items", []):
            pos = item.get("pos", {})
            if "x" in pos and "y" in pos:
                positions.append((float(pos["x"]), float(pos["y"])))
    
    return positions

def convert_to_mm(value: str) -> float:
    """
    Convert a value with units to millimeters
    
    Args:
        value (str): The value with units
    
    Returns:
        float: The value in millimeters
    """
    if value.endswith('mm'):
        return float(value[:-2])
    elif value.endswith('cm'):
        return float(value[:-2]) * 10
    elif value.endswith('in'):
        return float(value[:-2]) * 25.4
    elif value.endswith('px'):
        # Assuming 96 DPI for pixel to mm conversion
        return float(value[:-2]) * 25.4 / 96
    else:
        return float(value)

def get_bounding_box(paths):
    bbox = {'x_min': float('inf'), 'y_min': float('inf'), 'x_max': float('-inf'), 'y_max': float('-inf')}
    
    for path in paths:
        for segment in path:
            bbox['x_min'] = min(bbox['x_min'], segment.start.real, segment.end.real)
            bbox['y_min'] = min(bbox['y_min'], segment.start.imag, segment.end.imag)
            bbox['x_max'] = max(bbox['x_max'], segment.start.real, segment.end.real)
            bbox['y_max'] = max(bbox['y_max'], segment.start.imag, segment.end.imag)
    
    return bbox

def add_arrows_to_svg(svg_file: str, positions: List[Tuple[float, float]], output_svg_file: str):
    """
    Add red arrows to an SVG file at the specified positions
    
    Args:
        svg_file (str): Path to the original SVG file
        positions (List[Tuple]): List of (x, y) coordinate tuples
        output_svg_file (str): Path to save the modified SVG file
    """
    # Load the SVG file
    paths, attributes, svg_attributes = svg2paths2(svg_file)
    
    # Parse the original SVG file
    tree = parse(svg_file)
    root = tree.getroot()
    
    # Create a defs section (for styles) if it doesn't exist
    defs = root.find('defs')
    if defs is None:
        defs = SubElement(root, 'defs')
    
    # Group for all DRC violation markers
    drc_group = SubElement(root, 'g', id="drc-violations")
    
    # Process each violation position
    for idx, (x, y) in enumerate(positions):
        # Create text element with Unicode arrow
        arrow = SubElement(drc_group, 'text', {
            'x': str(x),
            'y': str(y),
            'fill': 'red',
            'font-size': '1mm',
            'font-weight': 'bold',
            'text-anchor': 'start',
            'dominant-baseline': 'ideographic'
        })
        arrow.text = "↓"
        
        # Create a tspan element to hold the index with a smaller font size
        tspan = SubElement(arrow, 'tspan', {
            'font-size': '0.5mm',
            'dominant-baseline': 'middle'
        })
        tspan.text = f"[{idx}]"
    
    # Calculate the bounding box of the entire SVG
    bbox = get_bounding_box(paths)
    width = bbox['x_max'] - bbox['x_min'] + 4  # Expand width by 4 mm
    height = bbox['y_max'] - bbox['y_min'] + 4  # Expand height by 4 mm
    
    # Adjust the width, height, and viewBox attributes of the root element
    root.set('width', f"{width}mm")
    root.set('height', f"{height}mm")
    root.set('viewBox', f"{bbox['x_min'] - 2} {bbox['y_min'] - 2} {width} {height}")  # Adjust viewBox accordingly
        
    # Save the modified SVG
    tree.write(output_svg_file)
    print(f"Added {len(positions)} DRC violation markers")
    print(f"Modified SVG saved to {output_svg_file}")

def add_drc_markers(json_file: str, svg_file: str, output_svg_file: str):
    """
    Main function to coordinate the process of adding DRC markers
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        svg_file (str): Path to the SVG file to be modified
        output_svg_file (str): Path to save the modified SVG file
    """
    # Step 1: Parse the JSON to get violation positions
    positions = parse_drc_violations(json_file)
    
    # Step 2: Add arrows to the SVG file
    add_arrows_to_svg(svg_file, positions, output_svg_file)

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) != 4:
        print("Usage: python script.py <json_file> <svg_file> <output_svg_file>")
        print("Example: python script.py Test.json Test.svg Test_with_violations.svg")
        sys.exit(1)
    
    json_file = sys.argv[1]
    svg_file = sys.argv[2]
    output_svg_file = sys.argv[3]
    
    add_drc_markers(json_file, svg_file, output_svg_file)