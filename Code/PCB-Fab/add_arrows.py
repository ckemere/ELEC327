#!/usr/bin/env python3
import json
import xml.etree.ElementTree as ET
import re
from typing import List, Tuple

def parse_dimension(value, default=100):
    """
    Parse SVG dimension values with various units (px, mm, etc.)
    
    Args:
        value (str): The dimension value as a string (e.g., '297.0022mm', '100px')
        default (float): Default value to return if parsing fails
        
    Returns:
        float: The numeric value without units
    """
    if value is None:
        return default
        
    # Extract numeric part using regex
    match = re.match(r'([0-9.]+)([a-z]*)', value)
    if match:
        return float(match.group(1))
    return default

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

def add_arrows_to_svg(svg_file: str, positions: List[Tuple[float, float]], output_svg_file: str):
    """
    Add red arrows to an SVG file at the specified positions
    
    Args:
        svg_file (str): Path to the original SVG file
        positions (List[Tuple]): List of (x, y) coordinate tuples
        output_svg_file (str): Path to save the modified SVG file
    """
    # Parse the SVG file
    tree = ET.parse(svg_file)
    root = tree.getroot()
    
    # Extract namespace information if present
    namespace_match = re.match(r'{.*}', root.tag)
    namespace = namespace_match.group(0) if namespace_match else ''
    
    # Get or create a defs section (for styles)
    defs_element = root.find(f"{namespace}defs")
    if defs_element is None:
        defs_element = ET.SubElement(root, f"{namespace}defs")
    
    # Add a style for the arrows
    style_element = ET.SubElement(defs_element, f"{namespace}style")
    style_element.set("type", "text/css")
    style_element.text = """
        .drc-arrow {
            fill: red;
            font-size: 2px;
            font-weight: bold;
        }
    """
    
    # Group for all DRC violation markers
    drc_group = ET.SubElement(root, f"{namespace}g")
    drc_group.set("id", "drc-violations")
    
    # Process each violation position
    for x, y in positions:
        # Create text element with Unicode arrow
        arrow = ET.SubElement(drc_group, f"{namespace}text")
        arrow.set("x", str(x))
        arrow.set("y", str(y))
        arrow.set("class", "drc-arrow")
        arrow.set("text-anchor", "middle")
        arrow.set("dominant-baseline", "ideographic")
        # Rotate the arrow 45 degrees counter-clockwise around the violation point
        arrow.set("transform", f"rotate(-45, {x}, {y})")
        # Unicode arrow pointing down (↓)
        arrow.text = "↓"
        
        # Add title for tooltip
        title = ET.SubElement(arrow, f"{namespace}title")
        title.text = "DRC Violation"
    
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