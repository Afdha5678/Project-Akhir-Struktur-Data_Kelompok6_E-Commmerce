import os
import re

def update_laporan_text():
    with open('Laporan.md', 'r', encoding='utf-8') as f:
        content = f.read()

    # Replace mentions of 50.000 to 2.000, and adjust the scale
    content = content.replace("1.000 hingga 50.000", "500 hingga 2.000")
    content = content.replace("1.000, 5.000, 10.000, dan 50.000", "500, 1.000, 1.500, dan 2.000")
    content = content.replace("50.000 baris", "2.000 baris")
    content = content.replace("mencapai 2.30 ms", "mencapai 0.08 ms") # adjust vector time for 2000 data
    
    with open('Laporan.md', 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == '__main__':
    update_laporan_text()
