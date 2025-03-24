#!/usr/bin/env python3
import json
import os
from typing import List, Dict, Tuple

def parse_drc_violations(json_file: str) -> List[Dict]:
    """
    Parse JSON file containing DRC violations and extract positions and descriptions
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        
    Returns:
        List[Dict]: List of dictionaries with x, y coordinates and description for each violation
    """
    # Load DRC violations from JSON file
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    # Extract violation positions and descriptions
    violations = []
    for violation in data.get("violations", []):
        violation_desc = violation.get("description", "")
        violation_type = violation.get("type", "")
        for item in violation.get("items", []):
            pos = item.get("pos", {})
            item_desc = item.get("description", "")
            if "x" in pos and "y" in pos:
                violations.append({
                    "x": float(pos["x"]),
                    "y": float(pos["y"]),
                    "description": f"{violation_desc}<br>Item: {item_desc}<br>Type: {violation_type}"
                })
    
    return violations

def create_html_with_svg(svg_file: str, violations: List[Dict], output_html_file: str):
    """
    Create an HTML file that embeds the SVG and adds tooltips for DRC violations
    
    Args:
        svg_file (str): Path to the SVG file
        violations (List[Dict]): List of violation data (x, y coordinates and descriptions)
        output_html_file (str): Path to save the HTML file
    """
    # Read the SVG file content
    with open(svg_file, 'r') as f:
        svg_content = f.read()
    
    # Generate the HTML content
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PCB Design with DRC Violations</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }}
        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }}
        h1 {{
            color: #333;
            text-align: center;
        }}
        .svg-container {{
            width: 100%;
            overflow: auto;
            border: 1px solid #ddd;
            margin-top: 20px;
            position: relative;
        }}
        .tooltip {{
            position: absolute;
            background-color: rgba(0, 0, 0, 0.8);
            color: white;
            padding: 10px;
            border-radius: 4px;
            max-width: 300px;
            z-index: 100;
            display: none;
            pointer-events: none;
            font-size: 14px;
            line-height: 1.4;
        }}
        .stats {{
            margin-top: 20px;
            padding: 15px;
            background-color: #f0f8ff;
            border-radius: 4px;
            border-left: 4px solid #4682b4;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>PCB Design DRC Violations</h1>
        
        <div class="stats">
            <p><strong>Total violations:</strong> {len(violations)}</p>
        </div>
        
        <div class="svg-container" id="svg-container">
            {svg_content}
            <div class="tooltip" id="tooltip"></div>
        </div>
    </div>

    <script>
        // Violation data
        const violations = {json.dumps(violations)};
        
        // Get tooltip element
        const tooltip = document.getElementById('tooltip');
        
        // Find the SVG element and all DRC arrow elements
        const svgElement = document.querySelector('svg');
        const svgContainer = document.getElementById('svg-container');
        
        // Apply zoom and pan functionality
        let scale = 1;
        let panning = false;
        let pointX = 0;
        let pointY = 0;
        let start = {{ x: 0, y: 0 }};
        
        // Add zoom event
        svgContainer.addEventListener('wheel', (e) => {{
            e.preventDefault();
            const xs = (e.clientX - svgContainer.offsetLeft);
            const ys = (e.clientY - svgContainer.offsetTop);
            const delta = e.deltaY;
            
            if (delta > 0) {{
                scale *= 0.9;
            }} else {{
                scale *= 1.1;
            }}
            
            // Limit scale
            scale = Math.min(Math.max(0.5, scale), 5);
            
            // Apply transform to the SVG
            svgElement.style.transformOrigin = `${{xs}}px ${{ys}}px`;
            svgElement.style.transform = `translate(${{pointX}}px, ${{pointY}}px) scale(${{scale}})`;
        }});
        
        // Add panning events
        svgContainer.addEventListener('mousedown', (e) => {{
            panning = true;
            start = {{ x: e.clientX - pointX, y: e.clientY - pointY }};
        }});
        
        document.addEventListener('mousemove', (e) => {{
            if (!panning) return;
            pointX = (e.clientX - start.x);
            pointY = (e.clientY - start.y);
            svgElement.style.transform = `translate(${{pointX}}px, ${{pointY}}px) scale(${{scale}})`;
        }});
        
        document.addEventListener('mouseup', () => {{
            panning = false;
        }});
        
        // Function to find the closest violation to coordinates
        function findClosestViolation(x, y) {{
            // Convert SVG coordinates to screen coordinates
            const svgRect = svgElement.getBoundingClientRect();
            const svgWidth = svgElement.viewBox.baseVal.width;
            const svgHeight = svgElement.viewBox.baseVal.height;
            
            const scaleX = svgRect.width / svgWidth;
            const scaleY = svgRect.height / svgHeight;
            
            // Calculate max distance for match (5 pixels)
            const maxDistance = 5 / Math.min(scaleX, scaleY);
            
            let closestViolation = null;
            let minDistance = Infinity;
            
            for (const violation of violations) {{
                const dx = violation.x - x;
                const dy = violation.y - y;
                const distance = Math.sqrt(dx * dx + dy * dy);
                
                if (distance < maxDistance && distance < minDistance) {{
                    minDistance = distance;
                    closestViolation = violation;
                }}
            }}
            
            return closestViolation;
        }}
        
        // Add event listener to arrows/text elements within the DRC violations group
        svgElement.addEventListener('mousemove', (e) => {{
            // Get SVG coordinates from mouse position
            const svgPoint = svgElement.createSVGPoint();
            svgPoint.x = e.clientX;
            svgPoint.y = e.clientY;
            
            // Convert to SVG coordinates
            const svgMatrix = svgElement.getScreenCTM().inverse();
            const transformedPoint = svgPoint.matrixTransform(svgMatrix);
            
            // Find closest violation
            const violation = findClosestViolation(transformedPoint.x, transformedPoint.y);
            
            if (violation) {{
                tooltip.style.display = 'block';
                tooltip.style.left = (e.pageX + 10) + 'px';
                tooltip.style.top = (e.pageY + 10) + 'px';
                tooltip.innerHTML = violation.description;
            }} else {{
                tooltip.style.display = 'none';
            }}
        }});
        
        // Handle mouse leave for svg container
        svgContainer.addEventListener('mouseleave', () => {{
            tooltip.style.display = 'none';
        }});
        
        // Handle arrow element click - highlight the clicked violation
        svgElement.addEventListener('click', (e) => {{
            const svgPoint = svgElement.createSVGPoint();
            svgPoint.x = e.clientX;
            svgPoint.y = e.clientY;
            const svgMatrix = svgElement.getScreenCTM().inverse();
            const transformedPoint = svgPoint.matrixTransform(svgMatrix);
            
            const violation = findClosestViolation(transformedPoint.x, transformedPoint.y);
            if (violation) {{
                // Show tooltip in a fixed position
                tooltip.style.display = 'block';
                tooltip.style.left = (e.pageX + 10) + 'px';
                tooltip.style.top = (e.pageY + 10) + 'px';
                tooltip.innerHTML = violation.description;
                
                // Keep tooltip visible, will be hidden on next mousemove or mouseleave
            }}
        }});
    </script>
</body>
</html>
"""
    
    # Write the HTML file
    with open(output_html_file, 'w') as f:
        f.write(html_content)
    
    print(f"HTML visualization created at {output_html_file}")
    print(f"Found {len(violations)} DRC violations")

def visualize_drc_violations(json_file: str, svg_file: str, output_html_file: str):
    """
    Main function to coordinate the process of creating an HTML visualization
    
    Args:
        json_file (str): Path to the JSON file containing DRC violations
        svg_file (str): Path to the SVG file
        output_html_file (str): Path to save the HTML file
    """
    # Step 1: Parse the JSON to get violation positions and descriptions
    violations = parse_drc_violations(json_file)
    
    # Step 2: Create HTML file with embedded SVG and tooltips
    create_html_with_svg(svg_file, violations, output_html_file)

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) != 4:
        print("Usage: python script.py <json_file> <svg_file> <output_html_file>")
        print("Example: python script.py Test.json Test_with_violations.svg Test_visualization.html")
        sys.exit(1)
    
    json_file = sys.argv[1]
    svg_file = sys.argv[2]
    output_html_file = sys.argv[3]
    
    visualize_drc_violations(json_file, svg_file, output_html_file)