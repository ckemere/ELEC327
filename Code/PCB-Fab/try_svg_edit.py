import json
import xml.etree.ElementTree as ET
import math

# File paths
json_file = "Test.json"
svg_file = "Test.svg"
output_svg_file = "Test_modified.svg"

# Load JSON data
with open(json_file, "r") as f:
    data = json.load(f)

# Extract violation positions
positions = []
for violation in data.get("violations", []):
    for item in violation.get("items", []):
        pos = item.get("pos", {})
        if "x" in pos and "y" in pos:
            positions.append((pos["x"], pos["y"]))

# Load SVG file
ET.register_namespace("", "http://www.w3.org/2000/svg")  # Preserve namespace
svg_tree = ET.parse(svg_file)
svg_root = svg_tree.getroot()

# Namespace handling
ns = {'svg': "http://www.w3.org/2000/svg"} if '}' in svg_root.tag else {}

# Add circles to SVG
# for x, y in positions:
#     circle = ET.Element("circle", {
#         "cx": str(x),
#         "cy": str(y),
#         "r": "3",  # Radius in mm
#         "stroke": "red",
#         "stroke-width": "0.25",
#         "fill": "none"
#     })
#     svg_root.append(circle)

# Add arrow heads to SVG
# for x, y in positions:
#     arrow = ET.Element("path", {
#         "d": f"M{x-3},{y+5} L{x},{y} L{x+3},{y+5} M{x-1},{y+5} L{x+1},{y+5}",  # 30-degree arrowhead with a tail
#         "stroke": "red",
#         "stroke-width": "1.5",
#         "fill": "none"
#     })
#     svg_root.append(arrow)

#
# # Function to rotate a point around another point
# def rotate_point(x, y, cx, cy, angle):
#     radians = math.radians(angle)
#     cos_a = math.cos(radians)
#     sin_a = math.sin(radians)
#     dx = x - cx
#     dy = y - cy
#     new_x = cx + dx * cos_a - dy * sin_a
#     new_y = cy + dx * sin_a + dy * cos_a
#     return new_x, new_y

# # Add correctly positioned arrows to SVG
# angle = 45  # Rotate by 45 degrees
# scale = 0.25  # Reduce size to 1/4th
# stroke_width = "0.375"  # Reduce stroke width to 1/4th
# arrow_length = 5 * scale  # Adjusted length of the arrow tail
# arrow_width = 3 * scale  # Adjusted width of the arrowhead

# def create_arrow(x, y):
#     tail_x, tail_y = rotate_point(x, y + arrow_length, x, y, angle)  # Tail end rotated
#     left_x, left_y = rotate_point(x - arrow_width, y + arrow_length, x, y, angle)
#     right_x, right_y = rotate_point(x + arrow_width, y + arrow_length, x, y, angle)
    
#     return ET.Element("path", {
#         "d": f"M{tail_x},{tail_y} L{x},{y} L{left_x},{left_y} M{x},{y} L{right_x},{right_y}",  # Properly aligned arrow
#         "stroke": "red",
#         "stroke-width": stroke_width,
#         "fill": "none"
#     })

# for x, y in positions:
#     svg_root.append(create_arrow(x, y))


# Add Unicode rightward arrows to SVG
for x, y in positions:
    text = ET.Element("text", {
        "x": str(x),
        "y": str(y),
        "fill": "red",
        "font-size": "2"
    })
    text.text = "→"  # Unicode rightward arrow
    svg_root.append(text)

# Save modified SVG
svg_tree.write(output_svg_file)
print(f"Modified SVG saved as {output_svg_file}")

