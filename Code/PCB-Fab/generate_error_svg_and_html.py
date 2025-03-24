#!/usr/bin/env python3
import json
import re
from typing import List, Tuple
from svgpathtools import svg2paths2
from xml.etree.ElementTree import Element, SubElement, tostring, ElementTree, parse, register_namespace

# Register the SVG namespace
SVG_NAMESPACE = "http://www.w3.org/2000/svg"
register_namespace("", SVG_NAMESPACE)

def parse_drc_violations(json_file: str) -> List[Tuple[float, float, int, str]]:
    """
    Parse JSON file containing DRC violations and extract positions and descriptions
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        
    Returns:
        List[Tuple]: List of (x, y, id, description) tuples for each violation
    """
    # Load DRC violations from JSON file
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    # Extract violation positions and descriptions
    violations = []
    idx = 1
    for violation in data.get("violations", []):
        main_description = violation.get("description", "")

        for item in violation.get("items", []):
            description = item.get("description", "")
            pos = item.get("pos", {})
            if "x" in pos and "y" in pos:
                violations.append((float(pos["x"]), float(pos["y"]), idx,
                                   "{}:{}".format(main_description, description)))
        idx = idx + 1

    for violation in data.get("unconnected_items", []):
        main_description = violation.get("description", "")

        for item in violation.get("items", []):
            description = item.get("description", "")
            pos = item.get("pos", {})
            if "x" in pos and "y" in pos:
                violations.append((float(pos["x"]), float(pos["y"]), idx,
                                   "{}:{}".format(main_description, description)))
        idx = idx + 1


    return violations

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

def add_arrows_to_svg(svg_file: str, violations: List[Tuple[float, float, str]], output_svg_file: str):
    """
    Add red arrows to an SVG file at the specified positions
    
    Args:
        svg_file (str): Path to the original SVG file
        violations (List[Tuple]): List of (x, y, description) tuples
        output_svg_file (str): Path to save the modified SVG file
    """
    # Load the SVG file
    paths, attributes, svg_attributes = svg2paths2(svg_file)
    
    # Parse the original SVG file
    tree = parse(svg_file)
    root = tree.getroot()
    
    # Create a defs section (for styles) if it doesn't exist
    defs = root.find(f'{{{SVG_NAMESPACE}}}defs')
    if defs is None:
        defs = SubElement(root, f'{{{SVG_NAMESPACE}}}defs')
    
    # Group for all DRC violation markers
    drc_group = SubElement(root, f'{{{SVG_NAMESPACE}}}g', id="drc-violations")
    
    # Process each violation position
    for (x, y, idx, description) in violations:
        # Create text element with Unicode arrow
        arrow = SubElement(drc_group, f'{{{SVG_NAMESPACE}}}text', {
            'x': str(x),
            'y': str(y),
            'fill': 'red',
            'font-size': '0.25mm',
            'font-weight': 'bold',
            'text-anchor': 'start',
            'dominant-baseline': 'ideographic',
            'font-family': 'Arial, sans-serif',
            'transform': 'translate(-0.15, 0)'  # Shift the arrow right by 0.05mm
        })
        arrow.text = "↓"
        
        # Create a tspan element to hold the index with a smaller font size
        tspan = SubElement(arrow, f'{{{SVG_NAMESPACE}}}tspan', {
            'font-size': '0.25mm',
            'dominant-baseline': 'middle',
            'font-family': 'Arial, sans-serif'
        })
        tspan.text = f"[{idx}]"

        # Shift the arrow over just a little bit
    
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
    print(f"Added {len(violations)} DRC violation markers")
    print(f"Modified SVG saved to {output_svg_file}")

def create_html_report(svg_file: str, violations: List[Tuple[float, float, int, str]], output_html_file: str):
    """
    Create an HTML report displaying the SVG and a table of DRC violations
    
    Args:
        svg_file (str): Path to the SVG file
        violations (List[Tuple]): List of (x, y, description) tuples
        output_html_file (str): Path to save the HTML report
    """
    # Create the HTML content
    html_content = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>DRC Report</title>
        <style>
            table {{
                width: 100%;
                border-collapse: collapse;
            }}
            th, td {{
                border: 1px solid black;
                padding: 8px;
                text-align: left;
            }}
            th {{
                background-color: #f2f2f2;
            }}
        </style>
    </head>
    <body>
        <h1>DRC Report</h1>
        <div>
            <object type="image/svg+xml" data="{svg_file}" width="100%" height="600px"></object>
        </div>
        <h2>DRC Violations</h2>
        <table>
            <tr>
                <th>Index</th>
                <th>Description</th>
            </tr>
    """
    
    # Add rows to the table for each violation
    for (_, _, idx, description) in violations:
        html_content += f"""
            <tr>
                <td>{idx}</td>
                <td>{description}</td>
            </tr>
        """
    
    # Close the HTML tags
    html_content += """
        </table>
    </body>
    </html>
    """
    
    # Save the HTML content to a file
    with open(output_html_file, 'w') as f:
        f.write(html_content)
    print(f"HTML report saved to {output_html_file}")

def add_drc_markers(json_file: str, svg_file: str, output_svg_file: str, output_html_file: str):
    """
    Main function to coordinate the process of adding DRC markers and creating an HTML report
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        svg_file (str): Path to the SVG file to be modified
        output_svg_file (str): Path to save the modified SVG file
        output_html_file (str): Path to save the HTML report
    """
    # Step 1: Parse the JSON to get violation positions and descriptions
    violations = parse_drc_violations(json_file)
    
    # Step 2: Add arrows to the SVG file
    add_arrows_to_svg(svg_file, violations, output_svg_file)
    
    # Step 3: Create the HTML report
    create_html_report(output_svg_file, violations, output_html_file)

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) != 5:
        print("Usage: python script.py <json_file> <svg_file> <output_svg_file> <output_html_file>")
        print("Example: python script.py Test.json Test.svg Test_with_violations.svg report.html")
        sys.exit(1)
    
    json_file = sys.argv[1]
    svg_file = sys.argv[2]
    output_svg_file = sys.argv[3]
    output_html_file = sys.argv[4]
    
    add_drc_markers(json_file, svg_file, output_svg_file, output_html_file)